<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2017/3/9
 * Time: 下午6:02
 */

namespace Demo;


class SubSubCGI extends SubCGI{
  function __construct() {
    parent::__construct();
    echo "SubSubCGI\n";
  }
}