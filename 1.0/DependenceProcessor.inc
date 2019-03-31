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

  /**
   * @param Dependence[] $dependence
   */
  public function appendDependence(array $dependencies) {
    $this->dependencies_[] = array_merge($this->dependencies_[], $dependencies);

    if (!$this->hasMerged()) {
      return;
    }

    $this->reMergeDependencies();
  }

  private function hasMerged():bool {
    return isset($this->mergedNameDependenceMap_);
  }

  private function reMergeDependencies() {
    $this->mergedNameDependenceMap_ = null;
    $this->mergeDependencies();
  }

  private function checkDependenceClass() {
    $allClasses = [];
    /**
     * @var $dependency Dependence
     */
    foreach ($this->mergedNameDependenceMap_ as $name => $dependency) {
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
    if ($this->hasMerged()) {
      return;
    }

    $allDependencies = [];

    /**
     * @var $value Dependence
     */
    foreach ($this->dependencies_ as $value) {
      $this->postOrderTraversal($allDependencies, $value);
    }

    $this->mergedNameDependenceMap_ = [];
    /**
     * @var $dependency Dependence
     */
    foreach ($allDependencies as $dependency) {
      if (!array_key_exists($dependency->getName(), $this->mergedNameDependenceMap_)) {
        $this->mergedNameDependenceMap_[$dependency->getName()] = $dependency;
        continue;
      }

      /**
       * @var $old Dependence
       */
      $old = $this->mergedNameDependenceMap_[$dependency->getName()];
      if (version_compare($old->getVersion()
        , $dependency->getVersion(), '>=')) {
        continue;
      }

      $this->mergedNameDependenceMap_[$dependency->getName()] = $dependency;
    }

    $this->checkDependenceClass();
  }

  private function postOrderTraversal(array &$allDependencies
    , Dependence $dependence) {
    $subDep = $dependence->getSubDependencies();

    foreach ($subDep as $value) {
      $this->postOrderTraversal($allDependencies, $value);
    }
    $allDependencies[] = $dependence;
  }

  /**
   * @return array  [Dependence]
   */
  public function getMergeResult():array {
    return array_values($this->mergedNameDependenceMap_);
  }

  public function updateVersionByOther(DependenceProcessor $other) {
    if (!isset($this->mergedNameDependenceMap_)
      || !isset($other->mergedNameDependenceMap_)) {
      throw new \Exception("DependenceProcessor has not merged");
    }

    /**
     * @var $intersect Dependence[]
     */
    $intersect = array_intersect_key($this->mergedNameDependenceMap_
      , $other->mergedNameDependenceMap_);

    $reset = false;
    foreach ($intersect as $name => $dependence) {
      if (version_compare($this->mergedNameDependenceMap_[$name]->getVersion()
        , $other->mergedNameDependenceMap_[$name]->getVersion(), "<")) {
        $this->mergedNameDependenceMap_[$name] = $other->mergedNameDependenceMap_[$name];
        $reset = true;
      }
    }

    if ($reset) {
      $this->checkDependenceClass();
    }
  }

  public function loadDependencies() {

    $allClasses = [];
    /**
     * @var $dependency Dependence
     */
    foreach ($this->mergedNameDependenceMap_ as $name=> $dependency) {

      $allClasses = array_merge($allClasses, $dependency->getClasses());

      foreach ($dependency->getRequireFiles() as $item) {
        require_once($item);
      }
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
      . $dependence->getName() . ':' . $dependence->getVersion()
      . " -> " . $this->mergedNameDependenceMap_[$dependence->getName()]->getVersion()
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
  private $mergedNameDependenceMap_ = null;
}