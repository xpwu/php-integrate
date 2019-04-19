<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 11:07 PM
 */

namespace Inte;

use Inte\Dependence\Factory;
use Inte\Dependence\LibNameVersion;

class AnnotationProcessor {
  function __construct(string $repo, string $annotationProcessor) {
    $this->repo_ = $repo;
    $arr = explode(":", $annotationProcessor);
    $this->name_ = $arr[0];
    $this->version_ = $arr[1];

    $this->self_ = Factory::convertFromAnnoProcessor($this->repo_
      , $this->name_, $this->version_);

    $spec = new DependenceSpec(Project::getInstance()->getRootDir()
      , $this->self_->getTopDir());
    $spec->checkIsAAnnotationProcessor();

  }

  /**
   * @return string[]
   */
  public function getServersString(): array {
    $spec = new DependenceSpec(Project::getInstance()->getRootDir()
      , $this->getTopDir());
    return $spec->read()[DependenceSpec::AnnotationServerKey];
  }

  /**
   * @return Dependence[]
   */
  public function getDependencies(): array {
    $spec = new DependenceSpec(Project::getInstance()->getRootDir()
      , $this->getTopDir());
    $dependenciesStr = $spec->read()[DependenceSpec::DependenciesKey];
    $ret = [];
    foreach ($dependenciesStr as $value) {
      $ret[] = Factory::getDependence($this->repo_, $value);
    }

    return $ret;
  }

  public function convertToDependence():Dependence {
    return $this->self_;
  }

  private function getTopDir(): string {
    return $this->self_->getTopDir();
  }

  /**
   * @var Dependence
   */
  private $self_;

  private $repo_;
  private $name_;
  private $version_;

}