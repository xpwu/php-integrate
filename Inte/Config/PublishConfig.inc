<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 2:40 PM
 */

namespace Inte\Config;


class PublishConfig implements \Inte\Annotation\PublishConfig {

  function __construct(array $publish) {
    $this->repo = $publish['repo']??"";
    $this->name = $publish['name']??"";
    $this->version = $publish['version']??"0.0";
    $this->annotationServices = $publish["annotation_services"]??[];
  }

  public $repo;
  public $name;
  public $version;

  /**
   * @var array [string]
   */
  public $annotationServices = [];

  function getRepo(): string {
    return $this->repo;
  }

  function getName(): string {
    return $this->name;
  }

  function getVersion(): string {
    return $this->version;
  }

  function getAnnotationServices(): array {
    return $this->annotationServices;
  }

  public function isValid():bool {
    if ($this->repo === null || $this->repo === ""
      || $this->name === null || $this->name === ""
      || $this->version === null || $this->version === "") {
      return false;
    }

    return true;
  }
}