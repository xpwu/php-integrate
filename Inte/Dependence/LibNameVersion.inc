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


class LibNameVersion extends Content {

  function __construct(Position $position, bool $isAnno = false) {
    parent::__construct($position);

    $this->rootDir_ = $this->getPosition()->getRootDir();
    // check
    if (!$isAnno) {
      $spec = new DependenceSpec($this->rootDir_, $this->getPosition()->getTopDir());
      $spec->checkIsADependence();
    }
  }

  /**
   * @return array  [className=>file]
   */
  function getClasses(): array {
    $classLoader = new ClassLoader($this->rootDir_
      , $this->getPosition()->getTopDir(), $this->getPosition()->getName());
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

    $spec = new DependenceSpec($this->rootDir_, $this->getPosition()->getTopDir());
    $this->subDependencies_ = [];

    foreach ($spec->read()[DependenceSpec::DependenciesKey] as $value) {
      $this->subDependencies_[] = Factory::getDependence($repo, $value);
    }

    return $this->subDependencies_;
  }

  function getAllNeedFilesForInte(): array {
    return array_values($this->getClasses());
  }

  private $rootDir_;

  private $subDependencies_ = null;
}