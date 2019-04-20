<?php


namespace ProcessorDemo;


use Inte\Annotation\AbstractAnnotationServer;
use Inte\Annotation\Config;
use Inte\Annotation\Logger;
use Inte\Annotation\PhpFiler;

class DemoProcessor extends AbstractAnnotationServer {

  /**
   * @var Logger
   */
  private $logger_;

  /**
   * @var PhpFiler
   */
  private $filer_;

  private $isBuild_;

  /**
   * @param string[] $classes
   */
  function process(array $classes): void {
    if (!$this->isBuild_) {
      return;
    }
    foreach ($classes as $class) {
      $this->logger_->info($class);
      $cgi = new \ReflectionClass($class);

      if ($cgi->isSubclassOf('\Demo\CGI')) {
        $this->logger_->info("find subclass of \Demo\CGI <--- " . $class);

        $writer = $this->filer_->createSourceFile($cgi->getShortName()."Impl.inc");
        $content = <<<EOF
<?php

class {$cgi->getShortName()}Impl {
}
EOF;

        $writer->write($content);
      }
    }

    $this->logger_->info("test processor dependency: " . \ClassC::class);
  }

  function init(Logger $logger, PhpFiler $filer
    , Config $config, string $projectRootDir, bool $isBuild): void {

    $this->logger_ = $logger;
    $this->filer_ = $filer;
    $this->isBuild_ = $isBuild;
  }
}