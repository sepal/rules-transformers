<?php
/**
 * @file Transformers Pipelines UI classes
 */

/**
 * Transformer action set.
 */
class Transformer extends RulesActionContainer {

  protected $itemName = 'transformer';
  public $positionInfo = array();
  public $splitter = array();

  public function action($action) {
    return parent::action($action);
  }

  public function __sleep() {
    return parent::__sleep() + array('splitter' => 'splitter', 'positionInfo' => 'positionInfo');
  }

  public function addSplitter(){
    $id = uniqid();
    $this->splitter[$id] = array();
    $this->splitter[$id]['id'] = $id;
    $this->splitter[$id]['configured'] = FALSE;
    return $this->splitter[$id];
  }

  protected function exportChildren($key = 'Action') {
    return parent::exportChildren($key);
  }

  public function getSelectorChild($provider, $level = 0) {
    $variables = $this->call('stateVariables', array(TRUE));
    $result = array();
    try {
      $wrappers = rules_wrap_data($variables, $variables[$provider], TRUE);
      foreach ($wrappers as $name => $data) {
        $result[$name] = $name;
      }
    }
    catch (Exception $e) {
      drupal_set_message($e->getMessage(), 'error');
    }
    return $result;
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

    if (isset($options['init'])) {
      return;
    }

    drupal_add_library('transformers_pipelines', 'jsplumb');
    drupal_add_js(drupal_get_path('module', 'transformers_pipelines') . '/transformers_pipelines.js');
    drupal_add_css(drupal_get_path('module', 'transformers_pipelines') . '/transformers_pipelines.css');

    // Since the original $form['elements'] we need to savethe add action links.
    $add_links = array_intersect_key($form['elements']['#add']['#links'], array('add_action' => 1));

    // Recurse over all element childrens or use the provided iterator.
    $iterator = isset($iterator) ? $iterator : new RecursiveIteratorIterator($this->element, RecursiveIteratorIterator::SELF_FIRST);
    $root_depth = $this->element->depth();
    $form['elements'] = array();
    $js_vars = array();
    $js_vars['transformers_pipelines']['rule'] = $this->element->name;

    $form['elements']['variables'] = array(
      '#prefix' => '<div id="transformers_variables"><ul class="transformers_variables">',
      '#suffix' => '</ul></div>',
    );

    foreach ($this->element->availableVariables() as $key => $child) {
      $form['elements']['variables'][$key] = array(
        '#prefix' => '<li class="transformers_variables"><div class="transformers_variables" id="transformers_variables_' . $key . '">',
        '#markup' => $child['label'],
        '#suffix' => '</div></li>',
      );
      $js_vars['transformers_pipelines']['variables'][$key]['source'] = $key;
      $js_vars['transformers_pipelines']['variables'][$key]['scope'] = $child['type'];
    }

    $form['elements']['panel'] = array(
      '#prefix' => '<div id="transformers_panel">',
      '#suffix' => '</div>',
    );
    foreach ($iterator as $key => $child) {
      $id = $child->elementId();

      $element_id = 'transformers_element_' . $id;

      $style = '';
      if (isset($this->element->positionInfo[$element_id])) {
        $info = $this->element->positionInfo[$element_id];

        $style = 'position:absolute;top:' . $info['position']['top'] . 'px;left:' . $info['position']['left'] . 'px;';
      }

      $form['elements']['panel']['container'][$id] = array(
        '#prefix' => '<div style="' . $style . '" class="transformers_action" id ="' . $element_id . '">',
        '#suffix' => '</div>',
      );

      $form['elements']['panel']['container'][$id]['head'] = array(
        '#prefix' => '<div class="transformers_action_head">',
        '#suffix' => '</div>',
      );

      $form['elements']['panel']['container'][$id]['head']['label'] = array(
        '#type' => 'link',
        '#title' => $child->label(),
        '#href' => RulesPluginUI::path($child->root()->name, 'edit', $child),
        '#prefix' => '<span class="transformers_label">',
        '#suffix' => '</span>',
      );

      $form['elements']['panel']['container'][$id]['head']['icons'] = array(
        '#prefix' => '<span class="transformers_icons">',
        '#suffix' => '</span>',
      );

      $form['elements']['panel']['container'][$id]['head']['icons']['delete'] = array(
        '#type' => 'link',
        '#title' => 'X',
        '#href' => RulesPluginUI::path($child->root()->name, 'delete', $child),
      );

      $form['elements']['panel']['container'][$id]['anchors'] = array(
        '#prefix' => '<div>',
        '#suffix' => '</div>',
      );
      $form['elements']['panel']['container'][$id]['anchors']['in'] = array(
        '#prefix' => '<ul class="transformers_inputs">',
        '#suffix' => '</ul>',
      );

      $form['elements']['panel']['container'][$id]['anchors']['out'] = array(
        '#prefix' => '<ul class="transformers_outputs">',
        '#suffix' => '</ul>',
      );

      foreach ($child->pluginParameterInfo() as $key => $param) {
        $input_id = $element_id . '_' . $key;
        $form['elements']['panel']['container'][$id]['anchors']['in'][$key] = array(
          '#prefix' => '<li class="transformers_anchor transformers_inputs"><div id="' . $input_id . '">',
          '#markup' => $param['label'],
          '#suffix' => '</div></li>',
        );
        $js_vars['transformers_pipelines']['elements'][$id]['parameter'][$key]['target'] = $key;
        $js_vars['transformers_pipelines']['elements'][$id]['parameter'][$key]['scope'] = $param['type'];

        $settings = $child->settings;
        if (array_key_exists($key . ':select', $settings)) {
          $js_vars['transformers_pipelines']['elements'][$id]['parameter'][$key]['source'] = $settings[$key . ':select'];
        }
      }

      $variables = $child->pluginProvidesVariables();
      foreach ($child->providesVariables() as $key => $var) {
        $info = $variables[$var['source name']];
        $output_id =  $element_id . '_' . $key;
        $form['elements']['panel']['container'][$id]['anchors']['out'][$key] = array(
          '#prefix' => '<li class="transformers_anchor transformers_outputs"><div  id="' . $output_id . '">',
          '#markup' => $info['label'],
          '#suffix' => '</div></li>',
        );
        $js_vars['transformers_pipelines']['elements'][$id]['provides'][$key]['source'] = $key;
        $js_vars['transformers_pipelines']['elements'][$id]['provides'][$key]['scope'] = $param['type'];
      }
      $js_vars['transformers_pipelines']['elements'][$id]['element_id'] = $id;    $selector = 'annotated_csv_line';
    }

    foreach ($this->element->splitter as $key => $splitter) {
      $this->generateSplitter($form['elements']['panel']['container'][$key], $splitter, $element_id);

      $js_vars['transformers_pipelines']['splitter'][$key]['parameter'][$key]['target'] = 'any';
      $js_vars['transformers_pipelines']['splitter'][$key]['element_id'] = $key;
      if (!empty($splitter['configured'])) {
        $js_vars['transformers_pipelines']['splitter'][$key]['parameter'][$key]['source'] = $splitter['provider'];
        foreach ($this->element->getSelectorChild($splitter['provider']) as $name) {
          $js_vars['transformers_pipelines']['splitter'][$key]['provides'][$name]['source'] = $splitter['provider'] . ':' . $name;
        }
      }
    }

    drupal_add_js($js_vars, 'setting');
    $add_links['add_action']['href'] = str_replace("workflow/rules", "workflow/transformers", $add_links['add_action']['href']);

    $form['elements']['links'] = array(
      '#prefix' => '<div id="transformers_add_links"><ul class="action-links">',
      '#suffix' => '</ul></div>',
    );
    $form['elements']['links']['add_action'] = array(
      '#prefix' => '<li class="">',
      '#type' => 'link',
      '#href' => $add_links['add_action']['href'],
      '#title' => $add_links['add_action']['title'],
      '#suffix' => '</li>',
    );
    $form['elements']['links']['add_splitter'] = array(
      '#prefix' => '<li class="">',
      '#type' => 'link',
      '#href' => 'admin/config/workflow/transformers/config/' . $this->element->name . '/splitter/add',
      '#title' => t('Add splitter'),
      '#suffix' => '</li>',
    );
  }

