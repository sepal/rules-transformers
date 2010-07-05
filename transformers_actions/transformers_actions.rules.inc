<?php
// $Id$

/**
 * @file Transformers Actions - Rules integration.
 */

function transformers_actions_data_info() {
  $return = array(
    'csv' => array(
      'label' => t('CSV'),
      'parent' => 'text',
      'ui class' => 'RulesDataUIText',
    ),
    'xml' => array(
      'label' => t('XML'),
      'parent' => 'text',
      'ui class' => 'RulesDataUIText',
    ),
  );
}

function transformers_actions_rules_action_info() {
  return array(
    'csv_transformer' => array(
      'label' => t('CSV transformer'),
      'parameter' => array(
        'data' => array(
          'type' => 'text',
          'restriction' => 'input',
          'label' => t('CVS string'),
          'description' => t('The data that should be transformed'),
        ),
      ),
      'base' => 'transformers_actions_csv_import',
      'group' => t('Transformers'),
    ),
  );
}

function transformers_actions_csv_import($data, $settings, $state, $element) {

}

