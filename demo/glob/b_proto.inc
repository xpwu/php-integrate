<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2016/11/16
 * Time: 下午7:47
 */
class b_proto {
  function __construct() {
    echo 'b_proto'.PHP_EOL;
    new c_proto();
  }
}