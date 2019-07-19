<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/24
 * Time: 12:00 AM
 */

namespace Inte;



use Inte\Command\Build;
use Inte\Command\PrintAllProjectFiles;
use Inte\Command\PrintDependence;
use Inte\Command\ProcessAnnotation;
use Inte\Command\Publish;
use Inte\Error\NormalError;
use Inte\Error\OK;

class Options {
  private const Usage = <<<EOF
Usage:  option
  option: -h show this help;
          -c conf file name, default is project.conf.hphp.
              Find the dir automatically from the current dir to sys root dir('/').
              The arg can have a absolute dir or relative dir ;
          -v show version;
          -w show path;
          -r show project root dir;
          -l the same to '--command printFiles';
          -d the same to '--command printDependence';
          -p the same to '--command publish';
          -a the same to '--command processAnnotation';
          
          --command: 'build', 'publish', 'printFiles', 'printDependence', 'processAnnotation'
                default is 'build'

EOF;


  function __construct() {
    $option = getopt('ldarhvwpc:', ['command:'], $optind);

    $argv = $_SERVER['argv'];
    $pos_args = array_slice($argv, $optind);
    if (count($pos_args) !== 0) {
      throw new NormalError("Not support argv: ".$pos_args[0] .", please use '-h'");
    }

    if (array_key_exists('h', $option) && $option['h'] === false) {
      echo self::Usage;
      throw new OK();
    }

    if (array_key_exists('v', $option)) {
      echo "phpinte ". Version::str .PHP_EOL;
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

    $findRoot = "./";
    if (array_key_exists('c', $option)) {
      $this->configFileName = basename($option['c']);
      $findRoot = dirname($option['c']);
//      if (strpos($option['c'], '/') == 0) {
//        $this->configFileName = basename($option['c']);
//        // 从这里开始查找 为后续rootDir 做准备
//        chdir(dirname($option['c']));
//      } else {
//        $this->configFileName = $option['c'];
//      }
    }

    if (array_key_exists('l', $option)) {
      $option['command'] = 'printFiles';
    }

    if (array_key_exists('d', $option)) {
      $option['command'] = 'printDependence';
    }

    if (array_key_exists('p', $option)) {
      $option['command'] = 'publish';
    }

    if (array_key_exists('a', $option)) {
      $option['command'] = 'processAnnotation';
    }

    $this->findRootDir($findRoot);

    if (array_key_exists('r', $option)) {
      echo $this->rootDir.PHP_EOL;
      throw new OK();
    }

    $commands = ["build"=>Build::class
      , 'publish'=>Publish::class
      , 'printFiles'=>PrintAllProjectFiles::class
      , 'printDependence'=>PrintDependence::class
      , 'processAnnotation'=>ProcessAnnotation::class
    ];

    if (!array_key_exists('command', $option)) {
      $this->command_ = new $commands['build']();
      return;
    }

    if (!array_key_exists($option['command'], $commands)) {
      throw new NormalError("Not support command: "
        . $option['command'] . ", please use '-h'");
    }

    $this->command_ = new $commands[$option['command']]();
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

  private function findRootDir(string $findRoot) {
    $pwdori = getcwd();

    chdir($findRoot);
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