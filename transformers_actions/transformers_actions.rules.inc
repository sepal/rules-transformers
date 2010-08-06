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
    'csv_line' => array(
      'label' => t('CSV line'),
      'wrap' => TRUE,
      'property defaults' => array(
        'getter callback' => 'transformers_actions_metadata_wrapper_csv_getter',
      ),
      // The variable has to provide the correct property info.
      'property info' => array(),
    ),
    'xml' => array(
      'label' => t('XML'),
      'wrap' => TRUE,
    ),
    'xml_document' => array(
      'label' => t('XML'),
      'wrap' => TRUE,
      'parent' => 'xml',
    ),
    'xml_element' => array(
      'label' => t('XML Data'),
      'wrap' => TRUE,
      'parent' => 'xml',
      'property defaults' => array(
        'getter callback' => 'transformers_actions_metadata_wrapper_xml_getter',
      ),
      // The variable has to provide the correct property info.
      'property info' => array(),
    ),
    'metadata' => array(
      'label' => t('Metadata'),
      'ui class' => 'TransformersActionsMetadataUI',
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
        'skip_first_line' => array(
          'type' => 'boolean',
          'label' => t('Skip the first line'),
          'optional' => TRUE,
          'default value' => FALSE,
        ),
        'metadata' => array(
          'type' => 'metadata',
          'restriction' => 'input',
          'label' => t('Metadata for CSV'),
          'description' => t('Describe the columns for CSV'),
          'metatypes' => FALSE,
        ),
      ),
      'provides' => array(
        'csv' => array(
          'type' => 'list<csv_line>',
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
          'type' => 'list<csv_line>',
          'label' => t('CSV data'),
        ),
        'delimiter' => array(
          'type' => 'text',
          'label' => t('Delimiter'),
          'optional' => TRUE,
          'default value' => ',',
        ),
        'skip_first_line' => array(
          'type' => 'boolean',
          'label' => t('Skip the first line'),
          'optional' => TRUE,
          'default value' => FALSE,
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
        'xml_document' => array(
          'type' => 'xml_document',
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
          'label' => t('XML document or element'),
        ),
      ),
      'provides' => array(
        'xml_text' => array(
          'type' => 'text',
          'label' => t('XML text'),
          'description' => t('XML data as text'),
        ),
      ),
      'base' => 'transformers_actions_xml_export',
      'group' => t('Transformers'),
    ),
    'xpath_evalution' => array(
      'label' => t('Evaluate XPath'),
      'parameter' => array(
        'xml_document' => array(
          'type' => 'xml_document',
          'label' => t('XML document'),
        ),
        'xpath' => array(
          'type' => 'text',
          'label' => t('XPath expression'),
        ),
        'metadata' => array(
          'type' => 'metadata',
          'restriction' => 'input',
          'label' => t('Metadata for XML'),
          'description' => t('Describe attributes or nodes, attribute.'),
          'metatypes' => TRUE,
        ),
      ),
      'provides' => array(
        'xml_list' => array(
          'type' => 'list<xml_element>',
          'label' => t('List of xml elements'),
        ),
      ),
      'base' => 'transformers_actions_xpath_evalution',
      'group' => t('Transformers'),
    ),
  );
}

/**
 * Action: Import CSV data from a string.
 */
function transformers_actions_csv_import($csv_text, $delimiter = ',', $skip_first_line = FALSE) {
  $csv = NULL;
  $array = preg_split("/\n/", $csv_text);
  $j = 0;
  foreach ($array as $i => $line) {
    if ((!$skip_first_line) || ($i >0)) {
      if (!empty($line)) {
        $csv[$j] = preg_split("/$delimiter/", $line);
        $j++;
      }
    }
  }
  return array('csv' => $csv);
}

/**
 * Process callback for csv import.
 */
function transformers_actions_csv_import_process(RulesPlugin $element) {
  $property_info = array();
  foreach ($element->settings['metadata'] as $i => $column) {
    if ($column['type'] != '0') {
      $property_info[$column['label']] = array(
        'type' => $column['type'],
        'label' => $column['label'],
      );
    }
  };
  $element_info['provides']['csv'] = array(
    'property defaults' => array(
      'getter callback' => 'transformers_actions_metadata_wrapper_csv_getter',
    ),
  );
  $element_info['provides']['csv']['property info'] = $property_info;
  $element->alterInfo($element_info);
}

/**
 * Action: Export CSV data to a string.
 */
function transformers_actions_csv_export($csv, $delimiter = ',', $skip_first_line = FALSE) {
  $csv_array = array();
  foreach ($csv as $i => $line) {
    if (((!$skip_first_line) || ($i >0))) {
      $csv_array[] = implode($delimiter, $line);
    }
  }
  return array('csv_text' => implode("\n", $csv_array));
}

/**
 * Action: Import XML data from a string.
 */
function transformers_actions_xml_import($xml_text) {
  $doc = new DOMDocument();
  $doc->loadXML($xml_text);
  return array('xml_document' => $doc);
}

/**
 * Action: Export XML data to a string.
 */
function transformers_actions_xml_export($xml) {
  if (get_class($xml) == 'DOMNode') {
    $doc = new DOMDocument();
    $doc->appendChild($xml);
  }
  elseif (get_class($xml) == 'DOMDocument') {
    $doc = $xml;
  }
  else {
    // TODO: throw error.
  }
  $string = $doc->saveXML();
  return array('xml_text' => $string);
}

/**
 * Action: Evaluate XPath.
 */
function transformers_actions_xpath_evalution($xml_document, $xpath) {
  $xpath_obj = new DOMXPath($xml_document);
  $node_list = $xpath_obj->query($xpath);
  $list = transformers_actions_nodelist2array($node_list);

  return array('xml_list' => $list);
}

/**
 * Process callback for xpath_evaluation.
 */
function transformers_actions_xpath_evalution_process(RulesPlugin $element) {
  $property_info = array();
  foreach ($element->settings['metadata'] as $i => $entry) {
    if ($entry['type'] !== '0') {
      $property_info[$entry['label']] = array(
        'type' => $entry['type'],
        'label' => $entry['label'],
        'metatype' => $entry['metatype'],
      );
    }
  };
  $element_info['provides']['xml_list']['property info'] = $property_info;
  $element->alterInfo($element_info);
}
