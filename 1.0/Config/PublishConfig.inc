<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 2:40 PM
 */

namespace Inte\Config;


class PublishConfig {
  public $repo;
  public $name;
  public $version;

  /**
   * @var array [string]
   */
  public $annotationServices = [];
}