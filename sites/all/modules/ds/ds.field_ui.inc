<?php

/**
 * @file
 * Field UI functions for Display Suite.
 */

/**
 * Adds the Display Suite fields and layouts to the form.
 */
function ds_field_ui_fields_layouts(&$form, &$form_state) {

  // Get the entity_type, bundle and view mode.
  $entity_type = $form['#entity_type'];
  $bundle = $form['#bundle'];
  $view_mode = $form['#view_mode'];

  $form['#export_id'] = $entity_type . '|' . $bundle . '|' . $view_mode;

  // Add layout fieldset.
  _ds_field_ui_table_layouts($entity_type, $bundle, $view_mode, $form);

  // Add the fields.
  _ds_field_ui_fields($entity_type, $bundle, $view_mode, $form, $form_state);

  // Attach js.
  $form['#attached']['js'][] = drupal_get_path('module', 'ds') . '/js/ds.js';

  // Attach css.
  $form['#attached']['css'][] = drupal_get_path('module', 'ds') . '/css/ds.admin.css';

  // Add process function to add the regions.
  $form['#process'][] = 'ds_field_ui_regions';

  // Alter the text of the custom display settings.
  if (isset($form['modes'])) {
    $form['modes']['view_modes_custom']['#description'] = t('<a href="!url">Manage view modes</a>', array('!url' => url('admin/structure/ds/view_modes')));
    $form['additional_settings']['modes'] = $form['modes'];
    $form['additional_settings']['modes']['#weight'] = -10;
    unset($form['modes']);
  }
  else {
    $form['additional_settings']['modes']['view_modes_custom']['#description'] = t('<a href="!url">Manage view modes</a>', array('!url' => url('admin/structure/ds/view_modes')));
  }
}

/**
 * Menu callback: Revert layout and field settings form.
 */
function ds_revert_layout_field_settings_form(&$form, &$form_state, $id = '') {
  $layout = new stdClass();
  ctools_include('export');
  $ds_layout_settings = ctools_export_crud_load_all('ds_layout_settings');
  if (isset($ds_layout_settings[$id])) {
    $layout = $ds_layout_settings[$id];
  }

  if (isset($layout) && $layout->export_type == 3) {
    $form['#layout'] = $layout;
    return confirm_form($form,
      t('Are you sure you want to revert the layout for %layout?', array('%layout' => implode(', ', explode('|', $layout->id)))),
      drupal_get_destination(),
      t('This action cannot be undone.'),
      t('Revert'),
      t('Cancel')
    );
  }
  else {
    drupal_set_message(t('This operation is not possible.'));
  }
}

/**
 * Submit callback: revert layout and field settings.
 */
function ds_revert_layout_field_settings_form_submit(&$form, &$form_state) {
  $layout = $form['#layout'];

  db_delete('ds_field_settings')
    ->condition('id', $layout->id)
    ->execute();

  db_delete('ds_layout_settings')
    ->condition('id', $layout->id)
    ->execute();

  // Clear the ds_fields cache.
  cache_clear_all('ds_fields', 'cache');
  cache_clear_all('ds_field_settings', 'cache');

  // Clear entity info cache.
  cache_clear_all('entity_info', 'cache', TRUE);

  drupal_set_message(t('Layout has been reverted'));

  $form_state['redirect'] = isset($_GET['destination']) ? $_GET['destination'] : drupal_get_destination();
}

/**
 * Add Regions to 'Manage display' screen.
 *
 * @param $form
 *   The form to add layout fieldset and extra display suite fields.
 * @param $form_state
 *   The current form state.
 */
