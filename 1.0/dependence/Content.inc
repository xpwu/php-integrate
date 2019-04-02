<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/2
 * Time: 7:03 PM
 */

namespace Inte\Dependence;


use Inte\Dependence;

/**
 *
 * 路径信息：
 * 返回的所有文件或者路径都是相对于Project->rootDir的路径 或者 相对于系统include_path中的某一路径
 *
 */

abstract class Content {
  function __construct(Position $position) {
    $this->position_ = $position;
  }

  /**
   * @return array  [className=>file]
   */
  abstract function getClasses():array ;
  abstract function getRequireFiles():array ;

  /**
   * @return Dependence[]
   */
  abstract function getSubDependencies(string $repo):array ;

  abstract function getAllNeedFilesForInte():array ;

  protected function getPosition():Position {
    return $this->position_;
  }

  private $position_;
}