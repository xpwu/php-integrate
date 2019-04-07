<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2016/11/16
 * Time: 下午6:31
 */

namespace Subdir1;


class Test {
  function __construct() {
    echo 'Subdir1\Test'.PHP_EOL;
    new Subdir11\Test();
    new Subdir12\Test();
  }
}