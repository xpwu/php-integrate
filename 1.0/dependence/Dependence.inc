<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 12:27 PM
 */

namespace Inte;

/**
 *
 * 路径信息：
 * 返回的所有文件或者路径都是相对于Project->rootDir的路径 或者 相对于系统include_path中的某一路径
 *
 * todo: include_path 中的依赖文件还没有考虑
 */

interface Dependence {
  /**
   * @return array  [className=>file]
   */
  function getClasses():array ;
  function getRequireFiles():array ;

  /**
   * @return Dependence[]
   */
  function getSubDependencies():array ;

  function getAllFiles():array ;

  function getTopDir():string ;

  function getName():string ;
  function getVersion():string ;
  function getRepo():string ;
}