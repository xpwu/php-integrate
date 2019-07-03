<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 11:22 PM
 */

namespace Inte;


use Inte\Annotation\AbstractAnnotationServer;
use Inte\Annotation\AnnotationCompilerStrategyOfDependency;
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
    @mkdir($this->distDir_, 0777, true);
    $this->tempDir_ = $buildAnnotationDir . DIRECTORY_SEPARATOR . "tmp";
    @mkdir($this->tempDir_, 0777, true);

    $this->sequenceFileName_ = $this->tempDir_. DIRECTORY_SEPARATOR . "sequence";
  }

  public function setStrategy(
    AnnotationCompilerStrategyOfDependency $strategyOfDependency) {
    $this->strategyOfDependency_ = $strategyOfDependency;
  }

  public function init() {
    if (isset($this->dependenceProcessor_)) {
      return;
    }

    $this->dependenceProcessor_ = new DependenceProcessor($this->repo_, []);

    $depBySelf = [];
    foreach ($this->processors_ as $annotationProcessor) {
      $depBySelf[] = $annotationProcessor->convertToDependence();
    }

    $this->dependenceProcessor_->appendDependence($depBySelf);

    $this->dependenceProcessor_->mergeDependencies();

  }

  public function printDependence(string $prefix) {
    $this->dependenceProcessor_->printDependencies($prefix);
  }

  /**
   * @return DependenceProcessor
   * @throws \Exception
   */
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
  public function compile(array $files, bool $isBuild):array {

    // 因为两个进程没有同时操作同一数据的情况，所以不用考虑数据互斥的问题

    $pid = pcntl_fork();
    switch ($pid) {
      case -1:
        throw new NormalError("pcntl_fork error!");
      case 0 : {
//        error_reporting(0);
        try {
          $this->initInAnnotationEnvironment($isBuild);
          $this->compileInAnnotationEnvironment($files);
        }catch (NormalError $error) {
          $this->printError($error);
          exit($error->getExitStatus());
        }
        exit(0);
      }
    }

    pcntl_waitpid($pid, $status);
    if (!pcntl_wifexited($status) || pcntl_wexitstatus($status)) {
      throw new NormalError(" AnnotationCompiler Failed!!!");
    }

    $retFile = file_get_contents(Project::getInstance()->getRootDir()
      . DIRECTORY_SEPARATOR . $this->sequenceFileName_);
    if ($retFile === "") {
      return [];
    }

    return explode(";", $retFile);
  }

  private function printError(NormalError $error):void {
    $debug = $error->getTrace();
    $file = '...'.substr($debug[0]["file"], -100);
    $line = $debug[0]["line"];
    $debug1 = @$debug[1]??['class'=>"{main}", 'function'=>"{main}", 'type'=>" "];
    $class = $debug1['class'];
    $method = $debug1['function'];
    $type = $debug1['type'];

    echo "$file($line): ${class}${type}${method}() --- $error" . PHP_EOL;
  }

  private function initInAnnotationEnvironment(bool $isBuild) {
    /**
     * 只是被注解编译器使用的依赖在注解处理环境中加载，不能影响到项目环境
     */
    $this->dependenceProcessor_->loadDependencies();

    $serversStr = [];
    foreach ($this->processors_ as $annotationProcessor) {
      $serversStr = array_merge($serversStr
        , $annotationProcessor->getServersString());
    }
    $serversStr = array_unique($serversStr);

    foreach ($serversStr as $item) {

      $filer = new PhpSimpleFiler($this->distDir_, $item);
      $this->phpFilers_[] = $filer;

      /**
       * @var AbstractAnnotationServer $server
       */
      $server = new $item();
      $server->init(new SimpleLogger(), $filer
        , Project::getInstance()->getConfig()
        , Project::getInstance()->getRootDir()
        , $isBuild);
      $this->servers_[] = $server;
    }

  }

  private function compileInAnnotationEnvironment(array $allFiles) {

    $this->strategyOfDependency_->processBy($this->servers_);

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
        $outputFile = array_merge($outputFile, array_map(function ($value){
          return $this->distDir_ . DIRECTORY_SEPARATOR . $value;
        }, $filer->getAllNewFileAndClear()));
      }

      // 只是添加.inc结束的文件 再次输入注解处理器
      $allFiles = array_merge($allFiles, array_values(array_filter($outputFile
        , function (string $value){
          return strpos($value, ".inc");
        })));

      // 输入与输出都为0时， 才停止编译
    }while(count($inputClasses) != 0 || count($outputFile) != 0);

    // phpFilers_ 必须先指定processOver, 阻止写可能需要集成的文件
    foreach ($this->phpFilers_ as $filer) {
      $filer->processOver();
    }
    foreach ($this->servers_ as $server) {
      $server->processOverSign();
    }

    $parsedFileBySequence = array_merge($parsedFileBySequence, $allFiles);

    // put 时  phar 会自动去php里面的路径  已统一设置为 rootDir
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

  /**
   * @var AnnotationCompilerStrategyOfDependency
   */
  private $strategyOfDependency_;
}