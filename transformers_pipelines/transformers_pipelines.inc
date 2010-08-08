<?php
/**
 * @file Transformers Pipelines UI classes
 */

/**
 * Transformer action set.
 */
class Transformer extends RulesActionContainer {

  protected $itemName = 'transformer';

  public function action($action) {
    return parent::action($action);
  }

  protected function exportChildren($key = 'Action') {
    return parent::exportChildren($key);
  }
}

/**
 *
 * UI class for Transformer.
 * @author sebastian
 *
 */
class TransformersPipelineUI extends RulesActionContainerUI {

  public function form(&$form, &$form_state, $options = array(), $iterator = NULL) {
    parent::form($form, $form_state, $options, $this->element->getIterator());

    drupal_add_library('transformers_pipelines', 'jsplumb');
    drupal_add_js(drupal_get_path('module', 'transformers_pipelines') . '/transformers_pipelines.js');
    drupal_add_css(drupal_get_path('module', 'transformers_pipelines') . '/transformers_pipelines.css');

    // Since the original $form['elements'] we need to savethe add action links.
    $add_links = array_intersect_key($form['elements']['#add']['#links'], array('add_action' => 1));

    // Recurse over all element childrens or use the provided iterator.
    $iterator = isset($iterator) ? $iterator : new RecursiveIteratorIterator($this->element, RecursiveIteratorIterator::SELF_FIRST);
    $root_depth = $this->element->depth();
    $form['elements'] = array(
      '#prefix' => '<div id="transformers_panel">',
      '#suffix' => '</div>',
    );

    $js_vars = array();
    foreach ($iterator as $key => $child) {
      $id = $child->elementId();

      $element_id = 'transformers_element_' . $id;
      $form['elements'][$id] = array(
        '#prefix' => '<div class="transformers_action" id ="' . $element_id . '">',
        '#suffix' => '</div>',
      );

      $form['elements'][$id]['head'] = array(
        '#prefix' => '<div class="transformers_action_head">',
        '#suffix' => '</div>',
      );

      $form['elements'][$id]['head']['label'] = array(
        '#type' => 'link',
        '#title' => $child->label(),
        '#href' => RulesPluginUI::path($child->root()->name, 'edit', $child),
        '#prefix' => '<span class="transformers_label">',
        '#suffix' => '</span>',
      );

      $form['elements'][$id]['head']['icons'] = array(
        '#prefix' => '<span class="transformers_icons">',
        '#suffix' => '</span>',
      );

      $form['elements'][$id]['head']['icons']['delete'] = array(
        '#type' => 'link',
        '#title' => 'X',
        '#href' => RulesPluginUI::path($child->root()->name, 'delete', $child),
      );

      $form['elements'][$id]['anchors'] = array(
        '#prefix' => '<div>',
        '#suffix' => '</div>',
      );
      $form['elements'][$id]['anchors']['in'] = array(
        '#prefix' => '<ul class="transformers_inputs">',
        '#suffix' => '</ul>',
      );

      $form['elements'][$id]['anchors']['out'] = array(
        '#prefix' => '<ul class="transformers_outputs">',
        '#suffix' => '</ul>',
      );

      foreach ($child->parameterInfo() as $key => $param) {
        $input_id = 'transformers_' . $key;
        $form['elements'][$id]['anchors']['in'][$key] = array(
          '#prefix' => '<li class="transformers_anchor transformers_inputs"><div id="' . $input_id . '">',
          '#markup' => $param['label'],
          '#suffix' => '</div></li>',
        );
        $js_vars['transformers_pipelines'][$id]['parameter'][$key]['target'] = $input_id;
        $js_vars['transformers_pipelines'][$id]['parameter'][$key]['scope'] = $param['type'];

        $settings = $child->settings;
        if (array_key_exists($key . ':select', $settings)) {
          $js_vars['transformers_pipelines'][$id]['parameter'][$key]['source'] = 'transformers_' . $settings[$key . ':select'];
        }
      }

      foreach ($child->providesVariables() as $key => $var) {
        $output_id =  'transformers_' . $key;
        $form['elements'][$id]['anchors']['out'][$key] = array(
          '#prefix' => '<li class="transformers_anchor transformers_outputs"><div  id="' . $output_id . '">',
          '#markup' => $var['label'],
          '#suffix' => '</div></li>',
        );
        $js_vars['transformers_pipelines'][$id]['provides'][$key]['source'] = $output_id;
        $js_vars['transformers_pipelines'][$id]['provides'][$key]['scope'] = $param['type'];
      }

      $js_vars['transformers_pipelines'][$id]['element_id'] = $element_id;
    }


    drupal_add_js($js_vars, 'setting');

    $form['links'] = array(
      '#prefix' => '<div id="transformers_add_links">',
      '#markup' => l(t($add_links['add_action']['title']), $add_links['add_action']['href']),
      '#suffix' => '</div>',
    );
  }
}
