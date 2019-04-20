<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2016/11/16
 * Time: 下午7:39
 */

namespace Demo;

use PublishDemo\ClassA;

class Main {
  static public function main() {
    new \Demo\CGI();
    new \Demo\SubSubCGI();
    new ClassA();
  }
}
