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

    if (strpos($dependenceStr, ":") !== false) {
      $arr = explode(":", $dependenceStr);
      $name = $arr[0];
      $version = $arr[1];

      $position = new SystemLib($repo, $name, $version);
      if (RepoLib::isThis($repo, $name, $version)) {
        $position = new RepoLib($repo, $name, $version);
      }

      $content = new LibNameVersion($position, $repo);

      return new Dependence($position, $content);
    }

//    if (strpos($dependenceStr, "/") !== false) {
//      $arr = explode(":", $dependenceStr);
//      $name = $arr[0];
//      $version = $arr[1];
//
//      $position = new SystemLib($repo, $name, $version);
//      if (RepoLib::isThis($repo, $name, $version)) {
//        $position = new RepoLib($repo, $name, $version);
//      }
//
//      $content = new LibNameVersion($position, $repo);
//
//      return new Dependence($position, $content);
//    }

    throw new NormalError("Not support lib: $dependenceStr with repo: $repo");
  }

  static public function convertFromAnnoProcessor(string $repo
    , string $name, string $version):Dependence {

    $position = new SystemLib($repo, $name, $version);
    if (RepoLib::isThis($repo, $name, $version)) {
      $position = new RepoLib($repo, $name, $version);
    }

    $content = new LibNameVersion($position, $repo, true);

    return new Dependence($position, $content);
  }

}