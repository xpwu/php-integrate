<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/4
 * Time: 2:19 PM
 */

namespace Inte;

class RunTimeEnv {
  private static $runTimeEnv_ = null;
  public static function getInstance() {
    if (self::$runTimeEnv_ == null) {
      self::$runTimeEnv_ = new RunTimeEnv();
    }

    return self::$runTimeEnv_;
  }

  private function __construct() {
    $this->runTime_ = new NoRunTime();
  }

  public function getRunTime():RunTime {
    return $this->runTime_;
  }

  public function setRunTime(RunTime $runTime) {
    $this->runTime_ = $runTime;
  }

  private $runTime_;
}