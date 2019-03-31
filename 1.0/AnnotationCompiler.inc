<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 11:22 PM
 */

namespace Inte;


use Inte\Annotation\AbstractAnnotationServer;
use Inte\Annotation\PhpFiler;
use Inte\Error\NormalError;

class AnnotationCompiler {
  /**
   * AnnotationCompiler constructor.
   * @param string[] $annotationProcessors
   * @param string $buildAnnotationDir
   */
  function __construct(array $annotationProcessors
    , string $buildAnnotationDir) {
    $this->repo_ = Project::getInstance()->getConfig()->annoAndDepRepo;

    foreach ($annotationProcessors as $annotationProcessor) {
      $this->processors_[] = new AnnotationProcessor($this->repo_
        , $annotationProcessor);
    }

    $this->distDir_ = $buildAnnotationDir;
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

    $this->serversStr_ = [];
    foreach ($this->processors_ as $annotationProcessor) {
      $this->serversStr_ = array_merge($this->serversStr_
        , $annotationProcessor->getServersString());
    }
  }

  public function getDependenceProcessor():DependenceProcessor {
    if (!isset($this->dependenceProcessor_)) {
      throw new \Exception(
        "AnnotationCompiler must call 'init' before 'getDependenceProcessor'");
    }
    return $this->dependenceProcessor_;
  }

  /**
   * @return string[]
   */
  public function compile(array $classNames):array {

    $beforeFinder = new FileFinder($this->distDir_, ["**/*.inc"]);
    $beforeFiles = $beforeFinder->getAllFile();

    $pid = pcntl_fork();
    switch ($pid) {
      case -1:
        throw new NormalError("pcntl_fork error!");
      case 0 : {
//        error_reporting(0);
        try {
          $this->compileByChild($classNames);
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

    $afterFinder = new FileFinder($this->distDir_, ["**/*.inc"], $beforeFiles);
    return $afterFinder->getAllFile();
  }

  private function compileByChild(array $classNames) {
    /**
     * 只是被注解编译器使用的依赖在注解处理环境中加载，不能影响到项目环境
     */
    $this->dependenceProcessor_->loadDependencies();
    /**
     * @var AbstractAnnotationServer[] $servers
     */
    $servers = [];
    foreach ($this->serversStr_ as $item) {
      $servers[] = new $item();
    }

    foreach ($servers as $server) {
      $server->init(new Logger(), new PhpFiler($this->distDir_)
        , Project::getInstance()->getConfig());
      $server->process($classNames);
    }

  }

  /**
   * @var AnnotationProcessor[]
   */
  private $processors_ = [];
  private $serversStr_ = [];
  private $distDir_;
  /**
   * @var DependenceProcessor
   */
  private $dependenceProcessor_ = null;
  private $repo_;
}