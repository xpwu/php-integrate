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

  static public function preGlobArray(array $paths, int $maxDeep):array {
    $ret = [];

    foreach ($paths as $path) {
      $ret = array_merge($ret, self::preGlob($path, $maxDeep));
    }

    return array_unique($ret);
  }

  static public function preGlob(string $path, int $maxDeep):array {

    //  **/aa**/**/wekd

    //  把  **/yyy  或者  xxxx/**/yyy 中所有的**/ 替换为  '','*/','*/*/' ...

    $pattern = '/^(.*?\/)??\*\*\/(.*)/';

    $next = [$path];

    $replacements  = [];
    $last = '';
    $replacements[] = '${1}'.$last.'${2}';
    for ($i = 1; $i < $maxDeep; ++$i) {
      $last = $last . '*/';
      $replacements[] = '${1}'.$last.'${2}';
    }


    do {
      $subjects = $next;
      $next = [];

      foreach ($subjects as $subject) {
        $changed = false;
        foreach ($replacements as $replacement) {
          $newStr = preg_replace($pattern
            , $replacement, $subject, 1, $cnt);
          if ($cnt !== 0) {
            $next[] = $newStr;
            $changed = true;
          }
        }
        if (!$changed) {
          $ret[] = $subject;
        }
      }
    }while(count($next) !== 0);

    return array_unique($ret);
  }

  static public function dirDeep(string $inPath):int {
    if(!is_dir($inPath)) {
      return 0;
    }

    $max = 0;
    $p = scandir($inPath);
    foreach($p as $val){
      if ($val === "." || $val === "..") {
        continue;
      }

      $fullPath = $inPath. DIRECTORY_SEPARATOR. $val;

      if (is_file($fullPath)) {
        continue;
      }

      $dp = self::dirDeep($fullPath);
      if ($max < $dp) {
        $max = $dp;
      }
    }

    return $max + 1;
  }

  static public function escape(string $path):string {
    $pathArr = explode('/', $path);
    $res = [];
    foreach ($pathArr as $value) {
      if ($value === '.') {
        continue;
      }
      if ($value === '..') {
        $value = '__';
      }

      $res[] = $value;
    }

    return implode('/', $res);
  }
}