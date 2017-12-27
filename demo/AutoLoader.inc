<?php

/*
 * ------Auto Created By integrate, DO NOT modify this file------
 */


class inte_AutoLoader {

  static public $prefixDir = null;

  /**
   * @return bool
   */
  static function run() {
    if (self::$hasLoad_) {
      return true;
    }
    
    if (self::$prefixDir === null) {
      self::$prefixDir = dirname(__FILE__);
    }

    self::$hasLoad_ = spl_autoload_register( function($classname) {
      if (array_key_exists($classname, self::$classMap_)) {
        require_once (self::$prefixDir.'/'.self::$classMap_[$classname]);
				return true;
			}

			if (array_key_exists($classname, self::$excludeClassMap_)) {
        require_once (self::$excludeClassMap_[$classname]);
				return true;
			}

			return false;
    });
    return self::$hasLoad_;
  }

  static private $hasLoad_ = false;
  static public $classMap_ = [
    'Demo\CGI' => "./CGI.inc",
    'Demo\SubCGI' => "./SubCGI.inc",
    'ThirdParty\Test' => "./third_party/Test.inc",
    'Subdir2\Test' => "./subdir2/Test.inc",
    'Subdir1\Test' => "./subdir1/Test.inc",
    'Subdir1\Subdir11\Test' => "./subdir1/subdir11/Test.inc",
    'Subdir1\Subdir11\Config' => "./subdir1/subdir11/Dev.inc",
    'Subdir1\Subdir12\Test' => "./subdir1/subdir12/Test.inc",
    'Glob\Test' => "./glob/Test.inc",
    'a_proto' => "./glob/a_proto.inc",
    'b_proto' => "./glob/b_proto.inc",
    'c_proto' => "./glob/c_proto.inc",
    'Demo\SubSubCGI' => "./SubSubCGI.inc",
    'MyMap' => "./third_party/libmap/map.php",
  ];

  static private $excludeClassMap_ = [
  ];
}

