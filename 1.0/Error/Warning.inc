<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/27
 * Time: 11:18 PM
 */

namespace Inte\Error;


class Warning extends \Error {
  public function __toString():string {
    return "Warning: " . $this->getMessage() . PHP_EOL;
  }
}