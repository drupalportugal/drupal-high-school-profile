<?php

/**
 * @file
 * Views displays functions.
 */

/**
 * Manage the display of views.
 */
function ds_extras_vd($bundle = '', $action = '') {

  $entity_info = entity_get_info('ds_views');

  if (!empty($bundle) && isset($entity_info['bundles'][$bundle]) && $action == 'remove') {
    return drupal_get_form('ds_extras_vd_bundle_remove', $bundle, $entity_info['bundles'][$bundle]['label']);
  }

  if (!empty($bundle) && isset($entity_info['bundles'][$bundle]) && $action == 'display') {
    return ds_extras_vd_field_ui($bundle);
  }

  return ds_extras_vd_overview($entity_info);
}

/**
 * Show the overview form and the selection list.
 */
function ds_extras_vd_overview($entity_info) {
  $build = $rows = array();

  ctools_include('export');
  $vd_settings = ctools_export_crud_load_all('ds_vd');

  if ($entity_info) {
    foreach ($entity_info['bundles'] as $key => $value) {

      // If no initial bundles were created, entity API will create
      // one by default, so make sure we do not list that one.
      if ($key == 'ds_views') {
        continue;
      }

      $row = array();
      $row[] = $value['label'];
      $operations = l(t('Manage layout'), 'admin/structure/ds/vd/' . $key . '/display');

      if (isset($vd_settings[$key]) && $vd_settings[$key]->export_type == 1) {
        $operations .= ' - ' . l(t('Remove'), 'admin/structure/ds/vd/' . $key . '/remove');
      }
      $row[] = $operations;

      $rows[$key] = $row;
    }
  }

  if (empty($rows)) {
    $rows = array(
      array(array('data' => t('No views selected.'), 'colspan' => '2')),
    );
  }

  $variables = array(
    'header' => array(t('Title'), t('Operations')),
    'rows' => $rows,
  );
  $build['list'] = array('#markup' => theme('table', $variables));
  $build['form'] = drupal_get_form('ds_extras_vd_bundle_form', $rows);

  return $build;
}

/**
 * Return the views select form to create a bundle.
 */
function ds_extras_vd_bundle_form($form, $form_state, $rows) {

  $options = array();
  $views = views_get_all_views();
  foreach ($views as $view_key => $view) {

    // Ignore disabled views.
    if (isset($view->disabled) && $view->disabled) {
      continue;
    }

    // Loop through all displays.
    foreach ($view->display as $display_key => $display) {

      // Ignore default displays.
      if ($display_key == 'default') {
        continue;
      }

      $key = $view_key . '-' . $display_key;
      if (!isset($rows[$key])) {
        $options[$key] = drupal_ucfirst($view->name) . ': ' . $display->display_title;
      }
    }
  }

  $form['vd'] = array(
    '#title' => t('Select view'),
    '#description' => t('Select a Views display that you want to manage with Display Suite.'),
    '#type' => 'select',
    '#options' => $options,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );

  return $form;
}

/**
 * Submit callback: save the new bundle.
 */
function ds_extras_vd_bundle_form_submit($form, &$form_state) {

  // Save new bundle.
  $record = new stdClass();
  $record->vd = $form_state['values']['vd'];
  $record->label = $form['vd']['#options'][$record->vd];
  drupal_write_record('ds_vd', $record);

  // Clear entity cache and field info fields cache.
  cache_clear_all('field_info_fields', 'cache_field');
  cache_clear_all('entity_info', 'cache', TRUE);

  // Message and redirect.
  drupal_set_message(t('Bundle @label has been added.', array('@label' => $record->label)));
  $form_state['redirect'] = 'admin/structure/ds/vd';
}

/**
 * Return Field UI display screen for a view and bundle.
 *
 * @param $bundle
 *   The name of the bundle
 */
function ds_extras_vd_field_ui($bundle) {

  // Use drupal_build_form instead of drupal_get_form.
  $form_state = array();
  $arguments = array('ds_views', $bundle, 'default');
  $form_state['build_info']['args'] = $arguments;
  form_load_include($form_state, 'inc', 'field_ui', 'field_ui.admin');
  form_load_include($form_state, 'inc', 'ds_extras', 'ds_extras.vd');
  $build = drupal_build_form('field_ui_display_overview_form', $form_state);

  // Deny access to view mode.
  $build['additional_settings']['modes']['#access'] = FALSE;

  // Deny access to field_group if it exists.
  if (module_exists('field_group')) {
    unset($build['fields']['_add_new_group']);
  }

  return $build;
}

/**
 * Return confirm form to remove a views bundle
 */
function ds_extras_vd_bundle_remove($form, $form_state, $bundle, $label) {
  $form['#bundle'] = $bundle;
  $form['#label'] = $label;
  return confirm_form($form, t('Are you sure you want to remove bundle @label ?', array('@label' => $label)), 'admin/structure/ds/vd');
}

/**
 * Submit callback: remove a views bundle
 */
function ds_extras_vd_bundle_remove_submit($form, &$form_state) {

  $bundle = $form['#bundle'];
  $label = $form['#label'];

  // Remove bundle.
  db_delete('ds_vd')
    ->condition('vd', $bundle)
    ->execute();

  // Remove layout.
  db_delete('ds_layout_settings')
    ->condition('bundle', $bundle)
    ->execute();

  // Remove settings.
  db_delete('ds_field_settings')
    ->condition('bundle', $bundle)
    ->execute();

  // Clear entity cache and field info fields cache.
  cache_clear_all('field_info_fields', 'cache_field');
  cache_clear_all('entity_info', 'cache', TRUE);

  drupal_set_message(t('Bundle @label has been removed.', array('@label' => $label)));
}
