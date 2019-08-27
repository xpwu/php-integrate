<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/3
 * Time: 1:00 AM
 */

namespace Inte\Annotation;


interface Logger {
  public function throwError(string $msg):void ;

  public function warn(string $msg):void ;

  public function info(string $msg):void ;
}