<?php


namespace Inte\Annotation;


class AnnoCompilerStrategyOfNotCompileDependency
  extends AnnotationCompilerStrategyOfDependency {

  /**
   * @param AbstractAnnotationServer[] $servers
   */
  public function processBy(array $servers): void {
    // not need impl
  }
}