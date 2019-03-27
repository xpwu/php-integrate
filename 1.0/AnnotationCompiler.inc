<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 11:22 PM
 */

namespace Inte;


class AnnotationCompiler {
  function __construct(array $annotationProcessor
    , DependenceProcessor $dependenceProcessor
    , string $buildAnnotationDir) {
  }

  /**
   * @return array file [string]
   */
  public function compile(array $classFile):array {

  }
}