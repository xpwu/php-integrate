<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/2
 * Time: 6:53 PM
 */

namespace Inte\Dependence;



class SystemLib extends Position {

  function getAllNeedFilesForInte(Content $content): array {
    return [];
  }

  function getTopDir(): string {
    return $this->getRelativePathOfRepo();
  }
}