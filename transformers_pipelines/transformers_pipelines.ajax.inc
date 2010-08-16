<?php
// $Id$

/**
 * @file: Transformers Pipelines - Ajax callbacks
 */

/**
 * Connect to elements.
 */
function transformers_pipeline_ajax_connect($rules_config) {
  $connection = $_POST['transformers_target_connection'];
  $output['result'] = FALSE;

  if (isset($connection) && isset($rules_config)) {
    $element_target = rules_element_load($connection['target_element_id'], $rules_config->name);
    $element_source = rules_element_load($connection['source_element_id'], $rules_config->name);

    if ($element_source->weight > $element_target->weight) {
      $element_target->weight = $element_source->weight+1;
    }
    $element_target->settings[$connection['target'] . ':select'] = $connection['source'];

    $element_target->save();
    $output['result'] = TRUE;
  }

  drupal_json_output($output);
}

/**
 * Disconnect to elements.
 */
function transformers_pipelines_ajax_disconnect($rules_config) {
  $connection = $_POST['transformers_target_connection'];
  $output['result'] = FALSE;
  if (isset($connection) && isset($rules_config)) {
    $element_target = rules_element_load($connection['target_element_id'], $rules_config->name);
    unset($element_target->settings[$connection['target'] . ':select']);
    $element_target->save();
    $output['result'] = TRUE;
  }

  drupal_json_output($output);
}

/**
 * Connect an element to a splitter.
 */
function transformers_pipeline_splitter_connect($rules_config) {
  $connection = (isset($_POST['transformers_target_connection']) ? $_POST['transformers_target_connection'] : NULL);
  $output['result'] = FALSE;

  if (isset($connection) && isset($rules_config)) {
    $id = $connection['target_element_id'];
    if (empty($rules_config->splitter[$id]['configured'])) {
      $rules_config->splitter[$id]['configured'] = TRUE;
      $rules_config->splitter[$id]['provider'] = $connection['source'];
      $rules_config->splitter[$id]['element'] = $connection['source_element_id'];
      $rules_config->splitter[$id]['target']['selector'] = $connection['target'];
      $rules_config->splitter[$id]['target']['element'] = $connection['target_element_id'];
      $rules_config->save();
    }
    $output['result'] = TRUE;
  }
  drupal_json_output($output);
}

/**
 * Disconnect an element from a splitter
 */
function transformers_pipeline_splitter_disconnect($rules_config) {
  $connection = (isset($_POST['transformers_target_connection']) ? $_POST['transformers_target_connection'] : NULL);
  $output['result'] = FALSE;
  $id = $connection['target_element_id'];
  if ($rules_config->splitter[$id]['configured']) {
    $rules_config->splitter[$id]['configured'] = FALSE;
    unset($rules_config->splitter[$id]['provider']);
    unset($rules_config->splitter[$id]['target']);
    unset($rules_config->splitter[$id]['source']);
    $rules_config->save();
    $output['result'] = TRUE;
  }
  drupal_json_output($output);
}

/**
 * Callback to save to position of an action.
 */
function transformers_pipeline_ajax_save_position($rule_config) {
  $info = (isset($_POST['position_info']) ? $_POST['position_info'] : NULL);
  $output['result'] = TRUE;
  $rule_config->positionInfo[$info['id']] = $info;
  $rule_config->save();
  drupal_json_output($output);
}

/**
 * Callback that saves the height of the panel.
 */
function transformers_pipeline_ajax_panel_height($rule_config) {
  $height = (isset($_POST['panel_height']) ? $_POST['panel_height'] : NULL);
  $output['result'] = TRUE;
  $rule_config->panelHeight = $height;
  $rule_config->save();
  drupal_json_output($output);
}
