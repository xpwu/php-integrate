<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/3
 * Time: 1:33 AM
 */


/**
 *
 * todo: 需要对路径信息 做一个更好的规划
 *
 *
 */


namespace Inte;

class InteBuilder {

  private const preRequire = [
    "SourceParser.inc",
    "FileFinder.inc",
    "ClassLoader.inc"
  ];

  public function prepare() {
    foreach (self::preRequire as $file) {
      $beforeClass = get_declared_classes();
      $beforeInterface = get_declared_interfaces();
      require_once $file;
      $afterClass = get_declared_classes();
      $afterInterface = get_declared_interfaces();

      $diffClass = array_diff($afterClass, $beforeClass);
      $diffInterface = array_diff($afterInterface, $beforeInterface);

      foreach ($diffClass as $className) {
        $this->result_[$className] = $file;
      }
      foreach ($diffInterface as $inter) {
        $this->result_[$inter] = $file;
      }
    }
  }

  private function firstInte(ClassLoader $classLoader) {

    $finder = new FileFinder("."
      , ["**/*.inc", "*.inc"], [$classLoader->getFile()]);
    $allFiles = $finder->getAllFile();

    $parser = new SourceParser($allFiles, true);
    $parser->parse();
    $result = $parser->getResult();

    $left = $parser->getNotParseFile();
    foreach ($left as $item) {
      require_once $item;
    }

    $this->result_ = array_merge($result, $this->result_);

    $classLoader->write($this->result_);
  }

  private function writeStubForDebug(ClassLoader $classLoader, string $indexClass) {

    $classLoaderArrayStr = $classLoader->getArrayVarString();
    $req = $classLoader->getFile();

    $content = <<<EOF
<?php

  set_include_path('__FILE__'. PATH_SEPARATOR . get_include_path());

  require_once "${req}";

  spl_autoload_register( function(\$className) {
      if (array_key_exists(\$className, $classLoaderArrayStr)) {
        require_once (${classLoaderArrayStr}[\$className]);
        return true;
      }

      return false;
    });
    
  Inte\RunTimeEnv::getInstance()->setRunTime(
    new Inte\SimpleRunTime(__DIR__, __FILE__, ${classLoaderArrayStr}));
  
  $indexClass::main();
  
EOF;

    file_put_contents("debugStub.php", $content);

  }

  private function getCoreLibClass(ClassLoader $loader, array $srcDir):array {

    /**
     * @var string[] $arr
     */
    $arr = $loader->read();

    $coreLib = [];

    foreach ($arr as $class=>$file) {
      foreach ($srcDir as $item) {
        $pos = strpos($file, $item);
        if ($pos === false || $pos != 0) {
          continue;
        }

        $coreLib[$class] = $file;
      }
    }

    return $coreLib;
  }

  static public function main() {
    $builder = new InteBuilder();
    $builder->prepare();

    $classLoader = new ClassLoader(".", ".", "__Inte__");

    $builder->firstInte($classLoader);
    $files = array_values($builder->result_);

    $phWriter = new PharWriter("../bin/phpinte");
    $phWriter->write($files, $classLoader);
    $phWriter->setStub($classLoader, Main::class);

    $phExeWriter = new PharExeWriter("../bin/phpinte");
    $phExeWriter->write($files, $classLoader);
    $phExeWriter->setStub($classLoader, Main::class);

    $builder->writeStubForDebug($classLoader, Main::class);

    $libCoreFile = $builder->getCoreLibClass($classLoader
      , ['Annotation']);
    $publishWriter = new PublishWriter(".", "../lib"
      , "InteCoreAnnotation", Version::str);
    $publishWriter->write($libCoreFile);

    $libCoreFile = $builder->getCoreLibClass($classLoader
      , ['RunTime']);
    $publishWriter = new PublishWriter(".", "../lib"
      , "InteCore", Version::str);
    $publishWriter->write($libCoreFile);

    echo PHP_EOL;
  }

  private $result_;
}

InteBuilder::main();

