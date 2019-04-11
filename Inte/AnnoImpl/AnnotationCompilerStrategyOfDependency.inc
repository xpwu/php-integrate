<?php


namespace Inte\Annotation;


abstract class AnnotationCompilerStrategyOfDependency {
  public function setDependencyClasses(array $dependencyClasses) {
    $this->dependencyClasses_ = $dependencyClasses;
  }

  /**
   * @param AbstractAnnotationServer[] $servers
   */
  abstract public function processBy(array $servers):void ;

  protected $dependencyClasses_;

}