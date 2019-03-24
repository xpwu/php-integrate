<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/24
 * Time: 12:00 AM
 */

namespace Inte;

use Inte\Command\Command;

class Options {
  function __construct() {
    $option = getopt();
  }

  public function getCommand():Command {

  }

  public function getRootDir():string {

  }

  // 不带路径
  public function getConfigFileName():string {

  }
}