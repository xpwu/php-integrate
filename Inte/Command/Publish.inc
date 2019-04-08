<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/24
 * Time: 8:25 PM
 */

namespace Inte\Command;


use Inte\ClassLoader;
use Inte\Command;
use Inte\Dependence\RepoLib;
use Inte\DependenceProcessor;
use Inte\DependenceSpec;
use Inte\Error\NormalError;
use Inte\Project;
use Inte\PublishWriter;

class Publish extends Command {

  /**
   * @param array $classFileMap
   * @param DependenceProcessor $processor
   * @throws \ReflectionException
   */
  protected function postParseFile(array $classFileMap
    , DependenceProcessor $processor): void {

    $this->printStep("publish to repo");

    $project = Project::getInstance();
    $config = $project->getConfig();

    if (!$config->publishConfig->isValid()) {
      throw new NormalError('$publish config is error!');
    }

    $name = $config->publishConfig->name;
    $version = $config->publishConfig->version;

    // check annotation service
    $allServices = $config->publishConfig->annotationServices;
    foreach ($allServices as $service) {
      $serviceClass = new \ReflectionClass($service);
      if (!$serviceClass->isSubclassOf(
        \Inte\Annotation\AbstractAnnotationServer::class)) {
        throw new NormalError($service
          . " is not a annotationService, which must be subclass of "
          . \Inte\Annotation\AbstractAnnotationServer::class);
      }
    }

    $publishWriter = new PublishWriter($project->getRootDir()
      , $config->publishConfig->repo, $name, $version);
    $publishWriter->write($classFileMap, $config->dependencies, $allServices);

  }
}