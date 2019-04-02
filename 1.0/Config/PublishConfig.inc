<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 2:40 PM
 */

namespace Inte\Config;


class PublishConfig {

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
}