function ds_field_ui_regions($form, $form_state) {

  // Get the entity_type, bundle and view mode.
  $entity_type = $form['#entity_type'];
  $bundle = $form['#bundle'];
  $view_mode = $form['#view_mode'];

  // Check layout.
  $layout = isset($form['#ds_layout']) ? $form['#ds_layout'] : FALSE;

  // Change UI to add Region column if we have a layout.
  if ($layout) {

    $table = &$form['fields'];

    $table['#header'] = array(
      t('Field'),
      t('Weight'),
      t('Parent'),
      t('Region'),
      t('Label'),
      array('data' => t('Format'), 'colspan' => 3),
    );

    $table['#regions'] = array();
    foreach ($layout->regions as $region_key => $region_title) {
      $region_options[$region_key] = $region_title;
      $table['#regions'][$region_key] = array(
        'title' => $region_title,
        'message' => t('No fields are displayed in this region'),
      );
    }

    // Let other modules alter the regions.
    $context = array(
      'entity_type' => $entity_type,
      'bundle' => $bundle,
      'view_mode' => $view_mode
    );
    $region_info = array(
      'region_options' => &$region_options,
      'table_regions' => &$table['#regions'],
    );
    drupal_alter('ds_layout_region', $context, $region_info);

    $region_options['hidden'] = t('Disabled');
    $table['#regions']['hidden'] = array(
      'title' => t('Disabled'),
      'message' => t('No fields are hidden.')
    );

    $region = array(
      '#type' => 'select',
      '#options' => $region_options,
      '#default_value' => 'hidden',
      '#attributes' => array(
        'class' => array('ds-field-region'),
      )
    );

    // Update existing rows by changing rowHandler and adding regions.
    foreach (element_children($table) as $name) {
      $row = &$table[$name];
      $row['#js_settings'] = array('rowHandler' => 'ds');
      $row['#region_callback'] = 'ds_field_ui_row_region';

      // Remove hidden format.
      if (isset($row['format']['type']['#options']['hidden'])) {
        unset($row['format']['type']['#options']['hidden']);
      }

      // Add region between parent_wrapper & label.
      $second = array_splice($row, 7);
      $row['region'] = $region;
      $row['region']['#default_value'] = (isset($layout->settings['fields'][$name])) ? $layout->settings['fields'][$name] : 'hidden';
      $row = array_merge($row, $second);
    }
  }

  return $form;
}

/**
 * Returns the region to which a row in the Field UI screen belongs.
 *
 * @param $row
 *   The current row that is being rendered in the Field UI screen.
 */
function ds_field_ui_row_region($row) {
  return isset($row['region']['#value']) ? $row['region']['#value'] : 'hidden';
}

/**
 * Move the view modes so Field UI can handle them.
 */
function ds_field_ui_layouts_validate($form, &$form_state) {
  if (isset($form_state['values']['additional_settings']['modes']['view_modes_custom'])) {
    $form_state['values']['view_modes_custom'] = $form_state['values']['additional_settings']['modes']['view_modes_custom'];
  }
}

/**
 * Save the layout settings from the 'Manage display' screen.
 */
