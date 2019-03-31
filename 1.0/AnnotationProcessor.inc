<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 11:07 PM
 */

namespace Inte;


use Inte\Annotation\AbstractAnnotationServer;
use Inte\Dependence\Factory;
use Inte\Dependence\LibNameVersion;

class AnnotationProcessor {
  function __construct(string $repo, string $annotationProcessor) {
    $this->repo_ = $repo;
    $arr = explode(":", $annotationProcessor);
    $this->name_ = $arr[0];
    $this->version_ = $arr[1];

    $spec = new DependenceSpec($this->getTopDir());
    $spec->checkIsAAnnotationProcessor();
  }

  /**
   * @return string[]
   */
  public function getServersString(): array {
    $spec = new DependenceSpec($this->getTopDir());
    return $spec->read()[DependenceSpec::AnnotationServerKey];
  }

  /**
   * @return Dependence[]
   */
  public function getDependencies(): array {
    $spec = new DependenceSpec($this->getTopDir());
    $dependenciesStr = $spec->read()[DependenceSpec::DependenciesKey];
    $ret = [];
    foreach ($dependenciesStr as $value) {
      $ret[] = Factory::getDependence($this->repo_, $value);
    }

    return $ret;
  }

  public function convertToDependence():Dependence {
    return new LibNameVersion($this->repo_
      , $this->name_.":".$this->version_, true);
  }

  // todo: 后面增加库文件下载与上传处理器，路径应该由此处理器提供

  private function getTopDir(): string {
    return $this->repo_ . DIRECTORY_SEPARATOR
      . $this->name_ . DIRECTORY_SEPARATOR . $this->version_;
  }


  private $repo_;
  private $name_;
  private $version_;

}