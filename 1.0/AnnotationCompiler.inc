<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 11:22 PM
 */

namespace Inte;


use Inte\Annotation\AbstractAnnotationServer;
use Inte\Annotation\PhpSimpleFiler;
use Inte\Error\NormalError;

class AnnotationCompiler {
  /**
   * AnnotationCompiler constructor.
   * @param string[] $annotationProcessors
   * @param string $buildAnnotationDir
   */
  function __construct(string $repo, array $annotationProcessors
    , string $buildAnnotationDir) {
    $this->repo_ = $repo;

    foreach ($annotationProcessors as $annotationProcessor) {
      $this->processors_[] = new AnnotationProcessor($this->repo_
        , $annotationProcessor);
    }

    $this->distDir_ = $buildAnnotationDir . DIRECTORY_SEPARATOR . "src";
    mkdir($this->distDir_, 0777, true);
    $this->tempDir_ = $buildAnnotationDir . DIRECTORY_SEPARATOR . "tmp";
    mkdir($this->distDir_, 0777, true);

    $this->sequenceFileName_ = $this->tempDir_. DIRECTORY_SEPARATOR . "sequence";
  }

  public function init() {
    if (isset($this->dependenceProcessor_)) {
      return;
    }

    $this->dependenceProcessor_ = new DependenceProcessor($this->repo_, []);

    $depBySelf = [];
    foreach ($this->processors_ as $annotationProcessor) {
      $this->dependenceProcessor_->appendDependence(
        $annotationProcessor->getDependencies());
      $depBySelf[] = $annotationProcessor->convertToDependence();
    }

    $this->dependenceProcessor_->appendDependence($depBySelf);

    $this->dependenceProcessor_->mergeDependencies();

  }

  public function printDependence(string $prefix) {
    $this->dependenceProcessor_->printDependencies($prefix);
  }

  public function getDependenceProcessor():DependenceProcessor {
    if (!isset($this->dependenceProcessor_)) {
      throw new \Exception(
        "AnnotationCompiler must call 'init' before 'getDependenceProcessor'");
    }
    return $this->dependenceProcessor_;
  }

  /**
   * @return string[] 能按序解析的文件，包括新生成的文件。
   *      主要为了加快项目环境解析文件的速度。
   *      不一定包含了全部的输入文件$files
   */
  public function compile(array $files):array {

    // 因为两个进程没有同时操作同一数据的情况，所以不用考虑数据互斥的问题

    $pid = pcntl_fork();
    switch ($pid) {
      case -1:
        throw new NormalError("pcntl_fork error!");
      case 0 : {
//        error_reporting(0);
        try {
          $this->initInAnnotationEnvironment();
          $this->compileInAnnotationEnvironment($files);
        }catch (NormalError $error) {
          echo $error;
          exit($error->getExitStatus());
        }
        exit(0);
      }
    }

    pcntl_waitpid($pid, $status);
    if (!pcntl_wifexited($status) || pcntl_wexitstatus($status)) {
      throw new NormalError(" AnnotationCompiler Failed!!!");
    }

    $retFile = file_get_contents($this->sequenceFileName_);

    return explode(";", $retFile);
  }

  private function initInAnnotationEnvironment() {
    /**
     * 只是被注解编译器使用的依赖在注解处理环境中加载，不能影响到项目环境
     */
    $this->dependenceProcessor_->loadDependencies();

    $serversStr = [];
    foreach ($this->processors_ as $annotationProcessor) {
      $this->serversStr_ = array_merge($serversStr
        , $annotationProcessor->getServersString());
    }
    array_unique($serversStr);

    foreach ($serversStr as $item) {

      $filer = new PhpSimpleFiler($this->distDir_);
      $this->phpFilers_[] = $filer;

      /**
       * @var AbstractAnnotationServer $server
       */
      $server = new $item();
      $server->init(new Logger(), $filer
        , Project::getInstance()->getConfig());
      $this->servers_[] = $server;
    }

  }

  private function compileInAnnotationEnvironment(array $allFiles) {

    // 保存解析的文件的顺序
    $parsedFileBySequence = [];

    do {
      $sourceParser = new SourceParser($allFiles);
      $sourceParser->parse();
      $allFiles = $sourceParser->getNotParseFile();
      $parsedFileBySequence = array_merge($parsedFileBySequence
        , $sourceParser->getParsedFileOrderByParsingSequence());

      $inputClasses = array_keys($sourceParser->getResult());

      foreach ($this->servers_ as $server) {
        $server->process($inputClasses);
      }

      $outputFile = [];
      foreach ($this->phpFilers_ as $filer) {
        $outputFile[] = array_map(function ($value){
          return $this->distDir_ . DIRECTORY_SEPARATOR . $value;
        }, $filer->getAllNewFileAndClear());
      }

      // 只是添加.inc结束的文件 再次输入注解处理器
      $allFiles = array_merge($allFiles, array_values(array_filter($outputFile
        , function (string $value){
          return strpos($value, ".inc");
        })));

      // 输入与输出都为0时， 才停止编译
    }while(count($inputClasses) != 0 || count($outputFile) != 0);

    file_put_contents($this->sequenceFileName_
      , implode(";", $parsedFileBySequence));

  }

  /**
   * @var AnnotationProcessor[]
   */
  private $processors_ = [];
  /**
   * @var AbstractAnnotationServer[]
   */
  private $servers_ = [];
  private $distDir_;
  private $tempDir_;
  private $sequenceFileName_ = "";
  /**
   * @var DependenceProcessor
   */
  private $dependenceProcessor_ = null;
  private $repo_;

  /**
   * @var PhpSimpleFiler[]
   */
  private $phpFilers_ = [];
}