function ds_field_ui_layouts_save($form, &$form_state) {

  // Get some default values.
  $entity_type = $form['#entity_type'];
  $bundle = $form['#bundle'];
  $view_mode = isset($form['#view_mode']) ? $form['#view_mode'] : 'form';

  // Determine layout variables.
  $layout = $form_state['values']['additional_settings']['layout'];
  $hide_empty_regions = $form_state['values']['additional_settings']['hide_empty_regions'];
  $old_layout = $form_state['values']['additional_settings']['old_layout'];
  $new_layout = ($layout != $old_layout) || empty($old_layout);

  // Save layout and add regions if necessary.
  $record = new stdClass;
  $record->id = $form['#export_id'];
  $record->entity_type = $entity_type;
  $record->bundle = $bundle;
  $record->view_mode = $view_mode;
  $record->layout = $layout;
  $record->settings = array('hide_empty_regions' => $hide_empty_regions);

  // Remove old layout if necessary.
  if ($new_layout && !empty($old_layout)) {
    db_delete('ds_layout_settings')
      ->condition('entity_type', $entity_type)
      ->condition('bundle', $bundle)
      ->condition('view_mode', $view_mode)
      ->execute();
  }

  if ($new_layout && !empty($layout)) {
    // Save new layout.
    $record->settings = $record->settings;

    // Let other modules alter the layout settings.
    drupal_alter('ds_layout_settings', $record, $form_state);

    // Save the record.
    drupal_write_record('ds_layout_settings', $record);
  }
  // Update existing layout.
  elseif (!empty($layout)) {

    $fields = _ds_sort_fields($form_state['values']['fields'], 'weight');

    foreach ($fields as $key => $field) {

      // Make sure we need to save anything for this field.
      if (_ds_field_valid($key, $field, $form_state)) {
        continue;
      }

      if (!isset($record->settings['regions'][$field['region']])) {
        $record->settings['regions'][$field['region']] = array();
      }
      $record->settings['regions'][$field['region']][] = $key;
      $record->settings['fields'][$key] = $field['region'];
    }

    // Save the region classes.
    $record->settings['classes'] = array();
    foreach (array_keys($form['fields']['#regions']) as $region) {

      // Ignore hidden region.
      if ($region == 'hidden') {
        continue;
      }

      // Additional classes on regions.
      if (isset($form_state['values']['additional_settings'][$region])) {
        // Do not save empty string.
        $classes = implode(' ', $form_state['values']['additional_settings'][$region]);
        if (!empty($classes)) {
          $record->settings['classes'][$region] = $form_state['values']['additional_settings'][$region];
        }
      }
    }

    $record->settings = $record->settings;

    // Let other modules alter the layout settings.
    drupal_alter('ds_layout_settings', $record, $form_state);

    $l = $form['#ds_layout'];
    if ($l->export_type == 2) {
      drupal_write_record('ds_layout_settings', $record);
    }
    else {
      drupal_write_record('ds_layout_settings', $record, array('id'));
    }

    // Clear entity info cache.
    cache_clear_all('entity_info', 'cache', TRUE);
  }
}

/**
 * Save the field settings from the 'Manage display' screen.
 */
function ds_field_ui_fields_save($form, &$form_state) {

  // Setup some variables.
  $entity_type = $form['#entity_type'];
  $bundle = $form['#bundle'];
  $view_mode = $form['#view_mode'];

  // Delete previous field configuration configuration.
  db_delete('ds_field_settings')
    ->condition('entity_type', $entity_type)
    ->condition('bundle', $bundle)
    ->condition('view_mode', $view_mode)
    ->execute();

  $field_settings = array();

  // Save settings for each field.
  $fields = $form['#ds_fields'];
  foreach ($fields as $key => $field) {

    // Field settings.
    $field_values = $form_state['values']['fields'][$field];

    // In case the region is hidden, do not save.
    if (isset($field_values['region']) && $field_values['region'] == 'hidden') {
      continue;
    }

    // Build settings.
    $settings = array();
    $settings['weight'] = $field_values['weight'];
    $settings['label'] = $field_values['label'];
    $settings['format'] = $field_values['format']['type'];

    // Any formatter settings.
    if (isset($form_state['formatter_settings'][$field])) {
      $settings['formatter_settings'] = $form_state['formatter_settings'][$field];
    }

    $field_settings[$field] = $settings;
  }

  // Allow other modules to modify the field settings before they get saved.
  drupal_alter('ds_field_settings', $field_settings, $form, $form_state);

  // Save the record.
  if (!empty($field_settings)) {
    $record = new stdClass;
    $record->id = $form['#export_id'];
    $record->entity_type = $entity_type;
    $record->bundle = $bundle;
    $record->view_mode = $view_mode;
    $record->settings = $field_settings;
    drupal_write_record('ds_field_settings', $record);
  }

  // Clear the ds_fields cache.
  cache_clear_all('ds_fields', 'cache');
  cache_clear_all('ds_field_settings', 'cache');
}

/**
 * Creates a summary for the field format configuration summary.
 *
 * @param $field
 *   The configuration of the field.
 *
 * @return $summary
 *   An markup array.
 */
