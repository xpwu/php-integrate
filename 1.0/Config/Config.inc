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
    // todo: 全部都是相对路径
  }

  /**
   * @var array [string]
   */
  public $src = [];

  public $exclude_src = [];

  public $annoAndDepRepo = "";

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