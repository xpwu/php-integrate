<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/25
 * Time: 8:44 PM
 */

namespace Inte;


class DependenceSpec {
  public const NameKey = "name";
  public const VersionKey = "version";
  public const DependenciesKey = "dependencies";
  public const AnnotationServerKey = "annotationServer";

  private const FileName = "spec.conf.hphp";

  function __construct(string $dir) {
    $this->dir_ = $dir;
  }

  public function write(string $name, string $version
    , array $dependencies, ?string $annotationServer = null) {
    $arr = "";
    foreach ($dependencies as $dependence) {
      $arr = $arr . $dependence . ", ";
    }

    $annServer = "";
    if (!$annotationServer) {
      $annServer = "\$annotationServer = $annotationServer";
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
    $annotationServer = null;
    $dependencies = [];

    require_once $this->dir_ . DIRECTORY_SEPARATOR . self::FileName;

    $ret = [self::NameKey=>$name, self::VersionKey=>$version
      , self::DependenciesKey=>$dependencies];

    if ($annotationServer) {
      $ret[self::AnnotationServerKey] = $annotationServer;
    }

    return $ret;
  }

  private $dir_;
}