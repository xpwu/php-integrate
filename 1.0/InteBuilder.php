<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/3
 * Time: 1:33 AM
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

    $parser = new SourceParser($allFiles);
    $parser->parse();
    $result = $parser->getResult();

    $this->result_ = array_merge($result, $this->result_);

    $classLoader->write($this->result_);
  }

  static public function main() {
    $builder = new InteBuilder();
    $builder->prepare();

    $classLoader = new ClassLoader(".", "Inte");

    $builder->firstInte($classLoader);
    $files = array_values($builder->result_);

    $phWriter = new PharWriter("phpinte");
    $phWriter->write($files, $classLoader);
    $phWriter->setStub($classLoader, Main::class);

    $phExeWriter = new PharExeWriter("phpinte");
    $phExeWriter->write($files, $classLoader);
    $phExeWriter->setStub($classLoader, Main::class);

    echo PHP_EOL;
  }

  private $result_;
}

InteBuilder::main();

