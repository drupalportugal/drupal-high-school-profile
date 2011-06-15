<?php

/**
 * @file
 * Implements a generic entity delete action. Uses Entity API if available.
 */

function views_bulk_operations_delete_action_info() {
  return array(
    'views_bulk_operations_delete_item' => array(
      'type' => 'system',
      'label' => t('Delete item'),
      'configurable' => FALSE,
      'access operation' => VBO_ACCESS_OP_DELETE,
    ),
  );
}

function views_bulk_operations_delete_item($entity, $context) {
  $info = entity_get_info($context['entity_type']);
  $entity_id = $entity->{$info['entity keys']['id']};

  if (module_exists('entity')) {
    entity_delete($context['entity_type'], $entity_id);
  }
  else {
    // This works for core entity types, so there's no need to depend on Entity API.
    $delete_function = $context['entity_type'] . '_delete';
    if (function_exists($delete_function)) {
      $delete_function($entity_id);
    }
  }
}
