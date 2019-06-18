<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/1
 * Time: 8:02 PM
 */

namespace Inte\Annotation;


interface PhpFiler {
  function createSourceFile(string $fileDirAndName): PhpFileWriter;
}