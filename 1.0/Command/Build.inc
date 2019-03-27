<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/24
 * Time: 2:27 PM
 */

namespace Inte\Command;

use Inte\Command;

class Build extends Command {

  protected function output(): void {
    /**
     * 1、生成整体的classLoader 包括dependence的
     * 2、加入所有的文件，包括dependence的
     * 3、生成stub文件
     * 4、写phar
     */

    $this->allClasses_;
  }
}