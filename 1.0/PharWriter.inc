<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/3
 * Time: 1:50 PM
 */

namespace Inte;


use Inte\Error\NormalError;
use Phar;
use ReflectionClass;

class PharWriter {
  function __construct(string $fullName, bool $isExe = false) {
    if (!Phar::canWrite()) {
      throw new NormalError("phar can not write!");
    }

    $baseName = basename($fullName, ".phar");
    $dir = dirname($fullName);
    @unlink($dir.DIRECTORY_SEPARATOR.$baseName. ".phar");

    $this->ph_ = new Phar($dir.DIRECTORY_SEPARATOR.$baseName. ".phar"
      , 0, $baseName);
  }

  public function write(array $files, ClassLoader $classLoader) {
    $this->ph_->startBuffering();

    // classLoader 必须打包进去
    if (!in_array($classLoader->getFile(), $files, true)) {
      $files[] = $classLoader->getFile();
    }

    foreach ($files as $file) {
      if (substr($file, 0, 2) === './') {
        $file = substr($file, 2);
      }
      $this->ph_[$file] = file_get_contents($file);
    }

    $this->ph_->stopBuffering();
  }

  public function setStub(ClassLoader $classLoader, string $indexClass
    , array $requireFiles = []) {

    // index class 中必须存在 static 的 main 方法
    $indexRef = new ReflectionClass($indexClass);
    try {
      $method = $indexRef->getMethod('main');
      if (!$method->isStatic()) {
        throw new NormalError(
          "'main' is not a static method in 'index_class'($indexClass)");
      }
    }catch (\ReflectionException $exception) {
      throw new NormalError($exception->getMessage());
    }

    $classLoaderArrayStr = $classLoader->getArrayVarString();

    // classLoader 必须require
    if (!in_array($classLoader->getFile(), $requireFiles, true)) {
      $requireFiles[] = $classLoader->getFile();
    }

    $req = "";
    foreach ($requireFiles as $requireFile) {
      if (substr($requireFile, 0, 2) === './') {
        $requireFile = substr($requireFile, 2);
      }
      $req = $req . "require_once \"$requireFile\";".PHP_EOL;
    }

    $env = $this->getEnv();

    $stub = <<<EOF
$env<?php
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
  
  $indexClass::main();
  __HALT_COMPILER(); ?>
EOF;

    $this->ph_->setStub($stub);

    $this->postSetStub();

  }

  protected function getEnv():string {return "";}
  protected function postSetStub() {}

  /**
   * @var \Phar
   */
  private $ph_;
}