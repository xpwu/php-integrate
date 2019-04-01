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
use Inte\DependenceProcessor;
use Inte\DependenceSpec;
use Inte\Error\NormalError;
use Inte\Project;

class Publish extends Command {

  protected function postParseFile(array $classFileMap
    , DependenceProcessor $processor): void {
    $project = Project::getInstance();
    $config = $project->getConfig();
    $name = $config->publishConfig->name;
    $version = $config->publishConfig->version;

    // todo: 后续应该由文件上传下载器 获取
    $dir = $config->publishConfig->repo
      . DIRECTORY_SEPARATOR . $name . DIRECTORY_SEPARATOR . $version;

    mkdir($dir, 0777, true);

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


    $pwd = getcwd();
    chdir($project->getRootDir());
    $classLoader = new ClassLoader($dir, $name);
    $classLoader->write($classFileMap);

    $spec = new DependenceSpec($dir);
    $spec->write($name, $version, $config->dependencies, $allServices);

    foreach ($classFileMap as $value) {
      mkdir(dirname($value), 0777, true);
      copy($value, $dir .DIRECTORY_SEPARATOR. $value);
    }

    chdir($pwd);
  }
}