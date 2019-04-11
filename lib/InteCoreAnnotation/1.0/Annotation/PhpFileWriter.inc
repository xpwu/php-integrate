<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/1
 * Time: 7:59 PM
 */

namespace Inte\Annotation;


interface PhpFileWriter {
  function dontInteOrPublish();
  function write(string $content);
  function rewrite(string $content);
}