<?php
/**
 * Created by PhpStorm.
 * User: xpwu
 * Date: 2019/3/23
 * Time: 12:27 PM
 */

namespace Inte;
use Inte\Dependence\Content;
use Inte\Dependence\Position;
use Inte\Error\NormalError;

/**
 *
 * 路径信息：
 * 返回的所有文件或者路径都是相对于Project->rootDir的路径 或者 相对于系统include_path中的某一路径
 *
 */

class Dependence {

  public function __construct(Position $position, Content $content) {
    $this->position_ = $position;
    $this->content_ = $content;
  }

  /**
   * @return array  [className=>file]
   */
  public function getClasses():array {
    return $this->content_->getClasses();
  }
  public function getRequireFiles():array {
    return $this->content_->getRequireFiles();
  }

  /**
   * @return Dependence[]
   */
  public function getSubDependencies():array {
    return $this->content_->getSubDependencies($this->getRepo());
  }

  public function getAllFiles():array {
    return $this->position_->getAllNeedFilesForInte($this->content_);
  }

  public function getTopDir():string {
    return $this->position_->getTopDir();
  }

  public function getName():string {
    return $this->position_->getName();
  }
  public function getVersion():string {
    return $this->position_->getVersion();
  }
  public function getRepo():string {
    return $this->position_->getRepo();
  }

  /**
   * @var Content
   */
  private $content_;

  /**
   * @var Position
   */
  private $position_;
}