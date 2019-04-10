<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/3
 * Time: 12:26 AM
 */

namespace Inte\Dependence;


use Inte\Dependence;
use Inte\FileFinder;
use Inte\FileSystem;
use Inte\Project;

class RequireFile extends Content {

  /**
   * RequireFile constructor.
   * @param Position $position
   * @param string $fileOrClassDes  "a.php;c.inc;d/*.inc;Class@file.inc"
   */
  function __construct(Position $position, string $fileOrClassDes) {
    parent::__construct($position);
    $arr = explode(";", $fileOrClassDes);
    foreach ($arr as $value) {
      if (strpos($value, "@") === false) {
        $this->globFiles_[] = $value;
        continue;
      }

      $classFile = explode("@", $value);
      $this->classFileMap_[$classFile[0]] = $classFile[1];
    }
  }

  /**
   * @return array  [className=>file]
   */
  function getClasses(): array {
    return $this->classFileMap_;
  }

  function getRequireFiles(): array {
    return $this->getFilesByFilter($this->globFiles_);
  }

  /**
   * @return Dependence[]
   */
  function getSubDependencies(string $repo): array {
    return [];
  }

  function getAllNeedFilesForInte(): array {
    $fileFinder = new FileFinder($this->getPosition()->getTopDir()
      , FileSystem::preGlobArray(['**/*']
        , FileSystem::dirDeep(Project::getInstance()->getRootDir())));

    return array_map(function ($file){
      return $this->getPosition()->getTopDir(). DIRECTORY_SEPARATOR . $file;
    }, $fileFinder->getAllFile());
  }

  /**
   * @var string[]
   */
  private $globFiles_ = [];
  private $classFileMap_ = [];
}