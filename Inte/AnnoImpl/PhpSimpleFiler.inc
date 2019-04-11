<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 6:51 PM
 */

namespace Inte\Annotation;


use Inte\Project;

class PhpSimpleFiler implements PhpFiler {
  public function __construct(string $outputDir) {
    $this->outputDir_ = $outputDir;
  }

  public function modifyOutDirToRootDir(): void {
    $this->outputDir_ = Project::getInstance()->getRootDir();
  }

  public function createSourceFile(string $fileDirAndName):PhpFileWriter {
    return new PhpSimpleFileWriter($this->outputDir_, $fileDirAndName, $this);
  }

  /**
   * @param string $file  相对于$outputDir_ 的路径
   */
  public function addFile(string $file) {
    $this->allNewFile_[] = $file;
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
}