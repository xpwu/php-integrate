<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 7:19 PM
 */

namespace Inte;


class FileFinder {

  function __construct(string $pattern) {
    $this->pattern_ = $pattern;
  }

  /**
   * @return array [string]
   */
  public function getAllFile():array {
    $files = glob($this->pattern_, GLOB_BRACE);
    if (!$files) {
      $files = [];
    }

    return $files;
  }

  private $pattern_;
}