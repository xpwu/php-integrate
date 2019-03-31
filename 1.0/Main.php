<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 11:47 PM
 */

namespace Inte;

use Inte\Config\Config;
use Inte\Error\NormalError;
use Inte\Error\OK;
use Inte\Error\Warning;

class Main {
  private function prepare() {
    if (!function_exists("pcntl_fork")) {
      throw new NormalError("must load extension : 'pcntl' ");
    }

    if (!\Phar::canWrite()) {
      throw new NormalError("'phar.readonly' must be'false' in php.ini ");
    }
  }

  static public function main() {
    try {
      $main = new Main();

      $main->prepare();

      $options = new Options();

      Project::getInstance()->init(new Config($options->getRootDir()
          . DIRECTORY_SEPARATOR . $options->getConfigFileName())
        , $options->getRootDir());


      $options->getCommand()->run();

      echo "----SUCCESS----".PHP_EOL.PHP_EOL;
      exit(0);

    } catch (OK $error) {
      exit(0);
    } catch (NormalError $error) {
      echo $error;
      exit($error->getExitStatus());
    }
  }
}

Main::main();
