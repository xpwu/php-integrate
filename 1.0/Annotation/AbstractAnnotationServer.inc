<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 5:18 PM
 */

namespace Inte\Annotation;


use Inte\Config\Config;

interface AbstractAnnotationServer {
  function init(Logger $logger, PhpFiler $filer, Config $config):void;

  /**
   * @param string[] $classes
   */
  function process(array $classes):void;

}