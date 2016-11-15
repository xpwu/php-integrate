<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 15/11/10
 * Time: 下午12:09
 */

require_once ("Integrater.inc");
require_once ("Utils.inc");

class Main {
  static public function run() {
    $opt = getopt("hm:t:i:p:s:");
    $mode = Utils::DEV_MODE;
    $topDirStr = "__PHP_INTEGRATE_TOP_DIR__";
    $integraterDir = '';
    $pharName = '';
    $testPharName = '';
    if (!self::parseOpt($opt, $mode, $topDirStr
      , $integraterDir, $pharName, $testPharName)) {
      return;
    }

    $integrater = new Integrater($mode, self::findSrcTopDir($topDirStr));
    if (!$integrater->integrate()) {
      return;
    }

    if ($integraterDir !== '') {
      if (! $integrater->outputTo($integraterDir)) {
        return;
      }
    }

    if ($pharName !== '') {
      if (! $integrater->phar($pharName)) {
        return;
      }
    }

    if ($testPharName !== '') {
      if (! $integrater->pharForTest($testPharName)) {
        return;
      }
    }

    echo "---success---\n";
  }

  static private function parseOpt($opt, &$mode, &$topDirStr
    , &$integraterDir, &$pharName, &$testPharName) {
    static $usage = <<<EOF
Usage:  option
  option: -h show this help;
          -i [optional] integrater Dir name, which is at the top dir.
          -p [optional] phar path name.
          -s [optional] test phar path name
          -m dev --devlopment mode; pro --production mode; default is 'dev';
          -t [optional] top dir flag file name, default is __PHP_INTEGRATE_TOP_DIR__;
  NOTE THAT: -i -p -s MUST set one.
EOF;

    if (array_key_exists('h', $opt) && $opt['h'] === false) {
      echo $usage.PHP_EOL;
      return false;
    }
    if (array_key_exists('m', $opt)) {
      if ($opt['m'] === 'dev') {
        $mode = Utils::DEV_MODE;
      } elseif ($opt['m'] === 'pro') {
        $mode = Utils::PRO_MODE;
      } else {
        echo $usage.PHP_EOL;
        return false;
      }
    }
    if (array_key_exists('t', $opt)) {
      $topDirStr = $opt['t'];
    }
    if (array_key_exists('i', $opt)) {
      $integraterDir = $opt['i'];
    }
    if (array_key_exists('p', $opt)) {
      $pharName = $opt['p'];
    }
    if (array_key_exists('s', $opt)) {
      $testPharName = $opt['s'];
    }
    if (!array_key_exists('i', $opt) && !array_key_exists('p', $opt)
        && !array_key_exists('s', $opt)) {
      echo $usage.PHP_EOL;
      return false;
    }
    return true;
  }

  static private function findSrcTopDir($topDirStr) {
    $pwdori = getcwd();
    $pwd = getcwd();
    while (!is_file($pwd.'/'.$topDirStr)) {
      chdir('..');
      $pwd = getcwd();
    }
    chdir($pwdori);
    return $pwd;
  }

  function __construct() {
    // TODO: Implement __construct() method.
  }

}

Main::run();