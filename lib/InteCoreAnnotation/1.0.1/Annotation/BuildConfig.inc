<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/3
 * Time: 1:07 AM
 */

namespace Inte\Annotation;


interface BuildConfig {
  function getTarget():string ;
  function getIndexClass():string ;
}