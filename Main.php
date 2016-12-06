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
    $opt = getopt("hvwc:");

    $topDirStr = "integrate.conf.hphp";

    if (!self::parseOpt($opt, $topDirStr)) {
      return 1;
    }
    $topDir = self::findSrcTopDir($topDirStr);
    if (!$topDir) {
      Log::error("not find top dir");
      return 1;
    }

    $pharName = $topDir.'main';
    $mode = "dev";
    require_once ($topDir."/".$topDirStr);
    if (isset($type)) {$mode = $type;}
    if (isset($phar_name)) {$pharName = $phar_name;}
    if (isset($mk_name)) {Utils::$mk_name = $mk_name;}


    $integrater = new Integrater($mode, $topDir);
    if (!$integrater->integrate()) {
      return 1;
    }

    if ($pharName !== '') {
      if (! $integrater->phar($pharName)) {
        return 1;
      }
    }

    echo "---success---\n";
    return 0;
  }

  static private function parseOpt($opt, &$topDirStr) {
    static $usage = <<<EOF
Usage:  option
  option: -h show this help;
          -c conf file name;
          -v show version;
          -w show path.
EOF;

    if (!function_exists("pcntl_fork")) {
      $usage = "---NOT pcntl".PHP_EOL.$usage;
    } else {
      $usage = "---USE pcntl".PHP_EOL.$usage;
    }

    if (array_key_exists('h', $opt) && $opt['h'] === false) {
      echo $usage.PHP_EOL;
      return false;
    }

    if (array_key_exists('c', $opt)) {
      $topDirStr = $opt['c'];
    }

    if (array_key_exists('v', $opt)) {
      echo "phpinte 0.2".PHP_EOL;
      return false;
    }

    if (array_key_exists('w', $opt)) {
      echo getcwd().'/'.$_SERVER['argv'][0].PHP_EOL;
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
      if ($pwd === '/') {
        return false;
      }
    }
    chdir($pwdori);
    return $pwd;
  }

}

exit (Main::run());