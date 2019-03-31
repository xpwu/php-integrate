<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 6:51 PM
 */

namespace Inte\Annotation;


class PhpFiler {
  public function __construct(string $outputDir) {
    $this->outputDir_ = $outputDir;
  }

  public function createSourceFile(string $fileDirAndName) {
    if ($this->fd) {
      fclose($this->fd);
    }

    $dir = $this->outputDir_ .DIRECTORY_SEPARATOR. dirname($fileDirAndName);
    mkdir($dir, 0777, true);

    $this->fd = fopen($this->outputDir_
      . DIRECTORY_SEPARATOR . $fileDirAndName, "w");
  }

  public function append(string $content) {
    fwrite($this->fd, $content);
  }

  public function close() {
    if ($this->fd) {
      fclose($this->fd);
    }
  }

  private $outputDir_;

  private $fd = null;
}