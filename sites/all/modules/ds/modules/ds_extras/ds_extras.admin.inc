<?php

/**
 * @file
 * Display Suite Extras administrative functions.
 */

/**
 * Menu callback: Display Suite extras settings.
 */
function ds_extras_settings($form) {

  $form['ft'] = array(
    '#type' => 'fieldset',
    '#title' => t('Field templates'),
  );

  $form['ft']['ds_extras_field_template'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Toggle this checkbox to enable the "Field templates" functionality. With this feature, you can change properties per field: classes, label and wrappers.'),
    '#default_value' => variable_get('ds_extras_field_template', FALSE),
  );

  $form['ft']['ds_extras_field_template_default'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default field template'),
    '#states' => array(
      'invisible' => array(
        'input[name="ds_extras_field_template"]' => array('checked' => FALSE),
      ),
    ),
  );

  $theme_functions = module_invoke_all('ds_field_theme_functions_info');
  unset($theme_functions['theme_ds_field_expert']);
  $form['ft']['ds_extras_field_template_default']['ft-default'] = array(
    '#type' => 'select',
    '#options' => $theme_functions,
    '#default_value' => variable_get('ft-default', 'theme_field'),
    '#description' => t('Select the default field template for every field. You can override this per field on the "Manage display" screens.<br /><em>Default</em> will output the field as defined in Drupal Core, <em>Reset</em> will strip all wrappers. There is also an expert field template, which you can only select per field, to have full control over all wrappers.<br />On the <a href="!url">styles</a> form, you can configure extra styles which can be applied to the outer wrapper (they will be added to the <em>$classes</em> variable), except if you are using the reset or expert field template.<br />You can create your own custom field templates which need to be defined with hook_ds_field_theme_functions_info(). See ds.api.php for an example.<br />And last, but not least, you can now change the label of every field too. Note that, the classes for inline or above labels are not prefixed with <em>field-</em> in any field template except default.', array('!url' => url('admin/structure/ds/styles'))),
  );

  $form['switch'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node switch view modes'),
  );

  $form['switch']['ds_extras_switch_view_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Toggle this checkbox to enable the "Switch view mode" functionality. With this feature, you can change the view mode on a per node basis. A new tab on the node edit form will appear called "Display settings."'),
    '#default_value' => variable_get('ds_extras_switch_view_mode', FALSE),
  );

  $form['hide_page_title'] = array(
    '#type' => 'fieldset',
    '#title' => t('Hide page title'),
  );

  $form['hide_page_title']['ds_extras_hide_page_title'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Toggle this checkbox to enable the "Hide page title" functionality. With this feature, you can select on the "Manage display" screens if the page title must be hidden or not. <br /><strong>Important: this will only work on the full page of a node.</strong>'),
    '#default_value' => variable_get('ds_extras_hide_page_title', FALSE),
  );

  if (module_exists('block')) {
    $form['region'] = array(
      '#type' => 'fieldset',
      '#title' => t('Region to block'),
    );

    $form['region']['ds_extras_region_to_block'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable'),
      '#description' => t('Toggle this checkbox to enable the "Region to block" functionality. With this feature, you can create additional regions on the "Manage display" screens which will be exposed as blocks. <br /><strong>Important: this will not work on the default view mode</strong>.'),
      '#default_value' => variable_get('ds_extras_region_to_block', FALSE),
    );
  }

  if (module_exists('fivestar')) {
    $form['fivestar'] = array(
      '#type' => 'fieldset',
      '#title' => t('Fivestar'),
    );

    $form['fivestar']['ds_extras_fivestar'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable'),
      '#description' => t('Toggle this checkbox to enable the "Fivestar" functionality. A field called "Fivestar" will be made available on the Manage Display screens.'),
      '#default_value' => variable_get('ds_extras_fivestar', FALSE),
    );
  }

  if (module_exists('views')) {
    $form['views'] = array(
      '#type' => 'fieldset',
      '#title' => t('Views displays'),
    );

    $form['views']['ds_extras_vd'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable'),
      '#description' => t('Toggle this checkbox to enable the "Views displays" functionality. With this feature, you can use Display Suite to rearrange the template variables of any views display. A new menu item called "Views displays" will be available at !url.', array('!url' => l(url('admin/structure/ds'), 'admin/structure/ds', array('absolute' => TRUE)))),
      '#default_value' => variable_get('ds_extras_vd', FALSE),
    );
  }

  $form = system_settings_form($form);
  $form['#submit'][] = 'ds_extras_settings_submit';
  return $form;
}

