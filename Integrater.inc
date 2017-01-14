<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 15/11/9
 * Time: 上午1:51
 */

require_once ("Log.inc");
require_once ("Utils.inc");
require_once ("AutoChecker.inc");
require_once ("MkFileReader.inc");


class Integrater {
  function __construct($mode, $topDir) {
    $this->mode_ = $mode;
    $this->topDir_ = $topDir;
  }

  public function integrate() {
    $mk = new MkFileReader('.', $this->topDir_, $this->mode_);
    Log::debug_dump($mk);
    if (!$mk->read()) {
      return false;
    }

    $autock = new AutoChecker($mk);
    Log::debug_dump($autock);
    if (!$autock->check()) {
      return false;
    }
    Log::debug_dump($autock);

    $this->allfiles_ = array_merge($mk->allFiles(), $mk->allNotCkFiles());

    $filename = '';
    $content = $autock->outputAutoload($filename);
    $this->autoload_[$filename] = $content;

    return true;
  }

  public function outputTo($integrateDir){

    $desTopDir = realpath($this->topDir_).'/'.$integrateDir;

    if (!self::checkDestinationDirAndMk($this->allfiles_, $desTopDir)
      || !self::copyFileTo($this->allfiles_, $desTopDir)
      || !self::copyFileTo(Utils::$index, $desTopDir)) {
      return false;
    }

    // create 'AutoLoad.inc' file
    $filename = array_keys($this->autoload_)[0];
    file_put_contents($desTopDir.'/'.$filename, $this->autoload_[$filename]);
    return true;
  }

  public function phar($pharPath) {
    $pharName = basename($pharPath);
    if (Phar::canWrite()) {
      $ph = new Phar("$pharPath.phar", 0, $pharName);
      $ph->startBuffering();
      foreach ($this->allfiles_ as $key=>$value) {
        if (substr($value, 0, 2) === './') {
          $value = substr($value, 2);
        }
        $ph[$value] = file_get_contents($key);
      }
      $filename = array_keys($this->autoload_)[0];
      $ph['AutoLoad.inc'] = $this->autoload_[$filename];
      if (count(Utils::$index) != 0) {
        $fl = array_keys(Utils::$index)[0];
        $ph['webIndex.php'] = file_get_contents($fl);
      } else {
        $ph['webIndex.php'] = "<?php\n";
      }


      $requireOnceFiles = '';
      foreach (Utils::$requireOnceFiles as $file) {
        $rf = "phar://$pharName.phar/".$file;
        $requireOnceFiles .= "\n  require_once(\"$rf\");";
      }

      $requireOnceExcludeFiles = '';
      foreach (Utils::$requireOnceExcludeFiles as $requireOnceExcludeFile) {
        $requireOnceExcludeFiles .= "\n  require_once(\"$requireOnceExcludeFile\");";
      }

      $ph->stopBuffering();
      $stub = <<<EOF
<?php
  Phar::interceptFileFuncs();
  Phar::mapPhar();
  set_include_path('phar://' . __FILE__ . PATH_SEPARATOR . get_include_path());
  require_once("phar://$pharName.phar/AutoLoad.inc");
  inte_AutoLoader::run();
  inte_AutoLoader::\$prefix = "phar://$pharName.phar";
  $requireOnceFiles
  $requireOnceExcludeFiles
  require_once('phar://' . __FILE__ . "/webIndex.php");
  __HALT_COMPILER(); ?>
EOF;

      $ph->setStub($stub);
    } else {
      echo "phar can not write!\n";
      return false;
    }
    return true;
  }

//  public function pharForTest($pharPath) {
//    $pharPath = $pharPath . '_test';
//    $pharName = basename($pharPath);
//    if (Phar::canWrite()) {
//      $ph = new Phar("$pharPath.phar"
//        , FilesystemIterator::CURRENT_AS_FILEINFO | FilesystemIterator::KEY_AS_FILENAME
//        , $pharName);
//      $ph->startBuffering();
//      foreach ($this->allfiles_ as $key=>$value) {
//        if (substr($value, 0, 2) === './') {
//          $value = substr($value, 2);
//        }
//        $ph[$value] = file_get_contents($key);
//      }
//      $filename = array_keys($this->autoload_)[0];
//      $ph['AutoLoad.inc'] = $this->autoload_[$filename];
//      $fl = array_keys(Utils::$webIndex_)[0];
//      if (count(Utils::$testIndex_) != 0) {
//        $ph['testIndex.php'] = file_get_contents($fl);
//      } else {
//        $ph['testIndex.php'] = '<?php ';
//      }
//
//      $requireOnceFiles = '';
//      foreach (Utils::$requireOnceFiles as $file) {
//        $rf = "phar://$pharName.phar/".$file;
//        $requireOnceFiles .= "\n  require_once(\"$rf\");";
//      }
//
//      $ph->stopBuffering();
//      $stub = <<<EOF
//<?php
//  date_default_timezone_set('PRC');
//  Phar::interceptFileFuncs();
//  Phar::mapPhar();
//  set_include_path('phar://' . __FILE__ . PATH_SEPARATOR . get_include_path());
//  require_once("phar://$pharName.phar/AutoLoad.inc");
//  zxg_AutoLoader::run();
//  zxg_AutoLoader::\$prefix = "phar://$pharName.phar";
//   $requireOnceFiles
//  require_once("phar://$pharName.phar/testIndex.php");
//  __HALT_COMPILER(); ? >
//EOF;
//
//      $ph->setStub($stub);
//    } else {
//      echo "phar can not write!\n";
//      return false;
//    }
//    return true;
//  }

  static private function checkDestinationDirAndMk(array $files, $desTopDir) {
    if (file_exists($desTopDir)) {
      Log::error("$desTopDir has existed");
      return false;
    }
    // mk des top dir
    if (!mkdir($desTopDir)) {
      Log::error("mkdir $desTopDir failed!");
      return false;
    }
    Log::debug("\$desTopDir = ".$desTopDir);
    foreach ($files as $src => $des) {
      $desdir = $desTopDir.'/'.$des;
      if (0!==substr_compare($desdir, $desTopDir, 0, strlen($desTopDir))) {
        Log::error("the destination dir $desdir is not in the top dir $desTopDir");
        return false;
      }
    }
    return true;
  }

  static private function copyFileTo(array $files, $desTopDir) {
    Log::debug("\$desTopDir = $desTopDir");

    foreach ($files as $src => $des) {
      Log::debug("\$des = $desTopDir/$des");
      if (!file_exists(dirname($desTopDir.'/'.$des))) {
        Log::debug(dirname($desTopDir.'/'.$des));
        mkdir(dirname($desTopDir.'/'.$des), 0777, true);
      }
      copy($src, $desTopDir.'/'.$des);
    }
    return true;
  }

  private $mode_;
  private $topDir_;
  private $allfiles_;
  private $autoload_ = [];
}

