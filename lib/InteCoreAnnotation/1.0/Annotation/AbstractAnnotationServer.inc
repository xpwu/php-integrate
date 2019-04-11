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
  abstract function init(Logger $logger, PhpFiler $filer, Config $config):void;

  /**
   * @param string[] $classes
   */
  abstract function process(array $classes):void;

  public function processDependencyClasses(array $dependencyClasses):void {}
}