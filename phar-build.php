<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 15/11/10
 * Time: 下午10:59
 */


$files = [
  'AutoChecker.inc'
  , 'Integrater.inc'
  , 'Log.inc'
  , 'MkFileReader.inc'
  , 'Utils.inc'
  , 'Main.php'
];

if (Phar::canWrite()) {
  $ph = new Phar("inte.phar");
  $ph->startBuffering();
  foreach ($files as $value) {
    $ph[$value] = file_get_contents($value);
  }
  $ph->stopBuffering();
  $ph->setDefaultStub("Main.php");
} else {
  echo "can not write!\n";
}

