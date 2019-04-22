<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/2
 * Time: 7:01 PM
 */

namespace Inte\Dependence;


abstract class Position {
  function __construct(string $repo, string $name, string $version) {
    $this->repo_ = $repo;
    $this->name_ = $name;
    $this->version_ = $version;
  }

  abstract function getRootDir():string ;

  abstract function getAllNeedFilesForInte(Content $content):array ;

  abstract function getTopDir():string ;

  abstract function getAbsoluteTopDir():string ;

  protected function getRelativePathOfRepo():string {
    return $this->name_ . DIRECTORY_SEPARATOR . $this->version_;
  }

  public function getName():string {
    return $this->name_;
  }

  public function getVersion():string {
    return $this->version_;
  }

  public function getRepo():string {
    return $this->repo_;
  }

  protected $repo_;
  protected $name_;
  protected $version_;

}