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
    $this->buildDir = $this->conf->buildDir . DIRECTORY_SEPARATOR ."inte";

    $this->delDir($this->rootDir
      . DIRECTORY_SEPARATOR . $this->buildDir);

    $this->conf->src = FileSystem::preGlobArray($this->conf->src
      , FileSystem::dirDeep($this->rootDir));
    $this->conf->excludeSrc = FileSystem::preGlobArray($this->conf->excludeSrc
      , FileSystem::dirDeep($this->rootDir));

    set_include_path($rootDir . PATH_SEPARATOR . get_include_path());
  }

  private function __construct() {
  }

  public function getConfig():Config {
    return $this->conf;
  }

  public function getAnnotationOutputDir():string {
    $ret = $this->buildDir . DIRECTORY_SEPARATOR . self::buildAnnotationSubDir;
    // 因为 Phar::interceptFileFuncs(), 所以查找时需要加上 rootDir 变为绝对路径
    if (!file_exists($this->rootDir . DIRECTORY_SEPARATOR . $ret)) {
      mkdir($ret, 0777, true);
    }
    return $ret;
  }

  public function getBuildOutputDir():string {
    $ret = $this->buildDir . DIRECTORY_SEPARATOR . 'dist';
    // 因为 Phar::interceptFileFuncs(), 所以查找时需要加上 rootDir 变为绝对路径
    if (!file_exists($this->rootDir . DIRECTORY_SEPARATOR . $ret)) {
      mkdir($ret, 0777, true);
    }
    return $ret;
  }

  public function getRootDir():string {
    return $this->rootDir;
  }

  private function delDir(string $path){
    if(is_dir($path)){

      $p = scandir($path);
      foreach($p as $val){
        if ($val === "." || $val === "..") {
          continue;
        }

        $fullPath = $path. DIRECTORY_SEPARATOR. $val;

        if(is_dir($fullPath)){
          $this->delDir($fullPath);
          @rmdir($fullPath);
        }else{
          unlink($fullPath);
        }

      }
    }
  }

  /**
   * @var Config
   */
  private $conf;

  private $rootDir = ".";

  private $buildDir = "inte";

  private const buildAnnotationSubDir = "annotation";
}