/**
 * Submit callback: Extras settings screen.
 */
function ds_extras_settings_submit($form, &$form_state) {
  cache_clear_all('ds_fields', 'cache');
  cache_clear_all('entity_info:', 'cache', TRUE);
  cache_clear_all('theme_registry:', 'cache', TRUE);
  cache_clear_all('module_implements', 'cache_bootstrap');
  menu_rebuild();
}

/**
 * Alter Manage display screen.
 */
function ds_extras_field_ui_alter(&$form, &$form_state) {

  // Field template.
  if (variable_get('ds_extras_field_template', FALSE) && isset($form['#ds_layout'])) {

    $field_settings = $form['#field_settings'];
    $field_styles = _ds_styles('ds_styles_fields');
    $functions = module_invoke_all('ds_field_theme_functions_info');
    $default_field_function = variable_get('ft-default', 'theme_field');

    $i = 1;
    foreach (element_children($form['fields']) as $key) {


      // Check if the label is overridden - add the original too.
      $form['fields'][$key]['human_name']['#prefix'] = '<div class="field-label-row"><input type="hidden" class="original-label" value="' . $form['fields'][$key]['human_name']['#markup'] . '">';
      $form['fields'][$key]['human_name']['#suffix'] = '</div>';
      if (isset($field_settings[$key]['ft']['lb'])) {
        $form['fields'][$key]['human_name']['#markup'] = t('@label (Original: @original)', array('@label'=>$field_settings[$key]['ft']['lb'], '@original' => $form['fields'][$key]['human_name']['#markup']));
      }

      // Link to open settings.
      $form['fields'][$key]['format']['ft'] = array(
        '#prefix' => '<a href="" class="ft-link">' . t('Field display') . '</a><div class="field-template" id="ft-' . $i . '">',
        '#suffix' => '</div>',
      );
      $i++;

      $field_function = isset($field_settings[$key]['ft']['func']) ? $field_settings[$key]['ft']['func'] : $default_field_function;

      // Functions.
      $form['fields'][$key]['format']['ft']['function'] = array(
        '#title' => t('Choose a field template'),
        '#type' => 'select',
        '#options' => $functions,
        '#default_value' => $field_function,
        '#attributes' => array(
          'class' => array('ds-extras-field-template'),
        ),
      );

      // Field styles.
      if (!empty($field_styles)) {
        $field_styles_select = array(
          '#type' => 'select',
          '#multiple' => TRUE,
          '#options' => $field_styles,
          '#title' => t('Choose a field style'),
          '#default_value' => isset($field_settings[$key]['ft']['styles']) ? explode(' ', $field_settings[$key]['ft']['styles']) : array(),
          '#prefix' => '<div class="field-styles">',
          '#suffix' => '</div>',
        );
        $form['fields'][$key]['format']['ft']['styles'] = $field_styles_select;
      }
      else {
        $form['fields'][$key]['format']['ft']['styles'] = array(
          '#type' => 'value',
          '#value' => array(''),
        );
      }

      // Wrappers and label.
      $wrappers = array(
        'lb' => array(
          'title' => t('Label'),
            'states' => array(
            'invisible' => array(
              'select[name="fields[' . $key . '][label]"]' => array('value' => 'hidden'),
              'select[name="fields[' . $key . '][format][ft][function]"]' => array('value' => 'theme_ds_field_expert'),
            ),
          ),
        ),
        'ow' => array('title' => t('Outer wrapper')),
        'fis' => array('title' => t('Field items')),
        'fi' => array('title' => t('Field item')),
      );
      foreach ($wrappers as $wrapper_key => $value) {
        $form['fields'][$key]['format']['ft'][$wrapper_key] = array(
          '#type' => 'checkbox',
          '#title' => $value['title'],
          '#prefix' => '<div class="ft-group ' . $wrapper_key . '">',
          '#default_value' => isset($field_settings[$key]['ft'][$wrapper_key]) ? TRUE : FALSE,
          '#description' => t('Toggle to enable'),
        );
        if (isset($value['states'])) {
          $form['fields'][$key]['format']['ft'][$wrapper_key]['#states'] = $value['states'];
        }
        $form['fields'][$key]['format']['ft'][$wrapper_key . '-el'] = array(
          '#type' => 'textfield',
          '#title' => t('Element'),
          '#size' => '10',
          '#description' => t('E.g. div, span, h2 etc.'),
          '#default_value' => isset($field_settings[$key]['ft'][$wrapper_key . '-el']) ? $field_settings[$key]['ft'][$wrapper_key . '-el'] : '',
        );
        if (isset($value['states']) && $wrapper_key == 'lb') {
          $form['fields'][$key]['format']['ft'][$wrapper_key . '-el']['#states'] = $value['states'];
        }
        $classes = array(
          'field-name-' . strtr($key, '_', '-'),
        );
        $form['fields'][$key]['format']['ft'][$wrapper_key . '-cl'] = array(
          '#type' => 'textfield',
          '#title' => t('Classes'),
          '#size' => '10',
          '#suffix' => '</div><div class="clearfix"></div>',
          '#default_value' => isset($field_settings[$key]['ft'][$wrapper_key . '-cl']) ? $field_settings[$key]['ft'][$wrapper_key . '-cl'] : '',
          '#description' => t('Classes: !classes', array('!classes' => implode(', ', $classes))),
        );
        if (isset($value['states'])) {
          $form['fields'][$key]['format']['ft'][$wrapper_key . '-cl']['#states'] = $value['states'];
        }
      }

      $form['fields'][$key]['format']['ft']['update'] = array(
        '#type' => 'button',
        '#value' => t('Update'),
        '#attributes' => array(
          'class' => array('ft-update'),
        ),
      );

      // Another label needs some other stuff.
      unset($form['fields'][$key]['format']['ft']['lb']['#description']);
      $form['fields'][$key]['format']['ft']['lb']['#states'] = array(
        'invisible' => array(
          'select[name="fields[' . $key . '][label]"]' => array('value' => 'hidden'),
        )
      );
      $form['fields'][$key]['format']['ft']['lb']['#type'] = 'textfield';
      $form['fields'][$key]['format']['ft']['lb']['#size'] = '10';
      $form['fields'][$key]['format']['ft']['lb']['#attributes'] = array('class' => array('label-change'));
      $form['fields'][$key]['format']['ft']['lb']['#default_value'] = isset($field_settings[$key]['ft']['lb']) ? $field_settings[$key]['ft']['lb'] : '';
    }
  }

  // Views displays.
  if (variable_get('ds_extras_vd', FALSE)) {
    // Add an additional submit callback.
    $form['#submit'] = array_merge(array('ds_extras_vd_field_ui_submit'), $form['#submit']);
  }

  // Hide page title functionality, currently only works on node.
  if (isset($form['#ds_layout']) && $form['#entity_type'] == 'node' && ($form['#view_mode'] == 'full' || variable_get('ds_extras_switch_view_mode', FALSE)) &&  variable_get('ds_extras_hide_page_title', FALSE)) {
    $form['additional_settings']['ds_layouts']['ds_hide_page_title'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide page title'),
      '#default_value' => isset($form['#ds_layout']->settings['hide_page_title']) ? $form['#ds_layout']->settings['hide_page_title'] : FALSE,
      '#weight' => 100,
    );
  }

  // Region to block only fires if there is a layout and we're working on the
  // a view mode which is not equal to default.
  if (isset($form['#ds_layout']) && $form['#view_mode'] != 'default' && variable_get('ds_extras_region_to_block', FALSE)) {

    $layout = $form['#ds_layout'];

    // Get the entity_type, bundle and view mode.
    $entity_type = $form['#entity_type'];
    $bundle = $form['#bundle'];
    $view_mode = $form['#view_mode'];

    $region_blocks_options = array();
    $region_blocks = variable_get('ds_extras_region_blocks', array());
    foreach ($region_blocks as $key => $block) {
      if ($block['info'] == "{$entity_type}_{$bundle}_{$view_mode}") {
        $region_blocks_options[$key] = t('Remove') . ' ' . $block['title'];
      }
    }

    $form['additional_settings']['region_to_block'] = array(
      '#type' => 'fieldset',
      '#title' => t('Block regions'),
      '#description' => t('Create additional regions in this layout which will be exposed as blocks.')
    );

    $form['additional_settings']['region_to_block']['new_block_region'] = array(
      '#type' => 'textfield',
      '#title' => t('Region name'),
      '#description' => t('Enter a name to create a new region.'),
    );
    $form['additional_settings']['region_to_block']['new_block_region_key'] = array(
      '#title' => t('Machine name'),
      '#type' => 'machine_name',
      '#default_value' => '',
      '#maxlength' => 32,
      '#required' => FALSE,
      '#description' => t('The machine-readable name of this block region. This name must contain only lowercase letters and underscores. This name must be unique.'),
      '#disabled' => FALSE,
      '#machine_name' => array(
        'exists' => 'ds_extras_region_to_block_unique',
        'source' => array('additional_settings', 'region_to_block', 'new_block_region'),
      ),
    );

    if (!empty($region_blocks_options)) {
      $form['additional_settings']['region_to_block']['remove_block_region'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Existing block regions'),
        '#options' => $region_blocks_options,
        '#description' => t('Toggle regions you want to remove. Make sure no fields are attached in that region anymore or you will not see them anymore.'),
      );
    }

    $form['#submit'][] = 'ds_extras_block_submit';
  }
}

