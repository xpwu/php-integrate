<?php


namespace Inte\Command;


use Inte\Command;
use Inte\Error\OK;

class ProcessAnnotation extends Command {

  protected function postProcessAnnotation(array $newFiles): void {

    echo "new Files: (sum=". count($newFiles) .")" . PHP_EOL;

    foreach ($newFiles as $newFile) {
      echo $newFile . PHP_EOL;
    }

    throw new OK();
  }
}