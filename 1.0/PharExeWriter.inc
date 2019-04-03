<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/3
 * Time: 2:35 PM
 */

namespace Inte;


class PharExeWriter extends PharWriter {

  function __construct($fullName) {
    $baseName = basename($fullName, ".phar") . "_exe";
    $dir = dirname($fullName);
    parent::__construct($dir.DIRECTORY_SEPARATOR.$baseName);

    $this->oldName_ = $dir.DIRECTORY_SEPARATOR.$baseName . ".phar";
    $this->name_ = substr($this->oldName_, -1*strlen("_exe.phar"));
  }

  protected function getEnv():string {
    return "#!/usr/bin/env php" . PHP_EOL;
  }

  protected function postSetStub() {
    rename($this->oldName_, $this->name_);
    chmod($this->name_, 0777);
  }

  private $oldName_;
  private $name_;
}