<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 15/11/9
 * Time: 上午1:27
 */

require_once ("Log.inc");
require_once ("Utils.inc");

class MkFileReader {

  function __construct($relativeDir, $srcTopDir, $mode) {
    // $DES_TOP_DIR
    $this->topDir_ = $srcTopDir;
    $this->dir_ = $relativeDir;
    $this->mode_ = $mode;
  }

  /**
   * @return bool
   */
  public function read(){
    return $this->readDir($this->dir_);
  }

  /**
   * @return array [fullPath=>relativePath]
   */
  public function devFiles(){return $this->devFiles_;}
  public function proFiles(){return $this->proFiles_;}

  /**
   * @return array  devFiles() or proFiles() depende on the mode
   */
  public function allFiles() {
    if ($this->mode_ == Utils::DEV_MODE) {
      return $this->devFiles_;
    } else if ($this->mode_ == Utils::PRO_MODE) {
      return $this->proFiles_;
    }
    return [];
  }
  public function allNotCkFiles() {
    if ($this->mode_ == Utils::DEV_MODE) {
      return $this->notCkDevFiles;
    } else if ($this->mode_ == Utils::PRO_MODE) {
      return $this->notCkProFiles;
    }
    return [];
  }

  private function readDir($curRelativedir) {
    if (!is_dir($this->topDir_.'/'.$curRelativedir)) {
      Log::error($this->topDir_.'/'.$curRelativedir. "not exist!");
      return false;
    }
    $mk = $this->topDir_.'/'.$curRelativedir.'/'.'integrate.mk.hphp';
    Log::debug($mk);
    if (!is_file($mk)) {
      Log::warning($mk." not exist.");
      return true;
    }

    if (!$this->checkMk($mk)) return false;
    $common_NOT_CHECK_FILES = [];

    require ($mk);

    if (!$this->readFiles($curRelativedir, $common_FILES)) {
      return false;
    }
    if (!$this->readNotCkFiles($curRelativedir, $common_NOT_CHECK_FILES)) {
      return false;
    }
    if (($this->mode_ & Utils::DEV_MODE) != 0) {
      if (!$this->readFiles($curRelativedir, $development_FILES)) {
        return false;
      }
    }
    if (($this->mode_ & Utils::PRO_MODE) != 0) {
      if (!$this->readFiles($curRelativedir, $production_FILES)) {
        return false;
      }
    }
    if (isset($web_INDEX)) {
      if (count(Utils::$webIndex_)) {
        Log::error("web_INDEX = $web_INDEX is duplicated, the first set is ". Utils::$webIndex_[0]);
        return false;
      } else {
        Utils::$webIndex_[$this->topDir_.'/'.$curRelativedir.'/'.$web_INDEX]
          = $curRelativedir.'/'.$web_INDEX;
      }
    }
    if (isset($test_INDEX)) {
      if (count(Utils::$webIndex_)) {
        Log::error("test_INDEX = $test_INDEX is duplicated, the first set is ". Utils::$webIndex_[0]);
        return false;
      } else {
        Utils::$webIndex_[$this->topDir_.'/'.$curRelativedir.'/'.$test_INDEX]
          = $curRelativedir.'/'.$test_INDEX;
      }
    }
    if (isset($classLoader)) {
      foreach ($classLoader as $key => $value) {
        $classLoader[$key] = $curRelativedir.'/'.$value;
      }

      $intersect = array_intersect_key(Utils::$classLoader, $classLoader);
      if (count($intersect) === 0) {
        Utils::$classLoader = array_merge(Utils::$classLoader, $classLoader);
      } else {
        $inKey = array_keys($intersect);
        Log::error("duplicated $inKey[0]---".Utils::$classLoader[$inKey[0]]
          ." AND ".$classLoader[$inKey[0]]);
        return false;
      }
    }
    if (isset($require_once_FILES)) {
      foreach ($require_once_FILES as $key => $value) {
        $require_once_FILES[$key] = $curRelativedir.'/'.$value;
      }

      $intersect = array_intersect_key(Utils::$requireOnceFiles, $require_once_FILES);
      if (count($intersect) === 0) {
        Utils::$requireOnceFiles = array_merge(Utils::$requireOnceFiles, $require_once_FILES);
      } else {
        $inKey = array_keys($intersect);
        Log::error("duplicated $inKey[0]---".Utils::$requireOnceFiles[$inKey[0]]
          ." AND ".$require_once_FILES[$inKey[0]]);
        return false;
      }
    }


    if (isset($sub_DIRS)) {
      foreach ($sub_DIRS as $subdir) {
        $res = $this->readDir($curRelativedir.'/'.$subdir);
        if (!$res) {
          return false;
        }
      }
    }

    clearstatcache();
    return true;
  }

