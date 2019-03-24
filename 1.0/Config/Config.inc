<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 12:29 PM
 */

namespace Inte\Config;


class Config {
  function __construct(string $confFile) {
  }

  /**
   * @var array [string]
   */
  public $srcDir = [];

  /**
   * @var array [string]
   */
  public $annotationProcessors = [];

  /**
   * @var array [string]
   */
  public $dependencies = [] ;

  /**
   * @var PublishConfig
   */
  public $publishConfig = null;

  /**
   * @var BuildConfig
   */
  public $buildConfig = null;
}