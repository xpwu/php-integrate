<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 15/11/9
 * Time: 上午2:27
 */

class Log {
  const DEBUG = 0;
  static public function error($str){
    echo "ERROR: $str \n";
  }
  static public function warning($str) {
    echo "WARNING: $str \n";
  }
  static public function debug($str) {
    if (self::DEBUG) {
      echo "DEBUG: $str \n";
    }
  }
  static public function debug_dump($var) {
    if (self::DEBUG) {
      var_dump($var);
    }
  }
}