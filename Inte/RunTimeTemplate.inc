<?php


namespace Inte;


class RunTimeTemplate {

  private static $isInteSelf = false;

  static public function getContent():string {

    $content = <<<'EOF'
<?php


namespace Inte;


class RunTime {

  public function getPharDir():string {
    return $this->pharDir_;
  }

  public function getPharBaseName():string {
    return $this->pharName_;
  }

  public function getAllClassFileMapOfSelf(): array {
    return $this->allClasses_;
  }

  public function setPharDir(string $dir) {
    $this->pharDir_ = $dir;
  }

  public function setPharName(string $name) {
    $this->pharName_ = $name;
  }

  public function setAllClassFileMapOfSelf(array $map) {
    $this->allClasses_ = $map;
  }

  static public function default(): RunTime {
    if (self::$instance_ === null) {
      self::$instance_ = new RunTime();
    }
    return self::$instance_;
  }

  private $pharDir_;
  private $pharName_;
  private $allClasses_;

  private static $instance_ = null;
}

EOF;

    return $content;
  }

  static public function getRunTimeFileName():string {
    if (self::$isInteSelf) {
      return "__RunTime__.inc";
    }

    return Project::getInstance()->getBuildOutputDir()
      . DIRECTORY_SEPARATOR . "RunTime.inc";
  }

  static public function inteSelf():void {
    self::$isInteSelf = true;
  }

//  static public function getRunTimeFileNameForInteSelf():string {
//    return "__RunTime__.inc";
//  }
}