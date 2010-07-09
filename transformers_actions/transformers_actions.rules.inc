<?php
// $Id$

/**
 * @file Transformers Actions - Rules integration.
 */

function transformers_actions_data_info() {
  return array(
    'csv' => array(
      'label' => t('CSV'),
      wrap => TRUE
    ),
    'xml' => array(
      'label' => t('XML'),
      wrap => TRUE
    ),
  );
}

function transformers_actions_rules_action_info() {
  return array(
    'csv_import' => array(
      'label' => t('CSV transformer'),
      'parameter' => array(
        'data' => array(
          'type' => 'text',
          'label' => t('Path'),
          'description' => t('CSV data as text.'),
        ),
        'delimiter' => array(
          'type' => 'text',
          'label' => t('Delimiter'),
          'optional' => TRUE,
          'default value' => ',',
        ),
      ),
      'provides' => array(
        'csv' => array(
          'type' => 'csv',
          'label' => t('CSV data'),
        ),
      ),
      'base' => 'transformers_actions_csv_import',
      'group' => t('Transformers'),
    ),
  );
}

/*
 * Saves content to file and returns the path.
 */
function transformers_actions_content_to_file($data) {
  $file = file_save_data($data);
  return $file->uri;
}

/*
 * Action: Import CSV data from a file
 */
function transformers_actions_csv_import($data, $delimiter = '') {
  dpm($delimiter);
  $csv = NULL;
  if (!empty($path) || !empty($delimiter)) {
    $uri = transformers_actions_content_to_file($data);
    $iterator = new ParserCSVIterator($uri);
    $parser = new ParserCSV();
    $parser->setDelimiter($delimiter);
    $csv = $parser->parse($iterator);
  }
  return array('csv' => $csv);
}
