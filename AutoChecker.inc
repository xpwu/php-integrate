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
//    $this->mode_ = $mode;
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
//  private $mode_;
}
