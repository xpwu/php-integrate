<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2016/11/16
 * Time: 下午6:26
 */

namespace Demo;

use Subdir1\Test;

class CGI {
  function __construct() {
    echo 'Demo\CGI'.PHP_EOL;
    new Test();
    new \Subdir2\Test();
    new \Glob\Test();
    new \ThirdParty\Test();
  }
}
