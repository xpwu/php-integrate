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
    $this->dependencies_ = array_merge($this->dependencies_, $dependencies);

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
   * @return Dependence[]
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
    $count = count($this->dependencies_);
    $index = 0;

    $last = false;
    foreach ($this->dependencies_ as $dependence) {
      // 最后一个子依赖时，应该把父依赖的prefix的最后一个 "|" 换成 " "
      $index++;
      if ($index === $count) {
        $last = true;
      }

      $this->printDeepFirstly($prefix, $dependence, $last);
    }
  }

  private function printDeepFirstly(string $prefix
    , Dependence $dependence, bool $lastOne) {

    echo $prefix . "--"
      . $dependence->getName() . ':' . $dependence->getVersion()
      . " -> " . $this->mergedNameDependenceMap_[$dependence->getName()]->getVersion()
      . PHP_EOL;

    // 最后一个子依赖时，应该把父依赖的prefix的最后一个 "|" 换成 " "
    if ($lastOne) {
      $i = strrpos($prefix, "|");
      if ($i !== false) {
        $prefix[$i] = " ";
      }
    }

    $subDependencies = $dependence->getSubDependencies();
    $count = count($subDependencies);
    $index = 0;

    $last = false;
    foreach ($subDependencies as $subDependency) {

      $index++;
      if ($index === $count) {
        $last = true;
      }

      $this->printDeepFirstly($prefix  . "   |", $subDependency, $last);
    }

    // 子块结束时，也就是要继续输出上一级时，输出一个空行。
    // 具体就是自己有子块并打印结束，同时自己不是上一级的最后一个子块,
    // 此时会打印与我同级的下一条，归纳起来就是从打印自己的子块到打印自己
    // 的兄弟块过渡，相对于子块就是要输出上一级了，所以输出空行。
    // 如果是上一级的最后一个子块，不清楚后面是否还要输出，也就是不清楚是否会有上一级
    // 所以不输出空行
    if ($count !== 0 && !$lastOne) {
      echo $prefix . PHP_EOL;
    }
  }

  private $repo_;
  /**
   * @var Dependence[]
   */
  private $dependencies_ = [];
  /**
   * @var Dependence[]
   */
  private $mergedNameDependenceMap_ = null;
}