function ds_field_settings_summary($field) {
  $summary = module_invoke($field['module'], 'ds_field_format_summary', $field);
  return array(
    '#markup' => '<div class="field-formatter-summary">' . $summary . '</div>',
    '#cell_attributes' => array('class' => array('field-formatter-summary-cell')),
  );
}

/**
 * Creates a form for Display Suite fields.
 * .
 * @param $field
 *   The field definition.
 *
 * @return $form
 *   A form definition.
 */
function ds_field_settings_form($field) {
  return module_invoke($field['module'], 'ds_field_settings_form', $field);
}

/**
 * Implements hook_ds_field_format_summary().
 */
function ds_ds_field_format_summary($field) {
  $summary = '';
  $settings = isset($field['formatter_settings']) ? $field['formatter_settings'] : $field['properties']['default'];
  foreach ($settings as $key => $value) {
    if (!empty($value)) {
      $value = is_numeric($value) ? ($value ? t('Yes') : t('No')) : $value;
      $summary .= drupal_ucfirst(check_plain($key)) . ': ' . $value . '<br />';
    }
  }
  return $summary;
}

/**
 * Implements hook_ds_field_settings_form().
 */
function ds_ds_field_settings_form($field) {
  $form = array();

  $settings = isset($field['formatter_settings']) ? $field['formatter_settings'] : $field['properties']['default'];

  foreach ($field['properties']['settings'] as $key => $value) {

    switch ($value['type']) {

      case 'textfield':
        $form[$key] = array(
          '#type' => 'textfield',
          '#title' => check_plain(drupal_ucfirst($key)),
          '#default_value' => isset($settings[$key]) ? $settings[$key] : '',
          '#description' => isset($value['description']) ? check_plain($value['description']) : '',
        );
        break;

      case 'select':
        $form[$key] = array(
          '#type' => 'select',
          '#title' => check_plain(drupal_ucfirst($key)),
          '#default_value' => isset($settings[$key]) ? $settings[$key] : '',
          '#options' => $value['options'],
          '#description' => isset($value['description']) ? check_plain($value['description']) : '',
        );
        break;

    }
  }

  return $form;
}

/**
 * Add the layouts fieldset on the Field UI screen.
 *
 * @param $entity_type
 *   The name of the entity type.
 * @param $bundle
 *   The name of the bundle
 * @param $view_mode
 *   The name of the view_mode
 * @param $form
 *   A collection of form properties.
 */
