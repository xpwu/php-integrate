<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 4:04 PM
 */

namespace Inte;


use Inte\Config\Config;

class Project {
  static private $instance_ = null;
  static function getInstance():Project {
    if (Project::$instance_ == null) {
      Project::$instance_ = new Project();
    }

    return Project::$instance_;
  }

  public function init(Config $config, string $rootDir) {
    $this->conf = $config;
    $this->rootDir = $rootDir;
    set_include_path($rootDir . PATH_SEPARATOR . get_include_path());
  }

  private function __construct() {
  }

  public function getConfig():Config {
    return $this->conf;
  }

  public function getAnnotationOutputDir():string {
    $ret = $this->conf->buildDir . DIRECTORY_SEPARATOR . self::buildAnnotationDir;
    if (!file_exists($ret)) {
      mkdir($ret, 0777, true);
    }
    return $ret;
  }

//  public function getBuildOutputDir():string {
//
//  }

  public function getRootDir():string {
    return $this->rootDir;
  }

  /**
   * @var Config
   */
  private $conf;

  private $rootDir = "";

  private const buildDir = "inte";
  private const buildAnnotationDir = self::buildDir
          . DIRECTORY_SEPARATOR . "annotation";
}