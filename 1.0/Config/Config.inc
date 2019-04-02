<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 12:29 PM
 */

namespace Inte\Config;


class Config implements \Inte\Annotation\Config {
  function __construct(string $confFile) {
    require_once $confFile;
    $this->buildDir = isset($build_dir)??"build";
    $this->src = isset($src)??[];
    $this->excludeSrc = isset($exclude_src)??[];
    $this->annoAndDepRepo = isset($anno_and_dep_repo)??"";
    $this->annotationProcessors = isset($annotation_processors)??[];
    $this->dependencies = isset($dependencies)??[];

    $this->publishConfig = new PublishConfig(isset($dependencies)??[]);
    $this->buildConfig = new BuildConfig(isset($build)??[]);
  }

  public $buildDir = "build";

  /**
   * @var array [string]
   */
  public $src = [];

  public $excludeSrc = [];

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

  function getBuildConfig(): \Inte\Annotation\BuildConfig {
    return $this->buildConfig;
  }

  function getPublishConfig(): \Inte\Annotation\PublishConfig {
    return $this->publishConfig;
  }
}