<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 15/11/10
 * Time: 下午12:09
 */

require_once("Integrater.inc");
require_once("Utils.inc");


class Main {
  static public function run() {
    $opt = getopt("hvwc:l:");

    $topDirStr = "integrate.conf.hphp";

    $onlyClassLoaderPath = "";

    if (!self::parseOpt($opt, $topDirStr, $onlyClassLoaderPath)) {
      return 1;
    }
    $topDir = self::findSrcTopDir($topDirStr);
    if (!$topDir) {
      Log::error("not find top dir");
      return 1;
    }

    Utils::$topDir = $topDir;

    $pharName = $topDir.'main';
    $mode = "dev";
    require_once ($topDir."/".$topDirStr);
    if (isset($type)) {$mode = $type;}
    if (isset($phar_name)) {$pharName = $phar_name;}
    if (isset($mk_name)) {Utils::$mk_name = $mk_name;}
    if (isset($index)) {Utils::$index[$topDir.'/'.$index] = $index;}


    $integrater = new Integrater($mode, $topDir);
    if (!$integrater->integrate()) {
      return 1;
    }

    if ($onlyClassLoaderPath != "") {
      if (substr($onlyClassLoaderPath, 0, 1) != "/") {
        $onlyClassLoaderPath = $topDir."/".$onlyClassLoaderPath;
      }
      if (! $integrater->onlyOutputAutoLoader($onlyClassLoaderPath)) {
        return 1;
      }
    }

    if ($pharName !== '' && $onlyClassLoaderPath == "") {
      if (! $integrater->phar($pharName)) {
        return 1;
      }
    }

//    echo "---success---\n";
    return 0;
  }

  static private function parseOpt($opt, &$topDirStr, &$onlyClassLoader) {
    static $usage = <<<EOF
Usage:  option
  option: -h show this help;
          -c conf file name;
          -v show version;
          -w show path;
          -l only output class loader to path
              , relative to top dir or absolute dir
              , and filename is AutoLoader.inc.
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

    if (array_key_exists('l', $opt)) {
      $onlyClassLoader = $opt['l'];
    }

    if (array_key_exists('v', $opt)) {
      echo "phpinte 0.7.1".PHP_EOL;
      return false;
    }

    if (array_key_exists('w', $opt)) {
      $name = $_SERVER['argv'][0].PHP_EOL;
      if (substr($name, 0, 1) == "/") {
        echo $name;
      } else {
        echo getcwd().'/'.$_SERVER['argv'][0].PHP_EOL;
      }
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

$result = Main::run();
if ($result != 0) {
  error_reporting(E_ALL);
  foreach (Utils::$errorFiles as $key=>$value) {
    include_once $key;
  }
}

exit ($result);