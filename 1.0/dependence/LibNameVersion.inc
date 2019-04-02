<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/25
 * Time: 8:33 PM
 */

namespace Inte\Dependence;


use Inte\ClassLoader;
use Inte\DependenceSpec;
use Inte\FileFinder;

class LibNameVersion extends Content {

  function __construct(Position $position, string $name, bool $isAnno = false) {
    parent::__construct($position);

    $this->name_ = $name;
    // check
    if (!$isAnno) {
      $spec = new DependenceSpec($this->getPosition()->getTopDir());
      $spec->checkIsADependence();
    }
  }

  /**
   * @return array  [className=>file]
   */
  function getClasses(): array {
    $classLoader = new ClassLoader($this->getPosition()->getTopDir(), $this->name_);
    return array_map(function ($file){
      return $this->getPosition()->getTopDir() . DIRECTORY_SEPARATOR . $file;
    }, $classLoader->read());
  }

  function getRequireFiles(): array {
    return [];
  }

  /**
   * @return array [Dependence]
   */
  function getSubDependencies(string $repo): array {
    if (isset($this->subDependencies_)) {
      return $this->subDependencies_;
    }

    $spec = new DependenceSpec($this->getPosition()->getTopDir());
    $this->subDependencies_ = [];

    foreach ($spec->read()[DependenceSpec::DependenciesKey] as $value) {
      $this->subDependencies_[] = Factory::getDependence($repo, $value);
    }

    return $this->subDependencies_;
  }

  function getAllNeedFilesForInte(): array {
    $fileFinder = new FileFinder($this->getPosition()->getTopDir()
      , ['**'], [DependenceSpec::getFileName()]);

    return array_map(function ($file){
      return $this->getPosition()->getTopDir(). DIRECTORY_SEPARATOR . $file;
    }, $fileFinder->getAllFile());
  }

  private $name_ = "";

  private $subDependencies_ = null;
}