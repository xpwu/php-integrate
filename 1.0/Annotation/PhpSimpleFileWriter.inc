<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/1
 * Time: 8:05 PM
 */

namespace Inte\Annotation;


use Inte\Logger;

class PhpSimpleFileWriter implements PhpFileWriter {

  function __construct(string $outputDir, string $fileName, PhpSimpleFiler $filer) {
    $this->fileName_ = $fileName;
    $this->filer_ = $filer;
    $this->outputDir_ = $outputDir;
  }

  function write(string $content) {
    $dir = dirname($this->outputDir_ . DIRECTORY_SEPARATOR . $this->fileName_);
    mkdir($dir, 0777, true);

    // xb: 不能重名
    $fd = fopen($this->fileName_, "xb");
    if ($fd === false) {
      (new Logger())->warn($this->outputDir_
        . DIRECTORY_SEPARATOR . $this->fileName_ . " has existed!");
      return;
    }
    fwrite($fd, $content);
    fclose($fd);

    $this->hasWritten_ = true;

    $this->filer_->addFile($this->fileName_);
  }

  // 防止用户编码错误  写了重复的文件名，所有重写时，必须是调用过write后才能执行
  // write 中 已经防止了重名的情况
  function rewrite(string $content) {
    if (!$this->hasWritten_) {
      $this->write($content);
//      throw new \Exception("rewrite be called, but write has not called!");
    }
    $fd = fopen($this->fileName_, "wb");
    fwrite($fd, $content);
    fclose($fd);
  }

  private $outputDir_;
  private $fileName_;
  private $filer_;
  private $hasWritten_ = false;

}