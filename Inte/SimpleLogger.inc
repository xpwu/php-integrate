<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 6:04 PM
 */

namespace Inte;


use Inte\Annotation\Logger;
use Inte\Error\NormalError;

class SimpleLogger implements Logger {

  public function throwError(string $msg):void {
//    echo "ERROR: " . $msg . PHP_EOL;
    throw new NormalError($msg);
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
}