/**
 * Implements hook_ds_layout_region_alter().
 */
function ds_extras_ds_layout_region_alter($context, &$region_info) {

  $region_blocks = variable_get('ds_extras_region_blocks', array());
  // Bail out if region_blocks is empty or we are working on default view mode.
  if (empty($region_blocks) || $context['view_mode'] == 'default') {
    return;
  }

  $entity_type = $context['entity_type'];
  $bundle = $context['bundle'];
  $view_mode = $context['view_mode'];

  foreach ($region_blocks as $block_key => $block) {

    if ($block['info'] == "{$entity_type}_{$bundle}_{$view_mode}") {
      $region_info['region_options'][$block_key] = $block['title'];
      $region_info['table_regions'][$block_key] = array(
        'title' => check_plain($block['title']),
        'message' => t('No fields are displayed in this region'),
      );
    }
  }
}

/**
 * Implements hook_ds_field_settings_alter().
 */
function ds_extras_ds_field_settings_alter(&$field_settings, $form, &$form_state) {

  $fields = $form_state['values']['fields'];
  $default_field_function = variable_get('ft-default', 'theme_field');
  $wrappers = array(
    'ow' => t('Wrapper'),
    'fis' => t('Field items'),
    'fi' => t('Field item')
  );

  foreach ($fields as $key => $field) {

    // Make sure we need to save anything for this field.
    if (_ds_field_valid($key, $field, $form_state)) {
      continue;
    }

    // Get the values.
    $values = $fields[$key]['format']['ft'];

    // Theme output function.
    $function = $values['function'];
    if ($function != $default_field_function) {
      $field_settings[$key]['ft']['func'] = $function;
    }

    // Field styles.
    if ($function != 'theme_ds_field_expert' && $function != 'theme_ds_field_reset') {
      $classes = implode(' ', $values['styles']);
      if (!empty($classes)) {
        $field_settings[$key]['ft']['styles'] = $classes;
      }
    }

    // Label.
    if (isset($fields[$key]['label']) && $fields[$key]['label'] != 'hidden') {
      if (!empty($values['lb'])) {
        $field_settings[$key]['ft']['lb'] = $values['lb'];
      }
      if (!(empty($values['lb-el'])) && $function == 'theme_ds_field_expert') {
        $field_settings[$key]['ft']['lb-el'] = check_plain($values['lb-el']);
      }
      if (!(empty($values['lb-cl'])) && $function == 'theme_ds_field_expert') {
        $field_settings[$key]['ft']['lb-cl'] = check_plain($values['lb-cl']);
      }
    }

    // Custom field configuration.
    if ($function == 'theme_ds_field_expert') {
      foreach ($wrappers as $wrapper_key => $title) {
        if ($values[$wrapper_key]) {
          // Enable.
          $field_settings[$key]['ft'][$wrapper_key] = TRUE;
          // Element.
          $field_settings[$key]['ft'][$wrapper_key . '-el'] = !(empty($values[$wrapper_key . '-el'])) ? check_plain($values[$wrapper_key . '-el']) : 'div';
          // Classess.
          $field_settings[$key]['ft'][$wrapper_key . '-cl'] = !(empty($values[$wrapper_key . '-cl'])) ? check_plain($values[$wrapper_key . '-cl']) : '';
        }
      }
    }
  }
}

