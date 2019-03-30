<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/25
 * Time: 8:33 PM
 */

namespace Inte\Dependence;


use Inte\ClassLoader;
use Inte\Dependence;
use Inte\DependenceSpec;
use Inte\Error\NormalError;
use Inte\FileFinder;

class LibNameVersion implements Dependence {

  static public function isThis(string $repo, string $dependenceStr):bool {
    return strpos($dependenceStr, ":") !== false;
  }

  function __construct(string $repo, string $lib_version) {
    $this->repo_ = $repo;
    $arr = explode(":", $lib_version);
    $this->libName_ = $arr[0];
    $this->version_ = $arr[1];

    // check
    $spec = new DependenceSpec($this->getTopDir());
    $spec->checkIsADependence();
  }

  // todo: 后面增加库文件下载与上传处理器，路径应该由此处理器提供

  function getTopDir(): string {
    return $this->repo_ . DIRECTORY_SEPARATOR
      . $this->libName_ . DIRECTORY_SEPARATOR . $this->version_;
  }

  function getRepo():string {
    return $this->repo_;
  }

  /**
   * @return array  [className=>file]
   */
  function getClasses(): array {
    $classLoader = new ClassLoader($this->getTopDir(), $this->getName());
    return $classLoader->read();
  }

  function getRequireFile(): array {
    return [];
  }

  /**
   * @return array [Dependence]
   */
  function getSubDependencies(): array {
    if (isset($this->subDependencies_)) {
      return $this->subDependencies_;
    }

    $spec = new DependenceSpec($this->getTopDir());
    $this->subDependencies_ = [];

    foreach ($spec->read()[DependenceSpec::DependenciesKey] as $value) {
      $this->subDependencies_[] = Factory::getDependence($this->getRepo(), $value);
    }

    return $this->subDependencies_;
  }

  function getAllFiles(): array {
    $fileFinder = new FileFinder($this->getTopDir(), ['**']);
    return $fileFinder->getAllFile();
  }

  function getName(): string {
    return $this->libName_;
  }

  function getVersion(): string {
    return $this->version_;
  }

  private $repo_;
  private $libName_;
  private $version_;

  private $subDependencies_ = null;
}