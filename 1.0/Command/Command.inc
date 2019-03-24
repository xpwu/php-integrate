<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/24
 * Time: 2:18 PM
 */


namespace Inte\Command;

interface Command {
  function run():Status ;
}