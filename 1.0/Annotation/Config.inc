<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/3
 * Time: 1:06 AM
 */

namespace Inte\Annotation;


interface Config {
  function getBuildConfig():BuildConfig ;
  function getPublishConfig():PublishConfig;
}