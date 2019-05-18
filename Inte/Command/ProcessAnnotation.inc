<?php


namespace Inte\Command;


use Inte\Command;
use Inte\Error\OK;
use Inte\FileFinder;
use Inte\FileSystem;
use Inte\Project;

class ProcessAnnotation extends Command {

  protected function postProcessAnnotation(array $newFilesInteOrPublish
    , array $newFilesNotInteOrPublish): void {

    parent::postProcessAnnotation($newFilesInteOrPublish, $newFilesNotInteOrPublish);

    throw new OK();
  }
}