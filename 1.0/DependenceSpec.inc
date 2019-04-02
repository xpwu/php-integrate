<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/25
 * Time: 8:44 PM
 */

namespace Inte;


use Inte\Error\NormalError;

class DependenceSpec {
  public const NameKey = "name";
  public const VersionKey = "version";
  public const DependenciesKey = "dependencies";
  public const AnnotationServerKey = "annotationServer";

  private const FileName = "__spec.conf__.hphp";

  function __construct(string $dir) {
    $this->dir_ = $dir;
  }

  public function write(string $name, string $version
    , array $dependencies, array $annotationServer = []) {
    $arr = implode(",", $dependencies);

    $annServer = "";
    if (count($annotationServer) != 0) {
      $annServer = "\$annotationServer = ["
        . implode(",", $annotationServer) . "]";
    }

    $content = <<<EOF
<?php

\$name = {$name};
\$version = {$version};

\$dependencies = [{$arr}];

$annServer;

EOF;

    file_put_contents($this->dir_ . DIRECTORY_SEPARATOR . self::FileName
      , $content);
  }

  /**
   * @return array [key=>value]
   */
  public function read():array {
    if ($this->result_ !== null) {
      return $this->result_;
    }

    $annotationServer = [];
    $dependencies = [];

    require_once $this->dir_ . DIRECTORY_SEPARATOR . self::FileName;

    $this->result_ = [self::NameKey=>$name, self::VersionKey=>$version
      , self::DependenciesKey=>$dependencies];

    $this->result_[self::AnnotationServerKey] = $annotationServer;


    return $this->result_;
  }

  static public function getFileName():string {
    return self::FileName;
  }

  public function checkIsADependence():void {
    $ret = $this->read();

    $annServer = $ret[self::AnnotationServerKey];
    if ($annServer != null && count($annServer) != 0) {
      throw new NormalError($ret[self::NameKey] ." [@${ret[self::VersionKey]}]"
        . " is a Annotation Processor, which can Not be a dependence.");
    }
  }

  public function checkIsAAnnotationProcessor():void {
    $ret = $this->read();

    $annServer = $ret[self::AnnotationServerKey];
    if ($annServer == null || count($annServer) == 0) {
      throw new NormalError($ret[self::NameKey] ." [@${ret[self::VersionKey]}]"
        . " is Not a Annotation Processor, which has Not any annotationServer supported.");
    }
  }

  private $dir_;
  private $result_ = null;
}