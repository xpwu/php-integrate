<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2016/11/17
 * Time: 上午12:03
 */

namespace ThirdParty;


class Test {
  function __construct() {
    echo 'ThirdParty\Test'.PHP_EOL;
    new \MyMap();
  }
}