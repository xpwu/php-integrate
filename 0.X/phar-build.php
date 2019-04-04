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
  // pinte
  $ph = new Phar("bin/pinte_exe.phar");
  $ph->startBuffering();
  foreach ($files as $value) {
    $ph[$value] = file_get_contents($value);
  }
  $ph->stopBuffering();
  $stub = <<<EOF
#!/usr/bin/env php
<?php
  Phar::interceptFileFuncs();
  Phar::mapPhar();
  set_include_path('phar://' . __FILE__ . PATH_SEPARATOR . get_include_path());
  require_once('phar://' . __FILE__ ."/Main.php");
  __HALT_COMPILER(); ?>
EOF;
  $ph->setStub($stub);
  rename("bin/pinte_exe.phar", 'bin/phpinte');
  chmod('bin/phpinte', 0777);

  // pinte.phar
  $ph = new Phar("bin/pinte.phar");
  $ph->startBuffering();
  foreach ($files as $value) {
    $ph[$value] = file_get_contents($value);
  }
  $ph->stopBuffering();
  $ph->setDefaultStub("Main.php");
} else {
  echo "can not write!\n";
}