  private function readFiles($curRelativedir, &$files) {
    Log::debug_dump($files);
    $pwd = getcwd();
    chdir("$this->topDir_/$curRelativedir");
    foreach ($files as $src=>$des) {
      $srckey = $src;
      if (is_int($src)) {
        $src = $des;
      }
      if (strpos($src, '*') !== false) {
        Log::debug("strpos(src, '*')");
        $fls = glob($src);
        Log::debug_dump($files);
        unset($files[$srckey]);
        $files = array_merge($files, $fls);
        Log::debug_dump($files);
        continue;
      }
    }
    chdir($pwd);

    Log::debug_dump($files);
    foreach ($files as $src => $des) {
      if (is_int($src)) {
        $src = $des;
      }
      if (strpos($src, '*') === true) {
        continue;
      }
      $src = $this->topDir_.'/'.$curRelativedir.'/'.$src;
      if (!is_file($src) && !is_link($src)) {
        Log::error($src . " not exist.");
        return false;
      }
      if (is_link($src)) {
        $src = readlink($src);
        if (substr($src, 0, 1) !== '/') {
          $src = $this->topDir_.'/'.$curRelativedir.'/'.$src;
        }
      }
      if (($this->mode_ & Utils::DEV_MODE) != 0) {
        $res = array_search($curRelativedir.'/'.$des, $this->devFiles_);
        if ($res !== false) {
          Log::error("$res and $src have the same target file, which is $des");
          return false;
        }
        $this->devFiles_[$src] = $curRelativedir.'/'.$des;
      }
      if (($this->mode_ & Utils::PRO_MODE) != 0) {
        $res = array_search($curRelativedir.'/'.$des, $this->proFiles_);
        if ($res !== false) {
          Log::error("$res and $src have the same target file, which is $des");
          return false;
        }
        $this->proFiles_[$src] = $curRelativedir.'/'.$des;
      }
    }
    return true;
  }

  private function readNotCkFiles($curRelativedir, &$files) {
    Log::debug_dump($files);
    $pwd = getcwd();
    chdir("$this->topDir_/$curRelativedir");
    foreach ($files as $src=>$des) {
      $srckey = $src;
      if (is_int($src)) {
        $src = $des;
      }
      if (strpos($src, '*') !== false) {
        Log::debug("strpos(src, '*')");
        $fls = glob($src);
        Log::debug_dump($files);
        unset($files[$srckey]);
        $files = array_merge($files, $fls);
        Log::debug_dump($files);
        continue;
      }
    }
    chdir($pwd);

    Log::debug_dump($files);
    foreach ($files as $src => $des) {
      if (is_int($src)) {
        $src = $des;
      }
      if (strpos($src, '*') === true) {
        continue;
      }
      $src = $this->topDir_.'/'.$curRelativedir.'/'.$src;
      if (!is_file($src) && !is_link($src)) {
        Log::error($src . " not exist.");
        return false;
      }
      if (is_link($src)) {
        $src = readlink($src);
        if (substr($src, 0, 1) !== '/') {
          $src = $this->topDir_.'/'.$curRelativedir.'/'.$src;
        }
      }
      if (($this->mode_ & Utils::DEV_MODE) != 0) {
        $res = array_search($curRelativedir.'/'.$des, $this->devFiles_);
        if ($res !== false) {
          Log::error("$res and $src have the same target file, which is $des");
          return false;
        }
        $this->notCkDevFiles[$src] = $curRelativedir.'/'.$des;
      }
      if (($this->mode_ & Utils::PRO_MODE) != 0) {
        $res = array_search($curRelativedir.'/'.$des, $this->proFiles_);
        if ($res !== false) {
          Log::error("$res and $src have the same target file, which is $des");
          return false;
        }
        $this->notCkProFiles[$src] = $curRelativedir.'/'.$des;
      }
    }
    return true;
  }

  private function checkMk($mk) {
    $before = get_defined_vars();
    require ($mk);
    $after = get_defined_vars();
    $before["before"] = '';
    $before['common_FILES'] = '';
    $before['development_FILES'] = '';
    $before['production_FILES'] = '';
    $before['sub_DIRS'] = '';
    $before['web_INDEX'] = '';
    $before['classLoader'] = '';
    $before['test_INDEX'] = '';
    $before['common_NOT_CHECK_FILES'] = '';
    $before['require_once_FILES'] = '';
    $diff = array_diff_key($after, $before);
    if (is_array($diff)) {
      foreach ($diff as $val) {
        log::error("$mk: \$$val is not used. should only define vars: \$common_FILES \$development_FILES \$production_FILES \$sub_DIRS");
        return false;
      }
    }

    if (isset($common_FILES) && !is_array($common_FILES)) {
      Log::error("\$common_FILES should be a array");
      return false;
    }
    if (isset($development_FILES) && !is_array($development_FILES)) {
      Log::error("\$development_FILES should be a array");
      return false;
    }
    if (isset($production_FILES) && !is_array($production_FILES)) {
      Log::error("\$production_FILES should be a array");
      return false;
    }
    if (isset($sub_DIRS) && !is_array($sub_DIRS)) {
      Log::error("\$sub_DIRS should be a array");
      return false;
    }
    if (isset($web_INDEX) && is_array($web_INDEX)) {
      Log::error("\$web_INDEX should NOT be a array");
      return false;
    }
    if (isset($classLoader) && !is_array($classLoader)) {
      Log::error("\$classLoader should be a array");
      return false;
    }
    return true;
  }

  private $mode_;
  private $dir_;
  private $topDir_;
  private $devFiles_ = [];
  private $proFiles_ = [];
  private $notCkDevFiles = [];
  private $notCkProFiles = [];
}