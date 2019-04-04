<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/4
 * Time: 3:28 PM
 */

namespace Inte;


class SimpleRunTime implements RunTime {

  function __construct(string $pharDir, string $pharName) {
    $this->pharDir_ = $pharDir;
    $this->pharName_ = $pharName;
  }

  public function getPharDir():string {
    return $this->pharDir_;
  }

  public function getPharName():string {
    return $this->pharName_;
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

}