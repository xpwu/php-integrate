<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/1
 * Time: 8:05 PM
 */

namespace Inte\Annotation;



use Inte\Project;

class PhpSimpleFileWriter implements PhpFileWriter {

  function __construct(string $outputDir, string $fileName, PhpSimpleFiler $filer) {
    $this->fileName_ = $fileName;
    $this->filer_ = $filer;
    $this->outputDir_ = $outputDir;
    $this->defaultDir_ = $outputDir;
  }

  function modifyOutDirToRootDir():void {
    $this->outputDir_ = Project::getInstance()->getRootDir();
    $this->isRootDir_ = true;
  }

  function modifyOutDirToDefault():void {
    $this->outputDir_ = $this->defaultDir_;
    $this->isRootDir_ = false;
  }

  public function dontInteOrPublish() {
    $this->addFile_ = false;
  }

  /**
   * @param string $content
   * @throws \Exception
   */
  function write(string $content) {
    if ($this->addFile_) {
      $this->filer_->applyWriteFile($this->fileName_);
    }

    $dir = dirname($this->outputDir_ . DIRECTORY_SEPARATOR . $this->fileName_);
    @mkdir($dir, 0777, true);

    // xb: 不能重名
    if ($this->isRootDir_) {
      $fd = fopen($this->outputDir_ . DIRECTORY_SEPARATOR .
        $this->fileName_, "wb");
    } else {
      $fd = fopen($this->outputDir_ . DIRECTORY_SEPARATOR .
        $this->fileName_, "xb");
    }

    if ($fd === false) {
      throw new \Exception($this->outputDir_
        . DIRECTORY_SEPARATOR . $this->fileName_ . " has existed!");
    }
    if(!fwrite($fd, $content)){
      throw new \Exception($this->outputDir_
        . DIRECTORY_SEPARATOR . $this->fileName_ . " write error!");
    }
    fclose($fd);

    $this->hasWritten_ = true;

    if ($this->addFile_) {
      $this->filer_->addFile($this->fileName_);
    }
  }

  // 防止用户编码错误  写了重复的文件名，所有重写时，必须是调用过write后才能执行
  // write 中 已经防止了重名的情况
  /**
   * @param string $content
   * @throws \Exception
   */
  function rewrite(string $content) {
    if ($this->addFile_) {
      $this->filer_->applyWriteFile($this->fileName_);
    }

    if (!$this->hasWritten_) {
      $this->write($content);
      return;
//      throw new \Exception("rewrite be called, but write has not called!");
    }
    $fd = fopen($this->outputDir_ . DIRECTORY_SEPARATOR
      . $this->fileName_, "wb");
    fwrite($fd, $content);
    fclose($fd);
  }

  private $isRootDir_ = false;
  private $outputDir_;
  private $defaultDir_;
  private $fileName_;
  private $filer_;
  private $hasWritten_ = false;
  private $addFile_ = true;
}