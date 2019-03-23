<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 6:04 PM
 */

namespace Inte;


class Logger {

  public function __construct(LogStatus $status) {
    $this->status = $status;
  }

  public function error(string $msg):void {
    $this->hasError = true;
    echo "ERROR: " . $msg . PHP_EOL;
  }

  public function warn(string $msg):void {
    echo "WARNING: " . $msg . PHP_EOL;
  }

  public function info(string $msg):void {
    echo "INFO: " . $msg . PHP_EOL;
  }

//  public function debug(string $msg):void {
//    echo "DEBUG: " . $msg . PHP_EOL;
//  }

  private $status;
}