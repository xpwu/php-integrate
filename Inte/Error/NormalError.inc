<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/24
 * Time: 7:53 PM
 */

namespace Inte\Error;


class NormalError extends \Error {
  public function __construct(string $message, int $exitStatus = -1) {
    parent::__construct($message, $exitStatus);
  }

  public function __toString():string {
    return "Error: " . $this->getMessage() . PHP_EOL;
  }

  public function getExitStatus():int {
    return parent::getCode();
  }
}