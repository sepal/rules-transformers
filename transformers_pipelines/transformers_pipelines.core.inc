<?php
/**
 * @file Transformers Pipelines UI classes
 */

/**
 * Transformer
 */
class Transformer extends RulesActionContainer {

  protected $itemName = 'Transformer';

  public function action($action) {
    return parent::action($action);
  }

  protected function exportChildren($key = 'Action') {
    return parent::exportChildren($key);
  }
}