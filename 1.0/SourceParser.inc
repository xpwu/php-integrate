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
  function __construct(array $files) {
    $this->files_ = $files;
  }

  public function parse() {

    if ($this->parsed_) {
      return;
    }
    $this->parsed_ = true;

    $shm_id = ftok(__FILE__,'SourceParser');
    $shareMemory = shm_attach($shm_id);
    define('MayBeFailedIndex', 1);

    $nowFiles = [];

    do {
      $loadedFiles = [];

      for ($i = 0; $i < count($nowFiles); $i++) {

        $pid = pcntl_fork();
        switch ($pid) {
          case -1:
            throw new NormalError("pcntl_fork error!");
          case 0 : {
            error_reporting(0);
            shm_put_var($shareMemory,MayBeFailedIndex, $i);
            // 一次子进程尽可能多的测试
            for (;$i < count($nowFiles); $i++) {
              include_once $nowFiles[$i];
              shm_put_var($shareMemory,MayBeFailedIndex, $i+1);
            }
            exit(0);
          }
        }

        pcntl_waitpid($pid, $status);
//        if (!pcntl_wifexited($status) || pcntl_wexitstatus($status)) {
//          continue;
//        }

        $j = $i;
        $i = shm_get_var($shareMemory,MayBeFailedIndex);
        for (;$j < $i; $j++) {
          $this->checkAndLoadAFile($nowFiles[$j]);
          $loadedFiles[] = $nowFiles[$j];
        }

      }

      $nowFiles = array_diff($nowFiles, $loadedFiles);

      // 如果成功加载至少一个文件  则再次循环 测试是否有还可以加载的文件
    }while(count($loadedFiles) != 0);

    $this->noParseFile_ = $nowFiles;
    shm_detach($shareMemory);
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

    foreach ($diffClass as $className) {
      $this->result_[$className] = $file;
    }
    foreach ($diffInterface as $inter) {
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

  }

  private $files_ = [];
  private $result_ = [];
  private $noParseFile_ = [];

  private $parsed_ = false;
}