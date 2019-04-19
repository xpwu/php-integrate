<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 8:37 PM
 */

namespace Inte;


use Inte\Error\NormalError;

class SourceParser {

  private const indexCache = "__index_cache__";

  function __construct(array $files, bool $firstInte = false) {
    $this->files_ = $files;
    $this->firstInte_ = $firstInte;
  }

  private function cacheIndex(int $i) {
    // firstInte
    if ($this->firstInte_) {
      file_put_contents(sys_get_temp_dir()
        . DIRECTORY_SEPARATOR . self::indexCache, $i);
      return;
    }
    file_put_contents(Project::getInstance()->getBuildOutputDir()
      . DIRECTORY_SEPARATOR . self::indexCache, $i);
  }

  private function getIndexFromCache():int {
    // firstInte
    if ($this->firstInte_) {
      return (int)file_get_contents(sys_get_temp_dir()
        . DIRECTORY_SEPARATOR . self::indexCache);
    }
    return (int)file_get_contents(Project::getInstance()->getBuildOutputDir()
      . DIRECTORY_SEPARATOR . self::indexCache);
  }

  public function parse() {

    if ($this->parsed_) {
      return;
    }
    $this->parsed_ = true;

//    // firstInte 时 不是在Phar中 此时也不应有RunTimeEnv类的存在
//    $pathName = __FILE__;
//    // 在 phar 包中，ftok不能拿到文件自己，使用运行时的phar name 代替
//    if (class_exists(RunTimeEnv::class)) {
//      $pathName = RunTimeEnv::getInstance()->getRunTime()->getPharName();
//    }
//    $shm_id = ftok($pathName,'m');
//    $shareMemory = shm_attach($shm_id);
//    if (!defined('MayBeFailedIndex')) {
//      define('MayBeFailedIndex', 1);
//    }

    $nowFiles = $this->files_;

    do {
      $loadedFiles = [];

      for ($i = 0; $i < count($nowFiles); $i++) {

        $pid = pcntl_fork();
        switch ($pid) {
          case -1:
            throw new NormalError("pcntl_fork error!");
          case 0 : {
            error_reporting(0);
//            shm_put_var($shareMemory,MayBeFailedIndex, $i);
            $this->cacheIndex($i);
            // 一次子进程尽可能多的测试
            for (;$i < count($nowFiles); $i++) {
              include_once $nowFiles[$i];
//              shm_put_var($shareMemory,MayBeFailedIndex, $i+1);
              $this->cacheIndex($i+1);
            }
            exit(0);
          }
        }

        pcntl_waitpid($pid, $status);
//        if (!pcntl_wifexited($status) || pcntl_wexitstatus($status)) {
//          continue;
//        }

        $j = $i;
//        $i = shm_get_var($shareMemory,MayBeFailedIndex);
        $i = $this->getIndexFromCache();
        for (;$j < $i; $j++) {
          $this->checkAndLoadAFile($nowFiles[$j]);
          $loadedFiles[] = $nowFiles[$j];
        }

      }

      $nowFiles = array_values(array_diff($nowFiles, $loadedFiles));
      $this->allLoadedFiles_ = array_merge($this->allLoadedFiles_, $loadedFiles);

      // 如果成功加载至少一个文件  则再次循环 测试是否有还可以加载的文件
    }while(count($loadedFiles) != 0);

    $this->noParseFile_ = $nowFiles;
//    shm_detach($shareMemory);
  }

  private function checkAndLoadAFile(string $file) {
    $beforeConst = get_defined_constants();
    $beforeClass = get_declared_classes();
    $beforeInterface = get_declared_interfaces();
    $beforeVar = get_defined_vars();
    $beforeVar["beforeVar"] = $beforeVar;

    include_once $file;

    $afterVar = get_defined_vars();
    $afterConst = get_defined_constants();
    $afterClass = get_declared_classes();
    $afterInterface = get_declared_interfaces();

    $diffVar = array_diff_key($afterVar, $beforeVar);
    $diffClass = array_diff($afterClass, $beforeClass);
    $diffConst = array_diff($afterConst, $beforeConst);
    $diffInterface = array_diff($afterInterface, $beforeInterface);

    if (count($diffVar) != 0) {
      throw new NormalError($file ." can NOT define global var!---"
        . implode(',', array_keys($diffVar)));
    }
    if (count($diffConst) != 0) {
      throw new NormalError($file. " can NOT define global const!---"
        . implode(',', array_keys($diffConst)));
    }

    $selfClasses = RunTimeEnv::getInstance()
      ->getRunTime()->getAllClassesOfSelf();

    /**
     * 在集成依赖集成工具提供的lib的工程时，可能在解析一个工程的文件后，lib中的类
     * 才加载进去，此时求差后，类名数组中存在lib中的类名，但是文件名却是工程中的文件，
     * 类与文件不对应，需要排除掉。
     */

    foreach ($diffClass as $className) {
      // 集成工具去集成其他工程时，要去掉集成工具自己的类
      if (array_key_exists($className, $selfClasses)) {
        continue;
      }
      $this->result_[$className] = $file;
    }
    foreach ($diffInterface as $inter) {
      // 集成工具去集成其他工程时，要去掉集成工具自己的类
      if (array_key_exists($inter, $selfClasses)) {
        continue;
      }
      $this->result_[$inter] = $file;
    }

  }

  /**
   * @return array [className => file]
   */
  public function getResult():array {
    return $this->result_;
  }

  public function getNotParseFile():array {
    return $this->noParseFile_;
  }

  public function getParsedFileOrderByParsingSequence():array {
    return $this->allLoadedFiles_;
  }

  private $files_ = [];
  private $result_ = [];
  private $noParseFile_ = [];

  private $allLoadedFiles_ = [];

  private $parsed_ = false;

  private $firstInte_ = false;
}