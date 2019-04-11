<?php


namespace Inte\Annotation;


class AnnoCompilerStrategyOfCompileDependence
  extends AnnotationCompilerStrategyOfDependency {

  /**
   * @param AbstractAnnotationServer[] $servers
   */
  public function processBy(array $servers): void {
    foreach ($servers as $server) {
      $server->processDependencyClasses($this->dependencyClasses_);
    }
  }
}