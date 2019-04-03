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

    $this->delDir($this->buildDir);

    set_include_path($rootDir . PATH_SEPARATOR . get_include_path());
  }

  private function __construct() {
  }

  public function getConfig():Config {
    return $this->conf;
  }

  public function getAnnotationOutputDir():string {
    $ret = $this->buildDir . DIRECTORY_SEPARATOR . self::buildAnnotationSubDir;
    if (!file_exists($ret)) {
      mkdir($ret, 0777, true);
    }
    return $ret;
  }

  public function getBuildOutputDir():string {
    $ret = $this->buildDir . DIRECTORY_SEPARATOR . 'dist';
    if (!file_exists($ret)) {
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

  private $rootDir = "";

  private $buildDir = "inte";

  private const buildAnnotationSubDir = "annotation";
}