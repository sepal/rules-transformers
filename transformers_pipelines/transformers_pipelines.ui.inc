<?php
/**
 * @file Transformers Pipelines UI classes
 */

/**
 *
 * Enter description here ...
 * @author sebastian
 *
 */
class TransformersPipelineUI extends RulesActionContainerUI {

  public function form(&$form, &$form_state, $options = array(), $iterator = NULL) {
    // Pass an iterator just iterating over the rules, thus no further child
    // elements will be displayed.
    parent::form($form, $form_state, $options, $this->element->getIterator());
    // Only show the add rule link.
    $form['elements']['#add']['#links'] = array_intersect_key($form['elements']['#add']['#links'], array('add_rule' => 1));
    $form['elements']['#attributes']['class'][] = 'rules-rule-set';
    $form['elements']['#caption'] = t('Rules');
  }
}