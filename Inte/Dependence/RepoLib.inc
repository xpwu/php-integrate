<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/2
 * Time: 6:53 PM
 */

namespace Inte\Dependence;


use Inte\Project;

class RepoLib extends Position {
  function getAllNeedFilesForInte(Content $content): array {
    return $content->getAllNeedFilesForInte();
  }

  function getTopDir(): string {
    return $this->repo_ . DIRECTORY_SEPARATOR
      . $this->getRelativePathOfRepo();
  }

  public static function isThis(string $repo
    , string $name, string $version):bool {

    $dir = $repo . DIRECTORY_SEPARATOR . $name . DIRECTORY_SEPARATOR . $version;
    // 因为 Phar::interceptFileFuncs(), 所以查找时需要加上 rootDir 变为绝对路径
    if (file_exists(Project::getInstance()->getRootDir()
      . DIRECTORY_SEPARATOR . $dir)) {
      return true;
    }

    return false;
  }
}