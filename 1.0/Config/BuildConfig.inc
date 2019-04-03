<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 5:55 PM
 */

namespace Inte\Config;


use Inte\Project;

class BuildConfig implements \Inte\Annotation\BuildConfig {
  function __construct(array $build) {
    $this->target = $build['target']??"main";
    $this->indexClass = $build['index_class']??"NoIndexClass_PleaseSetIt";
  }

  private $target = "main";
  private $indexClass = "NoIndexClass_PleaseSetIt";

  function getTarget(): string {
    if ($this->target === "") {
      $this->target = Project::getInstance()->getBuildOutputDir()
        . DIRECTORY_SEPARATOR . "main";
    }
    @mkdir($this->target, 0777, true);
    return $this->target;
  }

  function getIndexClass(): string {
    if ($this->indexClass === "") {
      $this->indexClass = "NoIndexClass_PleaseSetIt";
    }
    return $this->indexClass;
  }
}