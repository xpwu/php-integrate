<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/4
 * Time: 3:28 PM
 */

namespace Inte;


class SimpleRunTime implements RunTime {

  function __construct(string $pharDir, string $pharName, array $allClasses) {
    $this->pharDir_ = $pharDir;
    $this->pharName_ = $pharName;
    $this->allClasses_ = $allClasses;
  }

  public function getPharDir():string {
    return $this->pharDir_;
  }

  public function getPharBaseName():string {
    return $this->pharName_;
  }

  public function getAllClassesOfSelf(): array {
    return $this->allClasses_;
  }

//  public function setPharDir(string $dir) {
//    $this->pharDir_ = $dir;
//  }
//
//  public function setPharName(string $name) {
//    return $this->pharName_ = $name;
//  }

  private $pharDir_;
  private $pharName_;
  private $allClasses_;

}