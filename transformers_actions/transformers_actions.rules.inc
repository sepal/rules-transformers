<?php
// $Id$

/**
 * @file Transformers Actions - Rules integration.
 */

/**
 * Implements hook_rules_data_info().
 */
function transformers_actions_rules_data_info() {
  return array(
    'csv' => array(
      'label' => t('CSV'),
      'wrap' => TRUE,
    ),
    'xml' => array(
      'label' => t('XML'),
      'wrap' => TRUE,
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
        'csv_text' => array(
          'type' => 'text',
          'label' => t('CSV text'),
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
        'csv_text' => array(
          'type' => 'text',
          'label' => t('CSV text'),
          'description' => t('CSV data as text'),
        ),
      ),
      'base' => 'transformers_actions_csv_export',
      'group' => t('Transformers'),
    ),
    'xml_import' => array(
      'label' => t('XML import'),
      'parameter' => array(
        'xml_text' => array(
          'type' => 'text',
          'label' => t('XML text'),
          'description' => t('XML data as text'),
        ),
      ),
      'provides' => array(
        'xml' => array(
          'type' => 'xml',
          'label' => t('XML document'),
        ),
      ),
      'base' => 'transformers_actions_xml_import',
      'group' => t('Transformers'),
    ),
    'xml_export' => array(
      'label' => t('XML export'),
      'parameter' => array(
        'xml' => array(
          'type' => 'xml',
          'label' => t('XML document'),
        ),
      ),
      'provides' => array(
        'xml_text' => array(
          'type' => 'xml',
          'label' => t('XML text'),
          'description' => t('XML data as text'),
        ),
      ),
      'base' => 'transformers_actions_xml_export',
      'group' => t('Transformers'),
    ),
  );
}

/*
 * Action: Import CSV data from a string.
 */
function transformers_actions_csv_import($csv_text, $delimiter = ',') {
  $csv = NULL;
  if (!empty($csv_text)) {
    $array = preg_split("/\n/", $csv_text);
    foreach ($array as $i => $line) {
      if (!empty($line)) {
        $csv[$i] = preg_split("/$delimiter/", $line);
      }
    }
  }
  return array('csv' => $csv);
}

/**
 * Action: Export CSV data to a string.
 */
function transformers_actions_csv_export($csv, $delimiter = ',') {
  $csv_array = array();
  foreach ($csv as $line) {
    $csv_array[] = implode($delimiter, $line);
  }
  return array('csv_text' => implode("\n", $csv_array));
}

/**
 * Action: Import XML data from a string.
 */
function transformers_actions_xml_import($xml_text) {
  $doc = new DOMDocument();
  $doc->loadXML($xml_text);
  return array('xml' => $doc);
}

/**
 * Action: Export XML data to a string.
 */
function transformers_actions_xml_export($xml) {
  $string = $xml->saveXML();
  return array('xml_text' => $string);
}