  public function generateSplitter(&$form, $splitter, $element_id) {
    $html_id = 'transformers_element_' . $splitter['id'];

      $style = '';
      if (isset($this->element->positionInfo[$html_id])) {
        $info = $this->element->positionInfo[$html_id];

        $style = 'position:absolute;top:' . $info['position']['top'] . 'px;left:' . $info['position']['left'] . 'px;';
      }
    $form = array(
      '#prefix' => '<div style="' . $style . '" class="transformers_action" class="transformers_action" id ="' . $html_id . '">',
      '#suffix' => '</div>',
    );

    $form['head'] = array(
      '#prefix' => '<div class="transformers_action_head">',
      '#suffix' => '</div>',
    );

    $form['head']['label'] = array(
      '#markup' => 'Splitter',
      '#prefix' => '<span class="transformers_label">',
      '#suffix' => '</span>',
    );

    $form['head']['icons'] = array(
      '#prefix' => '<span class="transformers_icons">',
      '#suffix' => '</span>',
    );

    $form['head']['icons']['delete'] = array(
      '#type' => 'link',
      '#title' => 'X',
      '#href' => 'admin/config/workflow/transformers/config/' . $this->element->name . '/splitter/delete',
    );

    $form['anchors'] = array(
      '#prefix' => '<div>',
      '#suffix' => '</div>',
    );
    $form['anchors']['in'] = array(
      '#prefix' => '<ul class="transformers_inputs">',
      '#suffix' => '</ul>',
    );

    $form['anchors']['out'] = array(
      '#prefix' => '<ul class="transformers_outputs">',
      '#suffix' => '</ul>',
    );

    if (empty($splitter['configured'])) {
      $html_id_any = $html_id . '_any';
      $form['anchors']['in']['any'] = array(
        '#prefix' => '<li class="transformers_anchor transformers_inputs"><div id="' . $html_id_any . '">',
        '#markup' => t('Any'),
        '#suffix' => '</div></li>',
      );
    }
    else {
      $html_id_any = $html_id . '_any';
      $form['anchors']['in']['any'] = array(
        '#prefix' => '<li class="transformers_anchor transformers_inputs"><div id="' . $html_id_any . '">',
        '#markup' => $splitter['provider'],
        '#suffix' => '</div></li>',
      );
      foreach ($this->element->getSelectorChild($splitter['provider']) as $name) {
        $html_id_name = $html_id . '_' . $splitter['provider'] . '-' . $name;
        $form['anchors']['out'][$name] = array(
          '#prefix' => '<li class="transformers_anchor transformers_outputs"><div  id="' . $html_id_name . '">',
          '#markup' => ':' . $name,
          '#suffix' => '</div></li>',
        );
      }
    }
  }
}
