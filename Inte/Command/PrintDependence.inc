<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/4/2
 * Time: 1:48 AM
 */

namespace Inte\Command;


use Inte\AnnotationCompiler;
use Inte\Command;
use Inte\DependenceProcessor;
use Inte\Error\OK;

class PrintDependence extends Command {
  protected function postParseDependence(DependenceProcessor $processor
    , AnnotationCompiler $compiler):void {

    echo PHP_EOL;

    echo '<dependence>' . PHP_EOL;
    $processor->printDependencies("  |");

    echo PHP_EOL;

    echo "<annotation-processor>" . PHP_EOL;
    $compiler->printDependence("  |");

    echo PHP_EOL;

    throw new OK();
  }
}