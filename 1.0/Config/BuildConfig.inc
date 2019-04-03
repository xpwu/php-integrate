<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 5:55 PM
 */

namespace Inte\Config;


class BuildConfig implements \Inte\Annotation\BuildConfig {
  function __construct(array $build) {
    $this->target = $build['target']??"main";
    $this->indexClass = $build['index_class']??"NoIndexClass_PleaseSetIt";
  }

  public $target = "main";
  public $indexClass = "NoIndexClass_PleaseSetIt";

  function getTarget(): string {
    mkdir($this->target, 0777, true);
    return $this->target;
  }

  function getIndexClass(): string {
    return $this->indexClass;
  }
}