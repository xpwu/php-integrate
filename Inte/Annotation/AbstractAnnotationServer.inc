<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 5:18 PM
 */

namespace Inte\Annotation;


use Inte\Config\Config;

abstract class AbstractAnnotationServer {
  function init(Logger $logger, PhpFiler $filer
    , Config $config, string $projectRootDir
    , bool $isBuild_):void {}

  /**
   * @param string[] $classes
   */
  abstract function process(array $classes):void;

  public function processDependencyClasses(array $dependencyClasses):void {}
}