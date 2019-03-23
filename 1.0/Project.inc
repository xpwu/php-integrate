<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 4:04 PM
 */

namespace Inte;


use Inte\Config\Config;

class Project {
  /**
   * @var array [libName=>Dependence]
   */
  private $dependencies = [];
  /**
   * @var Config
   */
  private $conf;
}