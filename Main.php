#!/use/bin/env php

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
    $opt = getopt("hc:");

    $topDirStr = "integrate.conf.hphp";
//    $integraterDir = '';
//    $testPharName = '';
    if (!self::parseOpt($opt, $topDirStr)) {
      return;
    }
    $topDir = self::findSrcTopDir($topDirStr);

    $pharName = $topDir.'main';
    $mode = "dev";
    require_once ($topDir."/".$topDirStr);
    if (isset($type)) {$mode = $type;}
    if (isset($phar_name)) {$pharName = $phar_name;}
    if (isset($mk_name)) {Utils::$mk_name = $mk_name;}


    $integrater = new Integrater($mode, $topDir);
    if (!$integrater->integrate()) {
      return;
    }

//    if ($integraterDir !== '') {
//      if (! $integrater->outputTo($integraterDir)) {
//        return;
//      }
//    }

    if ($pharName !== '') {
      if (! $integrater->phar($pharName)) {
        return;
      }
    }

//    if ($testPharName !== '') {
//      if (! $integrater->pharForTest($testPharName)) {
//        return;
//      }
//    }

    echo "---success---\n";
  }

  static private function parseOpt($opt, &$topDirStr) {
    static $usage = <<<EOF
Usage:  option
  option: -h show this help;
          -c conf name.
EOF;

    if (array_key_exists('h', $opt) && $opt['h'] === false) {
      echo $usage.PHP_EOL;
      return false;
    }
//    if (array_key_exists('m', $opt)) {
//      if ($opt['m'] === 'dev') {
//        $mode = Utils::DEV_MODE;
//      } elseif ($opt['m'] === 'pro') {
//        $mode = Utils::PRO_MODE;
//      } else {
//        echo $usage.PHP_EOL;
//        return false;
//      }
//    }
    if (array_key_exists('c', $opt)) {
      $topDirStr = $opt['c'];
    }
//    if (array_key_exists('i', $opt)) {
//      $integraterDir = $opt['i'];
//    }
//    if (array_key_exists('p', $opt)) {
//      $pharName = $opt['p'];
//    }
//    if (array_key_exists('s', $opt)) {
//      $testPharName = $opt['s'];
//    }
//    if (!array_key_exists('i', $opt) && !array_key_exists('p', $opt)
//        && !array_key_exists('s', $opt)) {
//      echo $usage.PHP_EOL;
//      return false;
//    }
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

}

Main::run();