<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2016/11/16
 * Time: 下午7:46
 */
class a_proto {
  function __construct() {
    echo 'a_proto'.PHP_EOL;
    new b_proto();
  }
}