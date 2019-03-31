<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 5:18 PM
 */

namespace Inte\Annotation;


use Inte\Config\Config;
use Inte\Logger;


abstract class AbstractAnnotationServer {
  function init(Logger $logger, PhpFiler $filer, Config $config):void {}

  abstract function process(array $classes):void;

}