/**
 * Submit callback after Field UI submission of a views display.
 */
function ds_extras_vd_field_ui_submit($form, &$form_state) {
  // Add the 'type' key to the extra title key so we can ditch the notice.
  $form_state['values']['fields']['title']['type'] = 'hidden';
}

/**
 * Submit callback: manage block regions.
 */
function ds_extras_block_submit($form, &$form_state) {

  // Create new region.
  if (!empty($form_state['values']['additional_settings']['region_to_block']['new_block_region'])) {

    // Get the entity_type, bundle and view mode.
    $entity_type = $form['#entity_type'];
    $bundle = $form['#bundle'];
    $view_mode = $form['#view_mode'];

    $block = array(
      'title' => $form_state['values']['additional_settings']['region_to_block']['new_block_region'],
      'info' => "{$entity_type}_{$bundle}_{$view_mode}",
    );

    $block_key = $form_state['values']['additional_settings']['region_to_block']['new_block_region_key'];
    $region_blocks = variable_get('ds_extras_region_blocks', array());
    $region_blocks[$block_key] = $block;
    variable_set('ds_extras_region_blocks', $region_blocks);
  }

  // Remove a region.
  if (isset($form_state['values']['additional_settings']['region_to_block']['remove_block_region'])) {
    $variable_set = FALSE;
    $region_blocks = variable_get('ds_extras_region_blocks', array());
    $remove = $form_state['values']['additional_settings']['region_to_block']['remove_block_region'];
    foreach ($remove as $key => $value) {
      if ($key === $value) {
        $variable_set = TRUE;
        db_delete('block')
          ->condition('delta', $key)
          ->condition('module', 'ds_extras')
          ->execute();
        unset($region_blocks[$key]);
      }
    }

    if ($variable_set) {
      variable_set('ds_extras_region_blocks', $region_blocks);
    }
  }
}

/**
 * Return unique region to block.
 */
function ds_extras_region_to_block_unique($name) {
  $region_blocks = variable_get('ds_extras_region_blocks', array());
  $value = strtr($name, array('-' => '_'));
  return isset($region_blocks[$value]) ? TRUE : FALSE;
}
