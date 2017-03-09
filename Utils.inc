<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 15/11/9
 * Time: 上午10:50
 */

class Utils {
  const DEV_MODE = 1;
  const PRO_MODE = 2;

  static public function delAllFileAtDir($dir) {
    $dh = opendir($dir);
    while(($d = readdir($dir)) !== false){
      if($d == '.' || $d == '..'){
        continue;
      }
      $tmp = $dir.'/'.$d;
      if(!is_dir($tmp)){
        unlink($tmp);
      }else{
        self::delAllFileAtDir($tmp);
      }
    }
    closedir($dh);
    rmdir($dir);
  }

  static public $index = [];
  static public $classLoader = [];
  static public $requireOnceFiles = [];
  static public $requireOnceExcludeFiles = [];
  static public $classLoader_exclude = [];

  static public $mk_name="integrate.mk.hphp";

  static public $topDir;

  static public $showError = false;
}