<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/24
 * Time: 2:27 PM
 */

namespace Inte\Command;

use Inte\ClassLoader;
use Inte\Command;
use Inte\Dependence;
use Inte\Error\NormalError;
use Inte\Project;
use Phar;

class Build extends Command {

  protected function output(): void {
    /**
     * 1、生成整体的classLoader 包括dependence的
     * 2、加入所有的文件，包括dependence的
     * 3、生成stub文件  设置include 还有各个依赖的repo路径也需要包括在include中
     * 4、写phar  加入 index
     */
    $project = Project::getInstance();

    // todo: 检查类是否重复定义
    $allClass = $this->allProjectClasses_;
    /**
     * @var $value Dependence
     */
    foreach ($project->getDependenceProcessor()->getMergeResult() as $value) {
      $allClass = array_merge($allClass, $value->getClasses());
    }

    $classLoader = new ClassLoader($project->getBuildOutputTempDir());
    $classLoader->write($allClass);

    $includes = [];
    $requireFiles = [];
    /**
     * @var $value Dependence
     */
    foreach ($project->getDependenceProcessor()->getMergeResult() as $value) {
      $includes[] = $value->getTopDir();
      $requireFiles = array_merge($requireFiles, $value->getRequireFile());
    }

    $includes[] = $project->getBuildOutputTempDir();
    $includes[] = $project->getAnnotationOutputDir();

    // classLoader 必须require
    $requireFiles[] = $classLoader->getFile();

    if (!Phar::canWrite()) {
      throw new NormalError("phar can not write!");
    }

    $pharPath = $project->getConfig()->buildConfig->target;
    $pharName = basename($pharPath);
    $ph = new Phar("$pharPath.phar", 0, $pharName);
    $this->appendFiles($ph, $classLoader);
    $this->setStub($ph, $includes, $requireFiles
      , $classLoader->getArrayVarString());
  }

  private function appendFiles(Phar $ph, ClassLoader $classLoader) {
    $project = Project::getInstance();
    $files = [];

    /**
     * @var $value Dependence
     */
    foreach ($project->getDependenceProcessor()->getMergeResult() as $value) {
      $files[] = $value->getAllFiles();
    }
    $files = array_merge($files, array_values($this->allProjectClasses_));
    $files[] = $classLoader->getFile();
    $files[] = $project->getConfig()->buildConfig->index;

    $ph->startBuffering();

    foreach ($files as $file) {
      if (substr($file, 0, 2) === './') {
        $file = substr($file, 2);
      }
      $ph[$file] = file_get_contents($file);
    }

    $ph->stopBuffering();
  }

  private function setStub(Phar $ph, array $includes
    , array $requireFiles, string $classLoaderArrayStr):void {
    $inc = 'phar://.__FILE__'.DIRECTORY_SEPARATOR.PATH_SEPARATOR;
    foreach ($includes as $include) {
      $inc = $inc . 'phar://.__FILE__.'
        .DIRECTORY_SEPARATOR. $include .PATH_SEPARATOR;
    }

    $index = Project::getInstance()->getConfig()->buildConfig->index;

    $req = "";
    foreach ($requireFiles as $requireFile) {
      $req = $req . "require_once($requireFile);".PHP_EOL;
    }

    $stub = <<<EOF
<?php
  Phar::interceptFileFuncs();
  Phar::mapPhar();

  set_include_path($inc . get_include_path());

  $req
  
  spl_autoload_register( function(\$className) {
      if (array_key_exists(\$className, $classLoaderArrayStr)) {
        require_once (${classLoaderArrayStr}[\$className]);
        return true;
      }

      return false;
    });
  
  require_once("$index");
  __HALT_COMPILER(); ?>
EOF;

    $ph->setStub($stub);

  }
}