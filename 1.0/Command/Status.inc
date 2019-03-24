<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/24
 * Time: 3:50 PM
 */

namespace Inte\Command {


  interface Status {
    public function toValue(): int;
  }

}

namespace Inte\Command\Status {

  use Inte\Command\Status;

  class OK implements Status {

    public function toValue(): int {
      return 0;
    }
  }
}

