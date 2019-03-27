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
    // TODO: 初始化 各个变量
  }

  private function __construct() {
  }

  public function getConfig():Config {
    return $this->conf;
  }

  public function getDependenceProcessor():DependenceProcessor {
    return $this->dependenceProcessor;
  }

  public function getAnnotationOutputDir():string {
    return $this->rootDir . Project::buildAnnotationDir;
  }

  public function getAnnotationProcessor():array {
    return $this->annotationProcessor;
  }

  public function getAnnotationDependenceProcessor():DependenceProcessor {
    return $this->annotationDependenceProcessor;
  }

  public function getBuildOutputTempDir():string {

  }

  public function getRootDir():string {
    return $this->rootDir;
  }

  /**
   * @var DependenceProcessor
   */
  private $dependenceProcessor;

  /**
   * @var Config
   */
  private $conf;

  /**
   * @var array  [string]
   */
  private $projectFiles = [];


  /**
   * @var array [AnnotationProcessor]
   */
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