function _ds_field_ui_table_layouts($entity_type, $bundle, $view_mode, &$form) {

  $layout_options = array();
  $ds_layouts = ds_get_layout_info();
  $layout_options = array('' => t('- None -'));
  foreach ($ds_layouts as $key => $layout) {
    $layout_options[$key] = $layout['label'];
  }

  // Add additional settings vertical tab.
  if (!isset($form['additional_settings'])) {
    $form['additional_settings'] = array(
      '#type' => 'vertical_tabs',
      '#theme_wrappers' => array('vertical_tabs'),
      '#prefix' => '<div>',
      '#suffix' => '</div>',
      '#tree' => TRUE,
    );
    $form['#attached']['js'][] = 'misc/form.js';
    $form['#attached']['js'][] = 'misc/collapse.js';
  }

  // Add layouts form.
  $form['additional_settings']['ds_layouts'] = array(
    '#type' => 'fieldset',
    '#title' => t('Layout for !bundle in !view_mode', array('!bundle' => str_replace('_', ' ', $bundle), '!view_mode' => str_replace('_', ' ', $view_mode))),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#parents' => array('additional_settings'),
  );

  ctools_include('export');
  $layout = new stdClass();
  $ds_layout_settings = ctools_export_crud_load_all('ds_layout_settings');
  if (isset($ds_layout_settings[$form['#export_id']])) {
    $layout = $ds_layout_settings[$form['#export_id']];
  }

  if (!empty($layout) && isset($layout->layout) && isset($ds_layouts[$layout->layout])) {
    $chosen_layout = $ds_layouts[$layout->layout];
    $layout_string = $layout->layout;
    $selected = t('You have selected the %layout_label layout. The default template can be found in %path', array('%layout_label' => $chosen_layout['label'], '%path' => $chosen_layout['path']));
    $suggestions = 'Possible template suggestions are: <br />';
    $suggestions .= '- ' . $layout_string . '--' . $entity_type . '.tpl.php<br />';
    $suggestions .= '- ' . $layout_string . '--' . $entity_type . '-' . $bundle . '.tpl.php<br />';
    $suggestions .= '- ' . $layout_string . '--' . $entity_type . '-' . $bundle . '-' . $view_mode . '.tpl.php<br />';
    $layout->settings = $layout->settings;
    $layout->regions = $chosen_layout['regions'];
    $form['#ds_layout'] = $layout;
  }

  $form['additional_settings']['ds_layouts']['layout'] = array(
    '#type' => 'select',
    '#title' => t('Select a layout'),
    '#options' => $layout_options,
    '#default_value' => isset($layout->layout) ? $layout->layout : '',
    '#weight' => -1,
  );

  if (isset($layout->export_type) && $layout->export_type == 3) {
    $form['additional_settings']['ds_layouts']['revert'] = array(
      '#markup' => l(t('This layout is overridden. Click to revert to default settings.'), 'admin/structure/ds/revert-layout/' . $form['#export_id'], array('query' => drupal_get_destination())),
      '#weight' => 0,
    );
  }

  $form['additional_settings']['ds_layouts']['hide_empty_regions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide empty regions'),
    '#default_value' => isset($layout->settings['hide_empty_regions']) ? $layout->settings['hide_empty_regions'] : FALSE,
    '#weight' => 1,
  );

  if (!empty($layout) && isset($layout->regions)) {
    $form['additional_settings']['ds_layouts']['suggestions'] = array(
      '#markup' => '<p>' . $selected . '</p><p>' . t('!suggestions', array('!suggestions' => strtr($suggestions, '_', '-'))) . '</p>',
    );

    // Add extra classes for the regions to have more control while theming.
    $form['additional_settings']['ds_classes'] = array(
      '#type' => 'fieldset',
      '#title' => t('Extra classes for regions'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#parents' => array('additional_settings'),
    );

    $styles = _ds_styles();
    if (!empty($styles)) {
      foreach (array_keys($layout->regions) as $region) {
        $form['additional_settings']['ds_classes'][$region] = array(
          '#type' => 'select',
          '#multiple' => TRUE,
          '#options' => $styles,
          '#title' => t('Class for @region', array('@region' => $region)),
          '#default_value' => isset($layout->settings['classes'], $layout->settings['classes'][$region]) ? $layout->settings['classes'][$region] : '',
        );
      }
      $form['additional_settings']['ds_classes']['info'] = array('#markup' => l(t('Manage region styles'), 'admin/structure/ds/styles'));
    }
    else {
      $form['additional_settings']['ds_classes']['info'] = array('#markup' => '<p>' . t('You have not defined any styles which can be used on regions.') . '</p><p>' . l(t('Manage region styles'), 'admin/structure/ds/styles') . '</p>');
    }
  }

  $form['additional_settings']['ds_layouts']['id'] = array(
    '#type' => 'value',
    '#value' => isset($layout->id) ? $layout->id : $form['#export_id'],
  );

  $form['additional_settings']['ds_layouts']['old_layout'] = array(
    '#type' => 'value',
    '#value' => isset($layout->layout) ? $layout->layout : 0,
  );

  // Add validate and submit handlers. Layout needs be first so
  // we can reset the type key for Field API fields.
  $form['#validate'][] = 'ds_field_ui_layouts_validate';
  $submit = $form['#submit'];
  $form['#submit'] = array('ds_field_ui_layouts_save');
  $form['#submit'] = array_merge($form['#submit'], $submit);
}

/**
 * Add the fields to the Field UI form.
 *
 * @param $entity_type
 *   The name of the entity type.
 * @param $bundle
 *   The name of the bundle
 * @param $view_mode
 *   The name of the view_mode
 * @param $form
 *   A collection of form properties.
 * @param $form_state
 *   A collection of form_state properties.
 */
