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

    if ($pharName !== '') {
      if (! $integrater->phar($pharName)) {
        return;
      }
    }

    echo "---success---\n";
  }

  static private function parseOpt($opt, &$topDirStr) {
    static $usage = <<<EOF
Usage:  option
  option: -h show this help;
          -c conf file name.
EOF;

    if (array_key_exists('h', $opt) && $opt['h'] === false) {
      echo $usage.PHP_EOL;
      return false;
    }

    if (array_key_exists('c', $opt)) {
      $topDirStr = $opt['c'];
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

Main::run();