<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/24
 * Time: 12:00 AM
 */

namespace Inte;



use Inte\Error\NormalError;
use Inte\Error\OK;

class Options {
  private const Usage = <<<EOF
Usage:  option
  option: -h show this help;
          -c conf file name, default is project.conf.hphp;
          -v show version;
          -w show path;
          
          --command: 'build' or 'publish',
                default is 'build'

EOF;


  function __construct() {
    $option = getopt('hvwc:', ['command'], $optind);

    $argv = $_SERVER['argv'];
    $pos_args = array_slice($argv, $optind);
    if ($pos_args !== "") {
      throw new NormalError("Not support argv: ".$pos_args .", please use '-h'");
    }

    if (array_key_exists('h', $option) && $option['h'] === false) {
      echo self::Usage;
      throw new OK();
    }

    if (array_key_exists('v', $option)) {
      echo "phpinte 1.0".PHP_EOL;
      throw new OK();
    }

    if (array_key_exists('w', $option)) {
      $name = $argv[0].PHP_EOL;
      if (substr($name, 0, 1) == "/") {
        echo $name;
      } else {
        echo getcwd().'/'.$name;
      }
      throw new OK();
    }

    if (array_key_exists('c', $option)) {
      $this->configFileName = $option['c'];
    }

    $this->findRootDir();

    $commands = ["build"=>'Inte\Command\Build', 'publish'=>'Inte\Command\Publish'];

    if (!array_key_exists('command', $option)) {
      $this->command_ = new $commands['build']();
      return;
    }

    if (!array_key_exists($option['command'], $commands)) {
      throw new NormalError("Not support command: "
        . $option['command'] . ", please use '-h'");
    }

    $this->command_ = new $option['command']();
  }

  public function getCommand():Command {
    return $this->command_;
  }

  public function getRootDir():string {
    return $this->rootDir;
  }

  // 不带路径
  public function getConfigFileName():string {
    return $this->configFileName;
  }

  private function findRootDir() {
    $pwdori = getcwd();
    $pwd = getcwd();
    while (!is_file($pwd.DIRECTORY_SEPARATOR.$this->configFileName)) {
      chdir('..');
      $pwd = getcwd();
      if ($pwd === '/') {
        throw new NormalError("Not find root dir, which exist file '"
          . $this->configFileName . "'");
      }
    }
    $this->rootDir = $pwd;

    chdir($pwdori);
  }

  private $command_;
  private $configFileName = "project.conf.hphp";
  private $rootDir;
}