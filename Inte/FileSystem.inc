<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/4
 * Time: 8:54 PM
 */

namespace Inte;


class FileSystem {

  static public function delAllDirAndFile(string $path){
    if(!is_dir($path)) {
      return;
    }

    $p = scandir($path);
    foreach($p as $val){
      if ($val === "." || $val === "..") {
        continue;
      }

      $fullPath = $path. DIRECTORY_SEPARATOR. $val;

      if (is_file($fullPath)) {
        @unlink($fullPath);
        continue;
      }

      self::delAllDirAndFile($fullPath);
      @rmdir($fullPath);
    }
  }

  static public function preGlob(string $path) {

  }

  static private function deep(string $path) {

  }

}