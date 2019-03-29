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
use Inte\DependenceSpec;
use Inte\Error\NormalError;
use Inte\Project;

class Publish extends Command {

  protected function output(): void {
    $project = Project::getInstance();
    $name = $project->getConfig()->publishConfig->name;
    $version = $project->getConfig()->publishConfig->version;

    // todo: 后续应该由文件上传下载器 获取
    $dir = $project->getConfig()->publishConfig->repo
      . DIRECTORY_SEPARATOR . $name . DIRECTORY_SEPARATOR . $version;

    mkdir($dir, 0777, true);

    // check annotation service
    $allServices = $project->getConfig()->publishConfig->annotationServices;
    foreach ($allServices as $service) {
      $serviceClass = new \ReflectionClass($service);
      if (!$serviceClass->isSubclassOf(\Inte\Annotation\AbstractProcessor::class)) {
        throw new NormalError($service
          . " is not a annotationService, which must be subclass of "
          . \Inte\Annotation\AbstractProcessor::class);
      }
    }


    $pwd = getcwd();
    chdir($project->getRootDir());
    $classLoader = new ClassLoader($dir, $name);
    $classLoader->write($this->allProjectClasses_);

    $spec = new DependenceSpec($dir);
    $spec->write($name, $version, $this->allProjectClasses_, $allServices);

    foreach ($this->allProjectClasses_ as $value) {
      mkdir(dirname($value), 0777, true);
      copy($value, $dir .DIRECTORY_SEPARATOR. $value);
    }

    chdir($pwd);
  }
}