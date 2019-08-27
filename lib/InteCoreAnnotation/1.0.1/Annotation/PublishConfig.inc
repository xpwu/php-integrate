<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/3
 * Time: 1:07 AM
 */

namespace Inte\Annotation;


interface PublishConfig {
  function getRepo():string ;
  function getName():string ;
  function getVersion():string ;
  function getAnnotationServices():array ;
}