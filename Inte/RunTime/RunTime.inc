<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/4
 * Time: 3:27 PM
 */

namespace Inte;


interface RunTime {
  public function getPharDir():string ;
  public function getPharName():string ;
}