<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/24
 * Time: 2:27 PM
 */

namespace Inte\Command;

use Inte\Annotation\AnnoCompilerStrategyOfCompileDependence;
use Inte\Annotation\AnnotationCompilerStrategyOfDependency;
use Inte\ClassLoader;
use Inte\Command;
use Inte\Dependence;
use Inte\DependenceProcessor;
use Inte\FileSystem;
use Inte\PharWriter;
use Inte\Project;

class Build extends Command {

  protected function postParseFile(array $classMap
    , DependenceProcessor $processor): void {

    $this->printStep("build phar");


    $project = Project::getInstance();
    $this->processor_ = $processor;
    $this->projectClassFileMap_ = $classMap;


    $classLoader = new ClassLoader($project->getRootDir()
      , $project->getBuildOutputDir());
    $classLoader->write($this->getAllClassMapForClassLoader());


    // 写入phar
    $pharPath = $project->getConfig()->buildConfig->getTarget();
    @mkdir(dirname($pharPath), 0777, true);

    $phWriter = new PharWriter($pharPath);
    $phWriter->write($this->getAllFiles(), $classLoader);
    $phWriter->setStub($classLoader
      , $project->getConfig()->buildConfig->getIndexClass()
      , $this->getAllRequireFiles());

  }

  protected function isBuild(): bool {
    return true;
  }

  private function getAllRequireFiles():array {
    // 准备requireFiles
    $requireFiles = [];
    /**
     * @var $value Dependence
     */
    foreach ($this->processor_->getMergeResult() as $value) {
      $requireFiles = array_merge($requireFiles, $value->getRequireFiles());
    }

    return $requireFiles;
  }

  private function getAllClassMapForClassLoader():array {
    // 准备 classFileMap for ClassLoader
    /**
     * @var $value Dependence
     */
    $classMapForClassLoader = $this->projectClassFileMap_;
    foreach ($this->processor_->getMergeResult() as $value) {
      $classMapForClassLoader = array_merge($classMapForClassLoader
        , $value->getClasses());
    }
    $classMapForClassLoader = array_map(function ($value){
      return FileSystem::escape($value);
    }, $classMapForClassLoader);

    return $classMapForClassLoader;
  }

  private function getAllFiles():array {
    $files = [];

    /**
     * @var $value Dependence
     */
    foreach ($this->processor_->getMergeResult() as $value) {
      $files = array_merge($files, $value->getAllFiles());
    }
    $files = array_merge($files, array_values($this->projectClassFileMap_));

    return $files;
  }


  private $projectClassFileMap_;
  /**
   * @var DependenceProcessor
   */
  private $processor_;
}