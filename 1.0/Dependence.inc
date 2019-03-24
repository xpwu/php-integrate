<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 12:27 PM
 */

namespace Inte;


interface Dependence {
  /**
   * @return array  [className=>file]
   */
  function getClasses():array ;
  function getRequireFile():string ;

  /**
   * @return array [Dependence]
   */
  function getSubDependencies():array ;

  function getAllFiles():array ;

  function getName():string ;
  function getVersion():string ;
}