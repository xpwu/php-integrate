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
    // todo: 初始化 build 目录

  }

  private function __construct() {
  }

  public function getConfig():Config {
    return $this->conf;
  }

  public function getDependenceProcessor():DependenceProcessor {
    return $this->dependenceProcessor;
  }

  public function getAnnotationDir():string {
    return $this->rootDir . Project::buildAnnotationDir;
  }

  /**
   * @var DependenceProcessor
   */
  private $dependenceProcessor;

  /**
   * @var Config
   */
  private $conf;


  private $annotationProcessor = [];

  /**
   * @var DependenceProcessor
   */
  private $annotationDependenceProcessor;

  private $rootDir = "";

  private const buildDir = DIRECTORY_SEPARATOR . "build"
      . DIRECTORY_SEPARATOR . "inte";
  private const buildAnnotationDir = Project::buildDir
          . DIRECTORY_SEPARATOR . "annotation";
}