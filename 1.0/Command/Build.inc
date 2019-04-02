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
use Inte\Error\NormalError;
use Inte\Project;
use Phar;
use ReflectionClass;

class Build extends Command {

  protected function postParseFile(array $classMap
    , DependenceProcessor $processor): void {
    /**
     * 1、生成整体的classLoader 包括dependence的
     * 2、加入所有的文件，包括dependence的
     * 3、生成stub文件  设置include 还有各个依赖的repo路径也需要包括在include中
     * 4、写phar  加入 index
     */
    $project = Project::getInstance();
    $this->processor_ = $processor;

    // 准备 classFileMap

    /**
     * @var $value Dependence
     */
    foreach ($this->processor_->getMergeResult() as $value) {
      $classMap = array_merge($classMap, $value->getClasses());
    }
    $classMap = array_map(function ($value){
      if (substr($value, 0, 2) === './') {
        return substr($value, 2);
      }
      return $value;
    }, $classMap);

    $classLoader = new ClassLoader(".");
    $classLoader->write($classMap);

    // 准备requireFiles

    $requireFiles = [];
    /**
     * @var $value Dependence
     */
    foreach ($this->processor_->getMergeResult() as $value) {
      $requireFiles = array_merge($requireFiles, $value->getRequireFiles());
    }

    // classLoader 必须require
    $requireFiles[] = $classLoader->getFile();
    $requireFiles = array_map(function ($value){
      if (substr($value, 0, 2) === './') {
        return substr($value, 2);
      }
      return $value;
    }, $requireFiles);

    // 写入phar

    if (!Phar::canWrite()) {
      throw new NormalError("phar can not write!");
    }

    $pharPath = $project->getConfig()->buildConfig->target;

    $pharName = basename($pharPath);
    $ph = new Phar("$pharPath.phar", 0, $pharName);

    $this->appendFiles($ph, $classLoader);
    $this->setStub($ph, $requireFiles
      , $classLoader->getArrayVarString());
  }

  private function appendFiles(Phar $ph, ClassLoader $classLoader) {
    $project = Project::getInstance();
    $files = [];

    /**
     * @var $value Dependence
     */
    foreach ($this->processor_->getMergeResult() as $value) {
      $files[] = $value->getAllFiles();
    }
    $files = array_merge($files, array_values($this->classFileMap_));
    $files[] = $classLoader->getFile();

    $ph->startBuffering();

    foreach ($files as $file) {
      if (substr($file, 0, 2) === './') {
        $file = substr($file, 2);
      }
      $ph[$file] = file_get_contents($file);
    }

    $ph->stopBuffering();
  }

  private function setStub(Phar $ph
    , array $requireFiles, string $classLoaderArrayStr):void {

    // index class 中必须存在 static 的 main 方法
    $index = Project::getInstance()->getConfig()->buildConfig->index_class;
    $indexRef = new ReflectionClass($index);
    try {
      $method = $indexRef->getMethod('main');
      if (!$method->isStatic()) {
        throw new NormalError(
          "'main' is not a static method, which supported by \$index_class ");
      }
    }catch (\ReflectionException $exception) {
      throw new NormalError($exception->getMessage());
    }

    $req = "";
    foreach ($requireFiles as $requireFile) {
      if (substr($requireFile, 0, 2) === './') {
        $requireFile = substr($requireFile, 2);
      }
      $req = $req . "require_once($requireFile);".PHP_EOL;
    }

    $stub = <<<EOF
<?php
  Phar::interceptFileFuncs();
  Phar::mapPhar();

  set_include_path('phar://.__FILE__'. PATH_SEPARATOR . get_include_path());

  $req
  
  spl_autoload_register( function(\$className) {
      if (array_key_exists(\$className, $classLoaderArrayStr)) {
        require_once (${classLoaderArrayStr}[\$className]);
        return true;
      }

      return false;
    });
  
  $index::main();
  __HALT_COMPILER(); ?>
EOF;

    $ph->setStub($stub);

  }

  private $classFileMap_;
  /**
   * @var DependenceProcessor
   */
  private $processor_;
}