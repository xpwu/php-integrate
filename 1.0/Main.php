<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 11:47 PM
 */

namespace Inte;

use Inte\Config\Config;

class Main {
  private function prepare() {

  }

  static public function run() {
    $main = new Main();

    $main->prepare();

    $options = new Options();

    Project::getInstance()->init(new Config($options->getRootDir()
        . DIRECTORY_SEPARATOR . $options->getConfigFileName())
      , $options->getRootDir());


    exit($options->getCommand()->run()->toValue());
  }
}

Main::run();
