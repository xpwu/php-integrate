<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/24
 * Time: 2:18 PM
 */


namespace Inte;

use Inte\Annotation\AnnoCompilerStrategyOfCompileDependence;
use Inte\Annotation\AnnotationCompilerStrategyOfDependency;
use Inte\Config\Config;
use Inte\Error\NormalError;

abstract class Command {
  public function run():void {
    $this->project_ = Project::getInstance();
    chdir($this->project_->getRootDir());
    $this->logger_ = new SimpleLogger();
    $this->config_ = $this->project_->getConfig();
    $this->step_ = 0;

    $allProjectFiles = $this->findFile();

    /**
     * @var $dependenceProcessor DependenceProcessor
     */
    list($dependenceProcessor, $annoCompiler) = $this->parseDependence();

    $sequenceFilesByCompiler = $this->processAnnotation($allProjectFiles, $annoCompiler);

    $allNeedParseFiles = array_merge($sequenceFilesByCompiler
      , array_diff($allProjectFiles, $sequenceFilesByCompiler));
    $this->parseFiles($allNeedParseFiles, $dependenceProcessor);

    $this->printStep("", true);
  }

  private function findFile():array {
    $this->printStep("find file");

    $fileFinder = new FileFinder($this->project_->getRootDir(), $this->config_->src
      , $this->config_->excludeSrc);
    $allProjectFiles = $fileFinder->getAllFile();

    if (count($allProjectFiles) == 0) {
      $this->logger_->warn("Not find any file!");
    }

    $this->postFindFile($allProjectFiles);

    return $allProjectFiles;
  }

  /**
   * @return array [DependenceProcessor, AnnotationCompiler]
   * @throws
   */
  private function parseDependence(): array {
    $this->printStep("parse dependence");

    $dependenceProcessor = new DependenceProcessor(
      $this->config_->annoAndDepRepo
      , $this->config_->dependencies);
    $dependenceProcessor->mergeDependencies();

    $compiler = new AnnotationCompiler(
      $this->config_->annoAndDepRepo
      , $this->config_->annotationProcessors
      , $this->project_->getAnnotationOutputDir());
    $compiler->init();
    $annoDepProcessor = $compiler->getDependenceProcessor();
    $annoDepProcessor->mergeDependencies();

    $dependenceProcessor->updateVersionByOther($annoDepProcessor);
    $dependenceProcessor->checkDependenceClass();
    $dependenceProcessor->loadDependencies();

    $annoDepProcessor->updateVersionByOther($dependenceProcessor);
    $annoDepProcessor->checkDependenceClass();

    $allDependencyClasses = [];
    foreach ($dependenceProcessor->getMergeResult() as $dependence) {
      $allDependencyClasses = array_merge($allDependencyClasses
        , array_keys($dependence->getClasses()));
    }
    $allDependencyClasses = array_unique($allDependencyClasses);
    $strategy = $this->getAnnoCompilerStrategyOfDependency();
    $strategy->setDependencyClasses($allDependencyClasses);

    $compiler->setStrategy($strategy);

    $this->postParseDependence($dependenceProcessor, $compiler);

    return [$dependenceProcessor, $compiler];
  }

  /**
   * @param array $allProjectFiles
   * @return string[] 按照解析顺序的排序的文件
   */
  private function processAnnotation(array $allProjectFiles
    , AnnotationCompiler $compiler):array {
    $this->printStep("process annotation");

    $sequenceFilesByCompiler
      = $compiler->compile($allProjectFiles, $this->isBuild());

    $newFilesInteOrPublish = array_values(array_diff($sequenceFilesByCompiler
      , $allProjectFiles));

    $annotationDir = Project::getInstance()->getAnnotationOutputDir();
    $annotationDir .= DIRECTORY_SEPARATOR . 'src';
    $rootDir = Project::getInstance()->getRootDir();

    $fileDirs = FileSystem::preGlob($annotationDir.'/**/*'
      , FileSystem::dirDeep($rootDir . '/' . $annotationDir));
    $files = (new FileFinder($rootDir, $fileDirs))->getAllFile();
    $newFilesNonInteOrPublish = array_diff($files, $newFilesInteOrPublish, []);

    $this->postProcessAnnotation($newFilesInteOrPublish, $newFilesNonInteOrPublish);

    return $sequenceFilesByCompiler;
  }

  private function parseFiles(array $allNeedParseFiles
    , DependenceProcessor $processor) {
    $this->printStep("parse files");

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
        if (array_key_exists($class, $newClasses)) {
          throw new NormalError(
            "$class are again defined both dependence:"
            .$dependence->getName() ." and Project: <root_dir>/{$newClasses[$class]}");
        }
      }
    }
  }

  private function printPhpFileError($errorFiles) {
    foreach ($errorFiles as $errorFile) {
      echo "error file: $errorFile " . PHP_EOL;
    }

    foreach ($errorFiles as $errorFile) {
      require_once $errorFile;
    }
  }

  protected function printStep(string $info, bool $end = false) {
    if ($this->step_ != 0) {
      $now = $this->currentMs();
      $time = $now - $this->lastStepStart_;
      $this->lastStepStart_ = $now;
      echo "   --- ${time}ms". PHP_EOL;
    } else {
      $this->lastStepStart_ = $this->currentMs();
    }

    if (!$end) {
      $this->step_++;
      echo "step $this->step_: $info... ".PHP_EOL ;
    } else {
      echo $info . PHP_EOL;
    }
  }

  private function currentMs():int {
    return (int)(microtime(true) * 1000);
  }

  protected function getAnnoCompilerStrategyOfDependency()
    :AnnotationCompilerStrategyOfDependency {
    return new AnnoCompilerStrategyOfCompileDependence();
  }

  protected function isBuild():bool {
    return false;
  }

  protected function postFindFile(array $files):void {}
  protected function postParseDependence(DependenceProcessor $processor
    , AnnotationCompiler $compiler):void {}
  protected function postProcessAnnotation(array $newFilesInteOrPublish
    , array $newFilesNotInteOrPublish):void {

    echo PHP_EOL;

    echo "new Files for InteOrPublish: (sum="
      . count($newFilesInteOrPublish) .")" . PHP_EOL;

    foreach ($newFilesInteOrPublish as $newFile) {
      echo $newFile . PHP_EOL;
    }

    echo PHP_EOL;

    echo "new Files for non-InteOrPublish: (sum="
      . count($newFilesNotInteOrPublish) .")" . PHP_EOL;

    foreach ($newFilesNotInteOrPublish as $newFile) {
      echo $newFile . PHP_EOL;
    }

    echo PHP_EOL;

  }
  protected function postParseFile(array $classFileMap
    , DependenceProcessor $processor):void {}

  /**
   * @var Config
   */
  private $config_;
  /**
   * @var SimpleLogger
   */
  private $logger_;
  /**
   * @var int
   */
  private $step_;
  private $lastStepStart_ = 0;
  /**
   * @var Project
   */
  private $project_;

}