function _ds_field_ui_fields($entity_type, $bundle, $view_mode, &$form, &$form_state) {

  // Do not add the fields if there is no layout.
  if (!isset($form['#ds_layout'])) {
    return;
  }

  // Get the fields and put them on the form.
  $fields = ds_get_fields($entity_type, FALSE);
  $field_settings = ds_get_field_settings($entity_type, $bundle, $view_mode);
  $form['#field_settings'] = $field_settings;

  $table = &$form['fields'];
  $form['#ds_fields'] = array();

  $field_label_options = array(
    'above' => t('Above'),
    'inline' => t('Inline'),
    'hidden' => t('<Hidden>'),
  );
  drupal_alter('ds_label_options', $field_label_options);

  foreach ($fields as $key => $field) {

    // Check on ui_limit.
    if (isset($field['ui_limit'])) {

      $continue = TRUE;

      foreach ($field['ui_limit'] as $limitation) {

        list($limit_bundle, $limit_view_mode) = explode('|', $limitation);
        if ($limit_bundle == '*' || $limit_bundle == $bundle) {
          if ($limit_view_mode == '*' || $limit_view_mode == $view_mode) {
            $continue = FALSE;
          }
        }
      }

      if ($continue) {
        continue;
      }
    }

    $form['#ds_fields'][] = $key;

    // Check on formatter settings.
    if (isset($form_state['formatter_settings'][$key])) {
      $field['formatter_settings'] = $form_state['formatter_settings'][$key];
    }
    elseif (isset($field_settings[$key]['formatter_settings'])) {
      $field['formatter_settings'] = $field_settings[$key]['formatter_settings'];
      $form_state['formatter_settings'][$key] = $field['formatter_settings'];
    }

    $hidden = array('hidden' => t('<Hidden>'));
    $formatters = isset($field['properties']['formatters']) ? $hidden + $field['properties']['formatters'] : $hidden + array('default' => t('Default'));

    $table[$key] = array(
      '#row_type' => 'field',
      '#js_settings' => array('field'),
      '#region_callback' => 'field_ui_display_overview_row_region',
      '#attributes' => array('class' => array('draggable', 'tabledrag-leaf')),
      'human_name' => array(
        '#markup' => check_plain($field['title']),
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#default_value' => isset($field_settings[$key]['weight']) ? $field_settings[$key]['weight'] : 0,
        '#size' => 3,
        '#attributes' => array('class' => array('field-weight')),
      ),
      'parent_wrapper' => array(
        'parent' => array(
          '#type' => 'select',
          '#empty_value' => '',
          '#options' => array(),
          '#attributes' => array('class' => array('field-parent')),
          '#parents' => array('fields', $key, 'parent'),
        ),
        'hidden_name' => array(
          '#type' => 'hidden',
          '#default_value' => $key,
          '#attributes' => array('class' => array('field-name')),
        ),
      ),
      'label' => array(
        '#type' => 'select',
        '#options' => $field_label_options,
        '#default_value' => isset($field_settings[$key]['label']) ? $field_settings[$key]['label'] : 'hidden',
      ),
      'format' => array(
        'type' => array(
          '#type' => 'select',
          '#options' => $formatters,
          '#default_value' => isset($field_settings[$key]['format']) ? $field_settings[$key]['format'] : 'hidden',
          '#attributes' => array('class' => array('field-formatter-type')),
        ),
      ),
      'settings_summary' => array(),
      'settings_edit' => array(),
    );

    // Not all fields have settings.
    if (!isset($field['properties']['settings'])) {
      continue;
    }

    $base_button = array(
      '#submit' => array('field_ui_display_overview_multistep_submit'),
      '#ajax' => array(
        'callback' => 'field_ui_display_overview_multistep_js',
        'wrapper' => 'field-display-overview-wrapper',
        'effect' => 'fade',
      ),
      '#field_name' => $key,
    );

    if ($form_state['formatter_settings_edit'] == $key) {
      $table[$key]['settings_edit'] = array(
        '#type' => 'container',
        '#attributes' => array('class' => array('field-formatter-settings-edit-form')),
        '#parents' => array('fields', $key, 'settings_edit_form'),
        '#weight' => -5,
        'label' => array(
          '#markup' => '<h3>' . t('Field settings') . '</h3>',
        ),
        // Create a settings form where hooks can pick in.
        'settings' => ds_field_settings_form($field),
        'actions' => array(
          '#type' => 'actions',
          'save_settings' => $base_button + array(
            '#type' => 'submit',
            '#name' => $key . '_formatter_settings_update',
            '#value' => t('Update'),
            '#op' => 'update',
          ),
          'cancel_settings' => $base_button + array(
            '#type' => 'submit',
            '#name' => $key . '_formatter_settings_cancel',
            '#value' => t('Cancel'),
            '#op' => 'cancel',
            // Do not check errors for the 'Cancel' button.
            '#limit_validation_errors' => array(),
          ),
        ),
      );
      $table[$key]['#attributes']['class'][] = 'field-formatter-settings-editing';
      $table[$key]['format']['type']['#attributes']['class'] = array('element-invisible');
    }
    else {
      // After saving, the settings are updated here aswell. First we create
      // the element for the table cell.
      $table[$key]['settings_summary'] = ds_field_settings_summary($field);
      // Add the configure button.
      $table[$key]['settings_edit'] = $base_button + array(
        '#type' => 'image_button',
        '#name' => $key . '_group_settings_edit',
        '#src' => 'misc/configure.png',
        '#attributes' => array('class' => array('field-formatter-settings-edit'), 'alt' => t('Edit')),
        '#op' => 'edit',
        // Do not check errors for the 'Edit' button.
        '#limit_validation_errors' => array(),
        '#prefix' => '<div class="field-formatter-settings-edit-wrapper">',
        '#suffix' => '</div>',
      );
    }
  }

  // Add fields submit handler.
  $form['#submit'][] = 'ds_field_ui_fields_save';
}

