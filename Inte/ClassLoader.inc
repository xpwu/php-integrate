<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 11:51 PM
 */

namespace Inte;

/**
 *
 * 路径规则：
 *  ClassLoader 中的文件路径都是相对于 rootDir 的路径
 *
 */

class ClassLoader {
  private const FileName = "__InteClassLoader__.inc";

  function __construct(string $dir, string $libName = "Project") {
    // 去掉前面的  ./
    if ($dir == ".") {
      $this->dir_ = "";
    } else {
      $this->dir_ = $dir . DIRECTORY_SEPARATOR;
    }

    $this->libName_ = $libName;
  }

  /**
   * @param array [class(string) => file(string)]
   */
  public function write(array $classes) {
    $arr = "";
    foreach ($classes as $class=>$file) {
      $arr = $arr . PHP_EOL . "\t\t\"" . $class . '" => "' . $file . "\",";
    }

    $content = <<< EOF
<?php

namespace {$this->libName_};

/*
 * ------Auto Created By Integrator, DO NOT modify this file------
*/

class ClassLoader {
  public const AllClasses = [
    {$arr}
  ];
}

EOF;


    file_put_contents($this->dir_ . self::FileName
      , $content);

  }

  /**
   * @return array [class => file]
   */
  public function read():array {
    require_once $this->dir_ . self::FileName;
    /**
     * @var array
     */
    $cls = $this->libName_ . '\ClassLoader';

    return $cls::AllClasses;

  }

  public function getArrayVarString():string {
    return $this->libName_ . '\ClassLoader::AllClasses';
  }

  public function getFile():string {
    return $this->dir_  . self::FileName;
  }

  private $dir_;
  private $libName_;

}