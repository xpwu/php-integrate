<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2016/11/16
 * Time: 下午6:32
 */

namespace Subdir1\Subdir11;


class Test {
  function __construct() {
    echo 'Subdir1\Subdir11\Test'.PHP_EOL;
    new Config();
  }
}