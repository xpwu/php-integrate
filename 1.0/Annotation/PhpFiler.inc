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
    $this->outputDir = $outputDir;
  }

  public function createSourceFile(string $fileDirAndName) {
    if ($this->fd) {
      fclose($this->fd);
    }
    $this->fileDirAndName = $fileDirAndName;
    $this->fd = fopen($this->outputDir
      . DIRECTORY_SEPARATOR . $this->fileDirAndName, "w");
  }

  public function append(string $content) {
    fwrite($this->fd, $content);
  }

  public function close() {
    if ($this->fd) {
      fclose($this->fd);
    }
  }

  private $fileDirAndName;
  private $outputDir;

  private $fd = null;
}