/**
 * Utility function to check if we need to save anything for this field.
 */
function _ds_field_valid($key, $field, &$form_state) {
  $continue = FALSE;

  // Ignore the Field group module and the region to block plugin.
  if ($key == '_add_new_group' || $key == '_add_new_block_region') {
    $continue = TRUE;
  }

  // If the field is in hidden region, do not save. Check if the
  // field has a type key which means it's from Field API and
  // we need to reset that type to 'hidden' so it doesn't get
  // fired by Field API in the frontend.
  if ($field['region'] == 'hidden') {
    if (isset($field['type'])) {
      $form_state['values']['fields'][$key]['type'] = 'hidden';
    }
    $continue = TRUE;
  }

  return $continue;
}

/**
 * Utility function to return styles.
 */
function _ds_styles($name = 'ds_styles_regions') {
  static $styles = array();

  if (!isset($styles[$name])) {
    $styles[$name] = array();
    $custom_styles = trim(variable_get($name, ''));
    if (!empty($custom_styles)) {
      $styles[$name][''] = t('None');
      $custom_styles = explode("\n", $custom_styles);
      foreach ($custom_styles as $key => $value) {
        $classes = explode("|", $value);
        $key = trim($classes[0]);
        $friendly_name = isset($classes[1]) ? trim($classes[1]) : $key;
        $styles[$name][check_plain($key)] = $friendly_name;
      }
    }
  }

  return $styles[$name];
}

/**
 * Utility function to sort a multidimensional array by a value in a sub-array.
 *
 * @param $a
 *   The array to sort.
 * @param $subkey
 *   The subkey to sort by.
 */
function _ds_sort_fields($a, $subkey) {
  foreach ($a as $k => $v) {
    if (isset($v[$subkey])) {
      $b[$k] = $v[$subkey];
    }
  }
  asort($b);
  foreach ($b as $key => $val) {
    $c[$key] = $a[$key];
  }
  return $c;
}

