<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/25
 * Time: 9:33 PM
 */

namespace Inte\Dependence;


use Inte\Dependence;
use Inte\Error\NormalError;

class Factory {
  static public function getDependence(string $repo
    , string $dependenceStr):Dependence {

    if (LibNameVersion::isThis($repo, $dependenceStr)) {
      return new LibNameVersion($repo, $dependenceStr);
    }

    throw new NormalError("Not support lib: $dependenceStr with repo: $repo");
  }
}