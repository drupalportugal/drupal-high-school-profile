<?php

/**
 * @file
 * Views hooks file.
 */

/**
 * Implements hook_views_plugins().
 */
function ds_views_plugins() {
  $path = drupal_get_path('module', 'ds');
  return array(
    'module' => 'ds',
    'row' => array(
      'ds' => array(
        'title' => t('Display suite'),
        'help' => t('Display the entity with the Display suite module.'),
        'handler' => 'views_plugin_ds_entity_view',
        'path' => $path . '/views',
        'base' => array('node', 'comment', 'users', 'apachesolr', 'taxonomy_term_data'),
        'theme' => 'ds_row_entity',
        'uses options' => TRUE,
        'type' => 'normal',
      ),
    ),
  );
}
