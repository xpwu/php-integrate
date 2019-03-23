<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 6:27 PM
 */

namespace Inte;


class LogStatus {
  public function hasError():bool {
    return $this->hasError;
  }

  public function setError(bool $error) {
    $this->hasError = $error;
  }

  private $hasError = false;
}