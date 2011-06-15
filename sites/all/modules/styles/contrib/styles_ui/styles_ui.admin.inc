<?php
// $Id: styles_ui.admin.inc,v 1.1.2.8 2010/05/15 13:36:36 aaron Exp $

/**
 *  @file
 *  Administrative page callbacks for the Styles UI module.
 */

/**
 *  Page callback for various styles preset overview listing pages.
 */
function styles_ui_containers_overview($field_type) {
  $presets = styles_presets();
  $presets = $presets[$field_type];
  $styles_containers = styles_containers();
  $field_containers = $styles_containers[$field_type];
  $field_info = field_info_field_types($field_type);
  $field_label = $field_info['label'];

  $header = array(t('Presets'), array('data' => t('Operations'), 'colspan' => 2));
  $rows = array();

  foreach ($presets as $key => $preset) {
    $row = array(theme('styles_ui_admin_overview', array('preset_name' => $key)));
    $row[] = array('data' => l(t('edit'), $field_containers['admin']['path'] . '/edit/' . $key));
    $row[] = array('data' => l(t('delete'), $field_containers['admin']['path'] . '/delete/' . $key));
    $rows[] = $row;
  }

  $title = 'Add ' . $field_label . ' style preset';

  $build['styles_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No styles available. <a href="@link">@add</a>.', array('@add' => t($title), '@link' => url($field_containers['admin']['path'] . '/add'))),
  );

  return $build;
}

/**
 *  Display the preset name on the admin overview page.
 */
function theme_styles_ui_admin_overview($variables) {
  $preset_name = $variables['preset_name'];

  $output = check_plain($preset_name);
  return $output;
}

/**
 *  Page callback for adding a styles preset.
 */
function styles_ui_preset_add_form($form, $form_state, $field_type) {
  $form = array();
  $form['field_type'] = array(
    '#type' => 'value',
    '#value' => $field_type,
  );
  $form['preset_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Preset name'),
    '#description' => t('Enter the name of your desired preset, which must be a unique name containing only alphanumeric characters and underscores.'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 *  Validation handler for the add style preset form.
 */
function styles_ui_preset_add_form_validate($form, $form_state) {
  $field_type = $form_state['values']['field_type'];
  $preset_name = $form_state['values']['preset_name'];
  $presets = styles_presets();
  if (!preg_match('!^[a-z0-9_]+$!', $preset_name)) {
    form_set_error('preset_name', t('The machine-readable style preset name must contain only lowercase letters, numbers, and underscores.'));
  }
  else if (!empty($presets[$field_type][$preset_name])) {
    form_set_error('preset_name', t('The machine-readable style preset name %preset_name is already taken.', array('%preset_name' => $preset_name)));
  }
}

/**
 *  Submission handler for the add style preset form.
 */
function styles_ui_preset_add_form_submit($form, $form_state) {
  $field_type = $form_state['values']['field_type'];
  $preset_name = $form_state['values']['preset_name'];
  $presets = styles_presets();
  // @TODO: Save the new preset.
}

/**
 *  Callback for the preset edit form.
 */
function styles_ui_preset_edit_form($form, $form_state, $field_type, $preset_name) {
  $field_info = field_info_field_types($field_type);
  $presets = styles_presets();
  $preset = $presets[$field_type][$preset_name];
  $styles_containers = styles_containers();
  $containers = $styles_containers[$field_type]['containers'];
  drupal_set_title(t('Edit @field_type style preset: @preset', array('@field_type' => $field_info['label'], '@preset' => $preset_name)));
  $form = array();
  $form['containers'] = array(
    '#type' => 'vertical_tabs',
  );

  // Begin the settings array to send to jQuery.
  $settings = array(
    'stylesUI' => array(
      'url' => url('styles-ui/preview'),
      'fieldType' => check_plain($field_type),
    ),
  );
  foreach ($containers as $container_name => $container) {
    $form['containers'][$container_name] = array(
      '#type' => 'fieldset',
      '#title' => $container['label'],
    );
    $this_preset = array_pop($preset[$container_name]);
    $options = array();
    foreach ($container['available styles'] as $style_name => $style) {
      $options[$style_name] = $style_name;
    }

    // Store the container in the 'rel' attribute for later AJAX previews.
    $rel = check_plain($container_name);
    $form['containers'][$container_name]['preset_' . $container_name] = array(
      '#type' => 'radios',
      '#title' => t('Style preset'),
      '#default_value' => $this_preset['name'],
      '#options' => $options,
      '#attributes' => array('class' => array('styles-ui-preset'), 'rel' => $rel),
    );
    // Add a preview.
    if (isset($containers[$container_name]['themes']['preview'])) {
      $preview = '<div id="styles-ui-preview-wrapper-' . $rel . '" class="styles-ui-preview-wrapper">' . theme($containers[$container_name]['themes']['preview'], array('field_type' => $field_type, 'container_name' => $container_name, 'style_name' => $this_preset['name'])) . '</div>';
      $form['containers'][$container_name]['preview_' . $container_name] = array(
        '#type' => 'item',
        '#title' => t('Style preview'),
        '#markup' => $preview,
      );
    }
  }
  // Add the javascript for live previews on radio select.
  $form['#attached'] = array(
    'js' => array(drupal_get_path('module', 'styles_ui') . '/styles_ui.js'),
  );
  drupal_add_js($settings, array('type' => 'setting'));
  return $form;
}

function styles_ui_preview_ajax($field_type, $container_name, $preset_name) {
  $styles_containers = styles_containers();
  $containers = $styles_containers[$field_type]['containers'];
  drupal_json_output(array(
    'id' => '#styles-ui-preview-wrapper-' . check_plain($container_name),
    'preview' => theme($containers[$container_name]['themes']['preview'], array('field_type' => $field_type, 'container_name' => $container_name, 'style_name' => $preset_name)),
  ));
  die();
}
