<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 15/11/9
 * Time: 下午6:19
 */

require_once ("MkFileReader.inc");
require_once ("Log.inc");
require_once ("Utils.inc");

class AutoChecker {
  function __construct(MkFileReader $mkr) {
    $this->mkr_ = $mkr;
  }

  public function outputAutoload(&$fileName) {
    $className = "inte_AutoLoader";
    $fileName = "$className.inc";
    $autoload = <<<EOF
<?php

/*
 * ------Auto Created By integrate, DO NOT modify this file------
 */


class $className {

  static public \$prefix = '.';

  /**
   * @return bool|void
   */
  static function run() {
    if (self::\$hasLoad_) {
      return true;
    }

    self::\$hasLoad_ = spl_autoload_register( function(\$classname) {
      if (!array_key_exists(\$classname, self::\$classMap_)) {
				return false;
			}
			require_once (self::\$prefix.'/'.self::\$classMap_[\$classname]);
			return true;
    });
    return self::\$hasLoad_;
  }

  static private \$hasLoad_ = false;
  static private \$classMap_ = [\n
EOF;

    foreach ($this->classMap_ as $cls => $file) {
      $autoload .= "    '" . $cls . "' => \"" . $file . "\",\n";
    }
    foreach (Utils::$classLoader as $cls => $file) {
      $autoload .= "    '" . $cls . "' => \"" . $file . "\",\n";
    }

    $autoload .= <<<EOF
  ];
}


EOF;
    return $autoload;
  }

  /**
   * @return bool
   */
  public function check(){
    if (!function_exists("pcntl_fork")) {
      return $this->checkWithNormal();
    } else {
      return $this->checkWithPcntl();
    }
  }

  private function checkWithNormal() {
    $files = $this->mkr_->allFiles();
    foreach ($files as $k=>$v) {
      if (pathinfo($k, PATHINFO_EXTENSION) === 'phar') {
        continue;
      }
      if (!$this->checkAfile($k, $v)) {
        return false;
      }
    }
    return true;
  }

  private function checkWithPcntl() {
    $files = [];
    $files[0] = $this->mkr_->allFiles();
    $files[1] = [];

    $i= 0;

    while (count($files[$i]) != 0) {
      $j = ($i == 0)? 1 : 0;
      $files[$j] = [];
      foreach ($files[$i] as $k=>$v) {
        if (pathinfo($k, PATHINFO_EXTENSION) === 'phar') {
          continue;
        }
        $res = $this->checkAfileWithPcntl($k, $v);
        if ($res === 1) {
          return false;
        } else if($res === 2) {
          $files[$j][$k] = $v;
        }
      }
      if (count($files[$i]) == count($files[$j])) {
        Log::error("如下文件可能存在循环依赖, 请重点检查extends or implement 类的依赖");
        foreach ($files[$i] as $k => $v) {
          Log::error($k);
        }
        return false;
      }
      $i = ($i == 0)? 1 : 0;
    }


    return true;
  }

  // 0: success; 1: failed; 2: delay
  private function checkAfileWithPcntl($srcfile, $desfile) {
    $beforeConst = get_defined_constants();
    $beforeClass = get_declared_classes();
    $beforeInterface = get_declared_interfaces();
//    $beforeVar = get_defined_vars();
//    $beforeVar["beforeVar"] = $beforeVar;

    $pid = pcntl_fork();
//    $beforeVar["pid"] = $pid;

    switch ($pid) {
      case -1: Log::error("pcntl_fork failed"); return 1;
      case 0 : {
        error_reporting(0);
        include_once $srcfile;
        exit(0);
      }
      default : {
        pcntl_waitpid($pid, $status);
        if (!pcntl_wifexited($status) || pcntl_wexitstatus($status)) {
          return 2;
        }
      }
    }

    $beforeVar = get_defined_vars();
    $beforeVar["beforeVar"] = $beforeVar;
    include_once $srcfile;
    $afterVar = get_defined_vars();
    $afterConst = get_defined_constants();
    $afterClass = get_declared_classes();
    $afterInterface = get_declared_interfaces();

    Log::debug_dump($beforeVar);
    Log::debug_dump($afterVar);
    Log::debug("afterVar type = ".gettype($afterVar));
    Log::debug("beforeVar type = ".gettype($beforeVar));
    $diffVar = array_diff_key($afterVar, $beforeVar);
    Log::debug("diffVar = ".gettype($diffVar));
    $diffClass = array_diff($afterClass, $beforeClass);
    $diffConst = array_diff($afterConst, $beforeConst);
    $diffInterface = array_diff($afterInterface, $beforeInterface);
    Log::debug_dump($afterClass);

    if (count($diffVar) != 0) {
      log::error($srcfile. " can NOT define var!---".implode(','
          , array_keys($diffVar)));
      return 1;
    }
    if (count($diffConst) != 0) {
      log::error($srcfile. " can NOT define global const!---".implode(','
          , array_keys($diffConst)));
      return 1;
    }
    foreach ($diffClass as $className) {
      $this->classMap_[$className] = $desfile;
    }
    foreach ($diffInterface as $inter) {
      $this->classMap_[$inter] = $desfile;
    }

    return 0;
  }

  private function checkAfile($srcfile, $desfile) {
    $beforeConst = get_defined_constants();
    $beforeClass = get_declared_classes();
    $beforeInterface = get_declared_interfaces();
    $beforeVar = get_defined_vars();
    $beforeVar["beforeVar"] = $beforeVar;
    require ($srcfile);
    $afterVar = get_defined_vars();
    $afterConst = get_defined_constants();
    $afterClass = get_declared_classes();
    $afterInterface = get_declared_interfaces();

    Log::debug_dump($beforeVar);
    Log::debug_dump($afterVar);
    Log::debug("afterVar type = ".gettype($afterVar));
    Log::debug("beforeVar type = ".gettype($beforeVar));
    $diffVar = array_diff_key($afterVar, $beforeVar);
    Log::debug("diffVar = ".gettype($diffVar));
    $diffClass = array_diff($afterClass, $beforeClass);
    $diffConst = array_diff($afterConst, $beforeConst);
    $diffInterface = array_diff($afterInterface, $beforeInterface);
    Log::debug_dump($afterClass);

    if (count($diffVar) != 0) {
      log::error($srcfile. " can NOT define var!---".implode(','
          , array_keys($diffVar)));
      return false;
    }
    if (count($diffConst) != 0) {
      log::error($srcfile. " can NOT define global const!---".implode(','
          , array_keys($diffConst)));
      return false;
    }
    foreach ($diffClass as $className) {
      $this->classMap_[$className] = $desfile;
    }
    foreach ($diffInterface as $inter) {
      $this->classMap_[$inter] = $desfile;
    }

    return true;
  }

  private $mkr_;
  private $classMap_ = [];
}
