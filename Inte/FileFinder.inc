<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 7:19 PM
 */

namespace Inte;

/**
 *
 * 路径信息：
 *  相对于 topDir 的路径
 *
 */


class FileFinder {

  function __construct(string $topDir, array $patterns, array $excludes = []) {
    $this->patterns_ = $patterns;
    $this->excludes_ = $excludes;
    $this->topDir_ = $topDir;
  }

  /**
   * @return array [string]
   */
  public function getAllFile():array {
    $pwd = getcwd();
    chdir($this->topDir_);

    $allFiles = [];
    foreach ($this->patterns_ as $pattern) {
      $files = glob($pattern, GLOB_BRACE);
      $allFiles = array_merge($allFiles, $files);
    }

    $allFiles = array_unique($allFiles);

    $excludeFiles = [];

    foreach ($this->excludes_ as $pattern) {
      $files = glob($pattern, GLOB_BRACE);
      $excludeFiles = array_merge($excludeFiles, $files);
    }

    $excludeFiles = array_unique($excludeFiles);

    $filesOrg = array_diff($allFiles, $excludeFiles);
    $files = [];
    foreach ($filesOrg as $item) {
      if (is_file($this->topDir_. DIRECTORY_SEPARATOR . $item)) {
        $files[] = $item;
      }
    }

    chdir($pwd);

    return array_values($files);
  }

  private $patterns_;
  private $excludes_;
  private $topDir_;
}