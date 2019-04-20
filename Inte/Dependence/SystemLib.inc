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
    $includePathStr = get_include_path();
    $includePaths = explode(':', $includePathStr);
    $actual = [];

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
        return $absolute;
      }
    }

    throw new NormalError("<name:" . $this->getName()
      . " version:" . $this->getVersion()
      . ">. Lib not found in " . implode(",", $actual));
  }
}