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
use Inte\DependenceProcessor;
use Inte\PharWriter;
use Inte\Project;

class Build extends Command {

  protected function postParseFile(array $classMap
    , DependenceProcessor $processor): void {

    $this->printStep("build phar");

    /**
     * 1、生成整体的classLoader 包括dependence的
     * 2、加入所有的文件，包括dependence的
     * 3、生成stub文件  设置include 还有各个依赖的repo路径也需要包括在include中
     * 4、写phar  加入 index
     */
    $project = Project::getInstance();
    $this->processor_ = $processor;

    // 准备 classFileMap
    $this->classFileMap_ = $classMap;
    /**
     * @var $value Dependence
     */
    foreach ($this->processor_->getMergeResult() as $value) {
      $this->classFileMap_ = array_merge($this->classFileMap_
        , $value->getClasses());
    }
    $this->classFileMap_ = array_map(function ($value){
      if (substr($value, 0, 2) === './') {
        return substr($value, 2);
      }
      return $value;
    }, $this->classFileMap_);

    $classLoader = new ClassLoader($project->getRootDir(), $project->getBuildOutputDir());
    $classLoader->write($this->classFileMap_);

    // 准备requireFiles

    $requireFiles = [];
    /**
     * @var $value Dependence
     */
    foreach ($this->processor_->getMergeResult() as $value) {
      $requireFiles = array_merge($requireFiles, $value->getRequireFiles());
    }

    // 写入phar

    $pharPath = $project->getConfig()->buildConfig->getTarget();
    @mkdir(dirname($pharPath), 0777, true);

    $phWriter = new PharWriter($pharPath);
    $phWriter->write($this->getAllFiles(), $classLoader);
    $phWriter->setStub($classLoader
      , $project->getConfig()->buildConfig->getIndexClass()
      , $requireFiles);

  }

  private function getAllFiles():array {
    $files = [];

    /**
     * @var $value Dependence
     */
    foreach ($this->processor_->getMergeResult() as $value) {
      $files = array_merge($files, $value->getAllFiles());
    }
    $files = array_merge($files, array_values($this->classFileMap_));

    return $files;
  }


  private $classFileMap_;
  /**
   * @var DependenceProcessor
   */
  private $processor_;
}