<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/2
 * Time: 6:53 PM
 */

namespace Inte\Dependence;



use Inte\Error\NormalError;

class SystemLib extends Position {

  function getAllNeedFilesForInte(Content $content): array {
    return [];
  }

  function getTopDir(): string {
    return $this->getRelativePathOfRepo();
  }

  function getAbsoluteTopDir(): string {
    return $this->getRootDir() . DIRECTORY_SEPARATOR . $this->getTopDir();
  }

  function getRootDir(): string {
    $includePathStr = get_include_path();
    $includePaths = explode(':', $includePathStr);
    $actual = [];

    // phar:// 协议的路径会被错误的分割，所以需要特殊处理
    $pwd = getcwd();
    $last = "";
    foreach ($includePaths as $includePath) {
      if ($includePath === ".") {
        $includePath = $pwd;
      }
      if ($includePath === "phar") {
        $last = "phar:";
        continue;
      }
      $actual[] = $last.$includePath;
      $last = "";

      $absolute = $includePath . DIRECTORY_SEPARATOR . $this->getTopDir();
      if (file_exists($absolute)) {
        return $includePath;
      }
    }

    throw new NormalError("<name:" . $this->getName()
      . " version:" . $this->getVersion()
      . ">. Lib not found in " . implode(",", $actual));
  }
}