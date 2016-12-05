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
    $opt = getopt("hvc:");

    $topDirStr = "integrate.conf.hphp";

    if (!self::parseOpt($opt, $topDirStr)) {
      return 1;
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
          -v show version.
EOF;

    if (array_key_exists('h', $opt) && $opt['h'] === false) {
      echo $usage.PHP_EOL;
      return false;
    }

    if (array_key_exists('c', $opt)) {
      $topDirStr = $opt['c'];
    }

    if (array_key_exists('v', $opt)) {
      echo "phpinte 0.1.1\n";
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

}

exit (Main::run());