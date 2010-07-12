<?php
// $Id$

/**
 * @file Transformers Actions - Rules integration.
 */

/**
 * Implements hook_datainfo().
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

/**
 * Implements hook_rules_action_info().
 */
function transformers_actions_rules_action_info() {
  return array(
    'csv_import' => array(
      'label' => t('CSV import'),
      'parameter' => array(
        'string' => array(
          'type' => 'text',
          'label' => t('CSV string'),
          'description' => t('CSV data as text'),
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
    'csv_export' => array(
      'label' => t('CSV export'),
      'parameter' => array(
        'csv' => array(
          'type' => 'csv',
          'label' => t('CSV data'),
        ),
        'delimiter' => array(
          'type' => 'text',
          'label' => t('Delimiter'),
          'optional' => TRUE,
          'default value' => ',',
        ),
      ),
      'provides' => array(
        'string' => array(
          'type' => 'text',
          'label' => t('CSV string'),
        ),
      ),
      'base' => 'transformers_actions_csv_export',
      'group' => t('Transformers'),
    ),
    'xml_import' => array(
      'label' => t('XML import'),
      'parameter' => array(
        'string' => array(
          'type' => 'text',
          'label' => t('XML string'),
          'description' => t('XML data as text'),
        ),
      ),
      'provides' => array(
        'xml' => array(
          'type' => 'xml',
          'label' => t('XML doc'),
        ),
      ),
      'base' => 'transformers_actions_xml_import',
      'group' => t('Transformers'),
    ),
    'xml_export' => array(
      'label' => t('XML export'),
      'parameter' => array(
        'string' => array(
          'type' => 'xml',
          'label' => t('XML doc'),
        ),
      ),
      'provides' => array(
        'string' => array(
          'type' => 'xml',
          'label' => t('XML data as text'),
        ),
      ),
      'base' => 'transformers_actions_xml_export',
      'group' => t('Transformers'),
    ),
  );
}

/*
 * Saves content to a file and returns the path.
 */
function transformers_actions_content_to_file($data) {
  $file = file_save_data($data);
  return $file->uri;
}

/*
 * Action: Import CSV data from a string.
 */
function transformers_actions_csv_import($string, $delimiter = '') {
  $csv = NULL;
  $uri = transformers_actions_content_to_file($string);
  $iterator = new ParserCSVIterator($uri);
  $parser = new ParserCSV();
  $parser->setDelimiter($delimiter);
  $csv = $parser->parse($iterator);
  return array('csv' => $csv);
}

/**
 * Action: Export CSV data to a string.
 */
function transformers_actions_csv_export($csv, $export = '') {
  $string = '';
  $uri = transformers_actions_content_to_file('');
  $fp = fopen($uri, 'w');
  foreach ($csv as $line) {
    fputcsv($fp, $line);
  }
  fclose($fp);
  $string = file_get_contents($uri);
  return array('string' => $string);
}

/**
 * Action: Import XML data from a string.
 */
function transformers_actions_xml_import($string) {
  $doc = new DOMDocument();
  $doc->loadXML($string);
  return array('xml' => $doc);
}

/**
 * Action: Export XML data to a string.
 */
function transformers_actions_xml_export($doc) {
  $string = $doc->saveXML();
  return array('string' => $string);
}
