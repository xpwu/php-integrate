<?php

/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 11:51 PM
 */

namespace Inte;

class ClassLoader {
  private const FileName = "ClassLoader.inc";

  function __construct(string $dir, string $libName = "Project") {
    $this->dir_ = $dir;
    $this->libName_ = $libName;
  }

  /**
   * @param array [class(string) => file(string)]
   */
  public function write(array $classes) {
    $arr = "";
    foreach ($classes as $class=>$file) {
      $arr = $arr . "\t\t\"" . $class . '"=>"' . $file . "\"," . PHP_EOL;
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


    file_put_contents($this->dir_ . DIRECTORY_SEPARATOR . self::FileName
      , $content);

  }

  /**
   * @return array [class => file]
   */
  public function read():array {
    require_once $this->dir_ . DIRECTORY_SEPARATOR . self::FileName;
    /**
     * @var array
     */
    $cls = $this->libName_ . '\ClassLoader';

    return $cls::AllClasses;

  }

  private $dir_;
  private $libName_;

}