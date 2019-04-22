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

class Main {
  private function prepare() {
    date_default_timezone_set('PRC'); // 设置时区

    if (!function_exists("pcntl_fork")) {
      throw new NormalError("must load extension : 'pcntl' ");
    }

    if (!\Phar::canWrite()) {
      throw new NormalError("'phar.readonly' must be'false' in php.ini ");
    }

    set_error_handler(function (int $errno, string $errstr
      , string $errfile, int $errline){
      /**
       * https://www.php.net/manual/zh/language.operators.errorcontrol.php
       * 错误控制运算符：@ 标注的忽略错误不再输出
       */
      if (error_reporting() === 0) {
        return false;
      }
      throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
    });
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
      echo PHP_EOL;
      exit(0);
    } catch (NormalError $error) {
      echo $error;
      echo PHP_EOL;
      exit($error->getExitStatus());
    }
  }
}
