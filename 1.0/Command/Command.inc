<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/24
 * Time: 2:18 PM
 */


namespace Inte;

use Inte\Config\Config;
use Inte\Error\NormalError;

abstract class Command {
  public function run():void {
    $this->project_ = Project::getInstance();
    $this->logger_ = new Logger();
    $this->config_ = $this->project_->getConfig();
    $this->step_ = 0;

    $allProjectFiles = $this->findFile();

    $dependenceProcessor = $this->parseDependence($annoCompiler);

    $sequenceFilesByCompiler = $this->processAnnotation($allProjectFiles, $annoCompiler);

    $allNeedParseFiles = array_merge($sequenceFilesByCompiler
      , array_diff($allProjectFiles, $sequenceFilesByCompiler));
    $this->parseFiles($allNeedParseFiles, $dependenceProcessor);
  }

  private function findFile():array {
    $this->step_++;
    $this->logger_->info("$this->step_: find file...");

    $fileFinder = new FileFinder($this->project_->getRootDir(), $this->config_->src
      , $this->config_->excludeSrc);
    $allProjectFiles = $fileFinder->getAllFile();

    if (count($allProjectFiles) == 0) {
      $this->logger_->warn("Not find any file!");
    }

    $this->postFindFile($allProjectFiles);

    return $allProjectFiles;
  }

  private function parseDependence(AnnotationCompiler &$compiler): DependenceProcessor {
    $this->step_++;
    $this->logger_->info("$this->step_: parse dependence...");

    $dependenceProcessor = new DependenceProcessor(
      $this->config_->annoAndDepRepo
      , $this->config_->dependencies);
    $dependenceProcessor->mergeDependencies();
    $dependenceProcessor->loadDependencies();

    $compiler = new AnnotationCompiler(
      $this->config_->annoAndDepRepo
      , $this->config_->annotationProcessors
      , $this->project_->getAnnotationOutputDir());
    $compiler->init();
    $annoDepProcessor = $compiler->getDependenceProcessor();
    $annoDepProcessor->mergeDependencies();

    $dependenceProcessor->updateVersionByOther($annoDepProcessor);
    $annoDepProcessor->updateVersionByOther($dependenceProcessor);

    $this->postParseDependence($dependenceProcessor, $compiler);

    return $dependenceProcessor;
  }

  /**
   * @param array $allProjectFiles
   * @return string[] 按照解析顺序的排序的文件
   */
  private function processAnnotation(array $allProjectFiles
    , AnnotationCompiler $compiler):array {
    $this->step_++;
    $this->logger_->info("$this->step_: process annotation...");
    $sequenceFilesByCompiler
      = $compiler->compile($allProjectFiles);
    $this->postProcessAnnotation(array_diff($sequenceFilesByCompiler
      , $allProjectFiles));

    return $sequenceFilesByCompiler;
  }

  private function parseFiles(array $allNeedParseFiles
    , DependenceProcessor $processor) {
    $this->step_++;
    $this->logger_->info("$this->step_: parse files...");
    $allProjectFile = $allNeedParseFiles;

    $sourceParser = new SourceParser($allProjectFile);
    $sourceParser->parse();
    $allProjectFile = $sourceParser->getNotParseFile();

    // 还有没有解析的文件
    if (count($allProjectFile) != 0) {
      // 输出编译错误
      $this->printPhpFileError($allProjectFile);
      throw new NormalError("-----Failed!!!------");
    }
    $classFileMap = $sourceParser->getResult();

    $this->checkAllClass($processor, $classFileMap);

    $this->postParseFile($classFileMap, $processor);
  }

  private function checkAllClass(DependenceProcessor $processor, array $newClasses) {
    foreach ($processor->getMergeResult() as $dependence) {
      foreach (array_keys($dependence->getClasses()) as $class) {
        if (array_key_exists($newClasses, $class)) {
          throw new NormalError(
            "$class are again defined both "
            .$dependence->getName() ." and Project://{$newClasses[$class]}");
        }
      }
    }
  }

  private function printPhpFileError($errorFiles) {
    foreach ($errorFiles as $errorFile) {
      require_once $errorFile;
    }
  }

  protected function postFindFile(array $files):void {}
  protected function postParseDependence(DependenceProcessor $processor
    , AnnotationCompiler $compiler):void {}
  protected function postProcessAnnotation(array $newFiles):void {}
  protected function postParseFile(array $classFileMap
    , DependenceProcessor $processor):void {}

  /**
   * @var Config
   */
  private $config_;
  /**
   * @var Logger
   */
  private $logger_;
  /**
   * @var int
   */
  private $step_;
  /**
   * @var Project
   */
  private $project_;

}