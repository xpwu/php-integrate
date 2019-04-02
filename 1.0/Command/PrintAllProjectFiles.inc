<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/2
 * Time: 1:49 AM
 */

namespace Inte\Command;


use Inte\Error\OK;

class PrintAllProjectFiles {
  protected function postFindFile(array $files):void {
    echo implode(PHP_EOL, $files) . PHP_EOL;

    throw new OK();
  }
}