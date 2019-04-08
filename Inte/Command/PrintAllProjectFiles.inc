<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/2
 * Time: 1:49 AM
 */

namespace Inte\Command;


use Inte\Command;
use Inte\Error\OK;

class PrintAllProjectFiles extends Command {
  protected function postFindFile(array $files):void {
    echo PHP_EOL . implode(PHP_EOL, $files) . PHP_EOL;

    echo PHP_EOL. count($files). " files" . PHP_EOL;

    throw new OK();
  }
}