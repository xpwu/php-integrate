<?php


namespace Inte;


class NoRunTime implements RunTime {

  public function getPharDir(): string {
    return "";
  }

  public function getPharBaseName(): string {
    return "";
  }

  public function getAllClassesOfSelf(): array {
    return [];
  }
}