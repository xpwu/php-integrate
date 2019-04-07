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

    // name:version
    if (strpos($dependenceStr, ":") !== false) {
      $arr = explode(":", $dependenceStr);
      $name = $arr[0];
      $version = $arr[1];

      $position = new SystemLib($repo, $name, $version);
      if (RepoLib::isThis($repo, $name, $version)) {
        $position = new RepoLib($repo, $name, $version);
      }

      $content = new LibNameVersion($position);

      return new Dependence($position, $content);
    }

    //  name/version/[a.php;c.inc;d/*.inc;Class@file.inc]
    if (strpos($dependenceStr, "/") !== false) {
      $arr = explode("[", $dependenceStr);
      $nameV = explode("/", $arr[0]);
      $name = $nameV[0];
      $version = $nameV[1];

      $position = new SystemLib($repo, $name, $version);
      if (RepoLib::isThis($repo, $name, $version)) {
        $position = new RepoLib($repo, $name, $version);
      }

      $content = new RequireFile($position, substr($arr[1], 0, -1));

      return new Dependence($position, $content);
    }

    throw new NormalError("Not support lib: $dependenceStr with repo: $repo");
  }

  static public function convertFromAnnoProcessor(string $repo
    , string $name, string $version):Dependence {

    $position = new SystemLib($repo, $name, $version);
    if (RepoLib::isThis($repo, $name, $version)) {
      $position = new RepoLib($repo, $name, $version);
    }

    $content = new LibNameVersion($position, true);

    return new Dependence($position, $content);
  }

}