<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 6:51 PM
 */

namespace Inte\Annotation;


use Inte\Error\NormalError;
use Inte\Project;

class PhpSimpleFiler implements PhpFiler {
  public function __construct(string $outputDir, string $relatedToAnnoServer) {
    $this->outputDir_ = $outputDir;
    $this->relatedToAnnoServer_ = $relatedToAnnoServer;

    $this->applyWriteFileStrategy_ = function (string $file) {
    };
  }

  public function modifyOutDirToRootDir(): void {
    $this->outputDir_ = Project::getInstance()->getRootDir();
  }

  public function createSourceFile(string $fileDirAndName):PhpFileWriter {
    if (!array_key_exists($fileDirAndName, $this->writersMap_)) {
      $this->writersMap_[$fileDirAndName]
        = new PhpSimpleFileWriter($this->outputDir_, $fileDirAndName, $this);
    }

    return $this->writersMap_[$fileDirAndName];
  }

  public function applyWriteFile(string $file) {
    ($this->applyWriteFileStrategy_)($file);
  }

  /**
   * @param string $file  相对于$outputDir_ 的路径
   */
  public function addFile(string $file) {
    $this->allNewFile_[] = $file;
  }

  public function processOver() {
    $this->applyWriteFileStrategy_ = function (string $file) {
      throw new NormalError($this->relatedToAnnoServer_
        . "ERROR, which will write $file. \n"
        . "'processOver' do NOT write any file which will InteOrPublish. "
        . "if you need write a non-InteOrPublish file, "
        . "please use 'writer:dontInteOrPublish' "
        . "to inform that is non-InteOrPublish");
    };
  }

  /**
   * @return string[]  相对于$outputDir_ 的路径
   */
  public function getAllNewFileAndClear():array {
    $ret = $this->allNewFile_;
    $this->allNewFile_ = [];

    return $ret;
  }

  private $outputDir_;

  private $allNewFile_ = [];

  private $writersMap_ = [];

  private $applyWriteFileStrategy_ = null;

  private $relatedToAnnoServer_;
}