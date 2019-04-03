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
    $this->buildDir = $build_dir??"build";
    if ($this->buildDir == "") {
      $this->buildDir = "build";
    }
    $this->src = $src??[];
    $this->excludeSrc = $exclude_src??[];
    $this->annoAndDepRepo = $anno_and_dep_repo??"";
    $this->annotationProcessors = $annotation_processors??[];
    $this->dependencies = $dependencies??[];

    $this->checkArrayOrNull($this->src);
    $this->checkArrayOrNull($this->excludeSrc);
    $this->checkArrayOrNull($this->annotationProcessors);
    $this->checkArrayOrNull($this->dependencies);

    $this->checkStringOrNull($this->annoAndDepRepo);

    $this->publishConfig = new PublishConfig($publish??[]);
    $this->buildConfig = new BuildConfig($build??[]);
  }

  // 如果类型不对  会自动报错  故没有写实现
  private function checkArrayOrNull(?array $a) {}
  private function checkStringOrNull(?string $a) {}

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