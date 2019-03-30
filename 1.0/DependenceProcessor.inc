<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 10:47 PM
 */

namespace Inte;


use Inte\Dependence\Factory;
use Inte\Error\NormalError;

class DependenceProcessor {
  function __construct(string $repo, array $dependencies) {
    $this->repo_ = $repo;

    foreach ($dependencies as $dependency) {
      $this->dependencies_[] = Factory::getDependence($repo, $dependency);
    }
  }

  private function checkDependenceClass() {
    $allClasses = [];
    /**
     * @var $dependency Dependence
     */
    foreach ($this->mergedDependencies_ as $name => $dependency) {
      foreach (array_keys($dependency->getClasses()) as $class) {
        if (!array_key_exists($class, $allClasses)) {
          $allClasses[$class] = $name;
          continue;
        }

        if ($allClasses[$class] === $name) {
          continue;
        }

        throw new NormalError(
          "$class are again defined both $name and $allClasses[$class]");
      }
    }
  }

  public function mergeDependencies() {
    if (isset($this->mergedDependencies_)) {
      return;
    }

    $allDependencies = $this->dependencies_;

    /**
     * @var $value Dependence
     */
    foreach ($this->dependencies_ as $value) {
      $this->traverseDeepFirstly($allDependencies, $value);
    }

    $this->mergedDependencies_ = [];
    /**
     * @var $dependency Dependence
     */
    foreach ($allDependencies as $dependency) {
      if (!array_key_exists($dependency->getName(), $this->mergedDependencies_)) {
        $this->mergedDependencies_[$dependency->getName()] = $dependency;
        continue;
      }

      /**
       * @var $old Dependence
       */
      $old = $this->mergedDependencies_[$dependency->getName()];
      if (version_compare($old->getVersion()
        , $dependency->getVersion(), '>=')) {
        continue;
      }

      $this->mergedDependencies_[$dependency->getName()] = $dependency;
    }

    $this->checkDependenceClass();
  }

  private function traverseDeepFirstly(array &$allDependencies
    , Dependence $dependence) {
    $subDep = $dependence->getSubDependencies();

    foreach ($subDep as $value) {
      $allDependencies[] = $value;
      $this->traverseDeepFirstly($allDependencies, $value);
    }
  }

  /**
   * @return array  [Dependence]
   */
  public function getMergeResult():array {
    return array_values($this->mergedDependencies_);
  }

  public function loadDependencies() {
    set_include_path($this->repo_
      . PATH_SEPARATOR . get_include_path());

    $allClasses = [];
    /**
     * @var $dependency Dependence
     */
    foreach ($this->mergedDependencies_ as $dependency) {
      $allClasses = array_merge($allClasses, $dependency->getClasses());
    }

    spl_autoload_register(function (string $class) use ($allClasses) {
      if (array_key_exists($class, $allClasses)) {
        require_once($allClasses[$class]);
        return true;
      }

      return false;
    });
  }

  public function printDependencies(string $prefix) {
    foreach ($this->dependencies_ as $dependence) {
      $this->printDeepFirstly($prefix, $dependence);
    }
  }

  private function printDeepFirstly(string $prefix, Dependence $dependence) {
    echo $prefix . "--"
      . $dependence->getName() . '@' . $dependence->getVersion()
      . " -> " . $this->mergedDependencies_[$dependence->getName()]->getVersion()
      . PHP_EOL;

    foreach ($dependence->getSubDependencies() as $subDependency) {
      $this->printDeepFirstly($prefix . "  |", $subDependency);
    }

    echo $prefix . PHP_EOL;
  }

  private $repo_;
  /**
   * @var Dependence[]
   */
  private $dependencies_;
  /**
   * @var Dependence[]
   */
  private $mergedDependencies_ = null;
}