<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/5
 * Time: 12:36 AM
 */

namespace Inte;


use Inte\Dependence\RepoLib;

class PublishWriter {
  function __construct(string $rootDir, string $repo, string $name, string $version) {
    $this->rootDir_ = $rootDir;
    $this->repo_ = $repo;
    $this->name_ = $name;
    $this->version_ = $version;
  }

  public function write(array $classFileMap, array $dependencies = []
    , array $allServices = []) {

    $pwd = getcwd();
    chdir($this->rootDir_);

    $repoLib = new RepoLib($this->repo_, $this->name_, $this->version_);
    $dir = $repoLib->getTopDir();
    FileSystem::delAllDirAndFile($this->rootDir_ . DIRECTORY_SEPARATOR .$dir);
    @mkdir($dir, 0777, true);


    $classLoader = new ClassLoader($this->rootDir_, $dir, $this->name_);
    $classLoader->write($classFileMap);

    $spec = new DependenceSpec($this->rootDir_, $dir);
    $spec->write($this->name_, $this->version_
      , $dependencies, $allServices);

    foreach ($classFileMap as $value) {
      $dist = $dir .DIRECTORY_SEPARATOR. FileSystem::escape($value);
      @mkdir(dirname($dist), 0777, true);
      copy($value, $dist);
    }

    chdir($pwd);
  }

  private $rootDir_;
  private $repo_;
  private $name_;
  private $version_;
}