<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/2
 * Time: 6:53 PM
 */

namespace Inte\Dependence;


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
    if (file_exists($dir)) {
      return true;
    }

    return false;
  }
}