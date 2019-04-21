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
  function __construct(string $fullName) {
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
      // todo: 在自集成与集成其他包时 都要保证路径的正确. 后续需要更好的处理
      $this->ph_[$file] = file_get_contents(Project::getInstance()->getRootDir()
        . DIRECTORY_SEPARATOR . $file);
    }

    // runTime 必须打包
    $this->ph_[RunTimeTemplate::getRunTimeFileName()]
      = RunTimeTemplate::getContent();

    $this->ph_->stopBuffering();
  }

  public function setStub(ClassLoader $classLoader, string $indexClass
    , array $requireFiles = []) {

    // index class 中必须存在 static 的 main 方法
    try {
      $indexRef = new ReflectionClass($indexClass);
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
    // RunTime 必须提前require
    $requireFiles[] = RunTimeTemplate::getRunTimeFileName();

    $req = "";
    foreach ($requireFiles as $requireFile) {
      if (substr($requireFile, 0, 2) === './') {
        $requireFile = substr($requireFile, 2);
      }
      $req = $req . "require_once \"$requireFile\";".PHP_EOL . "\t";
    }

    $env = $this->getEnv();

    $stub = <<<EOF
$env<?php
  Phar::interceptFileFuncs();
  Phar::mapPhar();

  // phar 中的stub __FILE__  __DIR__ 前面不带 phar:// 描述符
  // phar 中的其他文件  __FILE__  __DIR__ 前面都带 phar:// 描述符

  // 这句的书写方式很重要  "phar://".__FILE__  不能写错
  set_include_path("phar://".__FILE__ . PATH_SEPARATOR . get_include_path());

  $req
  
  spl_autoload_register( function(\$className) {
      if (array_key_exists(\$className, $classLoaderArrayStr)) {
        require_once (${classLoaderArrayStr}[\$className]);
        return true;
      }

      return false;
    });
    
  
  Inte\RunTime::default()->setPharDir(__DIR__);
  Inte\RunTime::default()->setPharName(basename(__FILE__));
  Inte\RunTime::default()->setAllClassFileMapOfSelf(${classLoaderArrayStr});
  
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