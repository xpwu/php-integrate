<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/24
 * Time: 2:18 PM
 */


namespace Inte;

use Inte\Error\NormalError;
use Inte\Error\Warning;

abstract class Command {
  public function run():void {
    $project = Project::getInstance();

    // 加载项目的依赖
    $depPro = $project->getDependenceProcessor();
    $depPro->mergeDependencies();
    $depPro->loadDependencies();

    // 加载本项目文件
    $fileFinder = new FileFinder($project->getRootDir(), $project->getConfig()->src
      , $project->getConfig()->exclude_src);
    $allProjectFile = $fileFinder->getAllFile();

    if (count($allProjectFile) == 0) {
      throw new Warning("Not find any file!");
    }

    //准备注解的依赖
    $annoDependence = $project->getAnnotationDependenceProcessor();
    $annoDependence->mergeDependencies();

    $annoCompiler = new AnnotationCompiler($project->getAnnotationProcessor()
      , $project->getAnnotationDependenceProcessor()
      , $project->getAnnotationOutputDir());

    /**
     * @var [class=>file]
     */
    $allClasses = [];

    $newFiles = $allProjectFile;

    while (count($newFiles) !== 0) {
      $sourceParser = new SourceParser($allProjectFile);
      $sourceParser->parse();
      $allProjectFile = $sourceParser->getNotParseFile();

      $classes = $sourceParser->getResult();
      $allClasses = array_merge($allClasses, $classes);

      // 处理注解
      $newFiles = $annoCompiler->compile($classes);
      for ($i = 0; $i < count($newFiles); ++$i) {
        $newFiles[$i] = $project->getAnnotationOutputDir()
          .DIRECTORY_SEPARATOR. $newFiles[$i];
      }

      $allProjectFile = array_merge($allProjectFile, $newFiles);

    }

    // 没有处理完
    if (count($allProjectFile) != 0) {
      // 输出编译错误
      $this->printPhpFileError($allProjectFile);
      throw new NormalError("-----Error!!!------");
    }

    $this->allProjectClasses_ = $allClasses;

    $this->output();
  }

  private function printPhpFileError($errorFiles) {
    foreach ($errorFiles as $errorFile) {
      require_once $errorFile;
    }
  }

  protected abstract function output():void ;

  /**
   * @var array
   */
  protected $allProjectClasses_;
}