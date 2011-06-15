<?php

/**
 * @file
 * Provides the Display suite views entity style plugin.
 */

/**
 * Plugin which defines the view mode on the resulting entity object.
 */
class views_plugin_ds_entity_view extends views_plugin_row {

  function init(&$view, &$display, $options = NULL) {
    parent::init($view, $display, $options);
    $this->base_table = $view->base_table;
    // Special case for apachesolr_views.
    if ($this->base_table == 'apachesolr') {
      $this->base_table = 'node';
    }
    $this->base_field = $this->ds_views_3_support();
  }

  // Return base_field based on base_table. It might not be
  // the cleanest solution, it's the fastest though.
  function ds_views_3_support() {
    $base_table_fields = array(
      'node' => 'nid',
      'comment' => 'cid',
      'users' => 'uid',
      'apachesolr' => 'nid',
      'taxonomy_term_data' => 'tid',
    );
    return $base_table_fields[$this->base_table];
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['view_mode'] = array('default' => 'teaser');
    $options['alternating'] = array('default' => FALSE);
    $options['changing'] = array('default' => FALSE);
    $options['grouping'] = array('default' => FALSE);
    $options['advanced'] = array('default' => FALSE);
    return $options;
  }

  function options_form(&$form, &$form_state) {

    $view_mode_options = array();
    $entity_type = $this->view->base_table;
    // In case we're working with users, change the entity type variable.
    if ($entity_type == 'users') $entity_type = 'user';
    $entity_view_modes = ds_entity_view_modes($entity_type);
    foreach ($entity_view_modes as $key => $view_mode) {
      $view_mode_options[$key] = $view_mode['label'];
    }

    // Default view mode.
    $form['default_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Default view mode'),
      '#collapsible' => TRUE,
      '#collapsed' => ($this->options['advanced']),
    );
    $form['default_fieldset']['view_mode'] = array(
      '#type' => 'select',
      '#default_value' => $this->options['view_mode'],
      '#options' => $view_mode_options,
      '#description' => t('Make sure you select a view mode which is compatible with the base table of this view which is %base_table. <br />Also note that if you excluded build modes, you will still see them listed here as it\'s possible to list different types in a view.', array('%base_table' => $this->view->base_table))
    );

    // Changing view modes.
    $form['alternating_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Alternating view mode'),
      '#collapsible' => TRUE,
      '#collapsed' => !$this->options['alternating'],
    );
    $form['alternating_fieldset']['alternating'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use the changing view mode selector'),
      '#default_value' => $this->options['alternating'],
    );
    $form['alternating_fieldset']['allpages'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use this configuration on every page. Otherwhise the default view mode is used as soon you browse away from the first page of this view.'),
      '#default_value' => (isset($this->options['alternating_fieldset']['allpages'])) ? $this->options['alternating_fieldset']['allpages'] : FALSE,
    );

    $limit = $this->view->display_handler->get_option('items_per_page');
    $pager = $this->view->display_handler->get_plugin('pager');
    $limit = (isset($pager->options['items_per_page'])) ? $pager->options['items_per_page'] : 0;
    if ($limit == 0 || $limit > 20) {
      $form['alternating_fieldset']['disabled'] = array(
        '#markup' => t('This option is disabled because you have unlimited items or listing more than 20 items.'),
      );
      $form['alternating_fieldset']['alternating']['#disabled'] = TRUE;
      $form['alternating_fieldset']['allpages']['#disabled'] = TRUE;
    }
    else {
      $i = 1;
      $a = 0;
      while ($limit != 0) {
        $form['alternating_fieldset']['item_' . $a] = array(
          '#title' => t('Item @nr', array('@nr' => $i)),
          '#type' => 'select',
          '#default_value' => (isset($this->options['alternating_fieldset']['item_' . $a])) ? $this->options['alternating_fieldset']['item_' . $a] : 'teaser',
          '#options' => $view_mode_options,
        );
        $limit--;
        $a++;
        $i++;
      }
    }

    // Grouping rows.
    $sorts = $this->view->display_handler->get_option('sorts');
    $groupable = !empty($sorts) && $this->options['grouping'];
    $form['grouping_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Group data'),
      '#collapsible' => TRUE,
      '#collapsed' => !$groupable,
    );
    $form['grouping_fieldset']['grouping'] = array(
      '#type' => 'checkbox',
      '#title' => t('Group data on a field. The value of this field will be displayed too.'),
      '#default_value' => $groupable,
    );
    if (!empty($sorts)) {
      $sort_options = array();
      foreach ($sorts as $key => $sort) {
        $sort_name = drupal_ucfirst(str_replace('_', ' ', $sort['table']) . ' ' . $sort['field']);
        $sort_options[$sort['table'] . '_' . $sort['field']] = $sort_name;
      }

      $form['grouping_fieldset']['group_field'] = array(
        '#type' => 'select',
        '#options' => $sort_options,
        '#default_value' => isset($this->options['grouping_fieldset']['group_field']) ? $this->options['grouping_fieldset']['group_field'] : '',
      );

      $form['grouping_fieldset']['group_field_function'] = array(
        '#type' => 'textfield',
        '#title' => 'Heading function',
        '#description' => check_plain(t('The value of the field can be in a very raw format (eg, date created). Enter a custom function which you can use to format that value. The value and the object will be passed into that function eg. <em>custom_function($raw_value, $object);</em>')),
        '#default_value' => isset($this->options['grouping_fieldset']['group_field_function']) ? $this->options['grouping_fieldset']['group_field_function'] : '',
      );
    }
    else {
      $form['grouping_fieldset']['grouping']['#disabled'] = TRUE;
      $form['grouping_fieldset']['grouping']['#description'] = t('Grouping is disabled because you do not have any sort fields.');
    }

    // Advanced function.
    $form['advanced_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Advanced view mode'),
      '#collapsible' => TRUE,
      '#collapsed' => !$this->options['advanced'],
    );
    $form['advanced_fieldset']['advanced'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use the advanced build mode selector'),
      '#description' => t('This gives you the opportunity to have full control of a list for really advanced features.<br /> There is no UI for this, you need to create a function named like this: <strong><em>ds_views_row_adv_@VIEWSNAME($entity, $view_mode)</em></strong>.<br />See <a href="http://drupal.org/node/697320#ds_views_row_adv_VIEWSNAME">http://drupal.org/node/697320#ds_views_row_adv_VIEWSNAME</a> for an example.', array('@VIEWSNAME' => $this->view->name)),
      '#default_value' => $this->options['advanced'],
    );
  }

  /**
   * Validate view mode type selector.
   */
  function options_validate($form, &$form_state) {
    if (($form_state['values']['row_options']['alternating_fieldset']['alternate'] || $form_state['values']['row_options']['grouping_fieldset']['grouping']) && $form_state['values']['row_options']['advanced_fieldset']['advanced']) {
      form_set_error('advanced', t('You can not have changing/grouping and advanced enabled at the same time'));
    }
  }

  /**
   * Reset all fieldsets except for changing.
   */
  function options_submit($form, &$form_state) {
    $form_state['values']['row_options']['view_mode'] = $form_state['values']['row_options']['default_fieldset']['view_mode'];
    $form_state['values']['row_options']['alternating'] = $form_state['values']['row_options']['alternating_fieldset']['alternating'];
    $form_state['values']['row_options']['grouping'] = $form_state['values']['row_options']['grouping_fieldset']['grouping'];
    $form_state['values']['row_options']['advanced'] = $form_state['values']['row_options']['advanced_fieldset']['advanced'];
  }

  /**
   * Preload all entities.
   */
  function pre_render($values) {
    $ids = array();
    foreach ($values as $row) {
      $ids[] = $row->{$this->field_alias};
    }

    switch ($this->base_table) {
      case 'node':
        $this->entities = node_load_multiple($ids);
        break;
      case 'comment':
        $this->entities = comment_load_multiple($ids);
        break;
      case 'users':
        $this->entities = user_load_multiple($ids);
        break;
      case 'taxonomy_term_data':
        $this->entities = taxonomy_term_load_multiple($ids);
        break;
      default:
        break;
    }
  }

  /**
   * Render each $row.
   */
  function render($row) {

    // The advanced selector searches for a function called
    // ds_views_row_adv_VIEWSNAME. Return the row immediately.
    if ($this->options['advanced']) {
      $row_function = 'ds_views_row_adv_' . $this->view->name;
      return $row_function($this->entities[$row->{$this->field_alias}], $this->options['view_mode']);
    }

    // Keep a static group array.
    static $grouping = array();
    $view_name = $this->view->name . '_' . $this->view->current_display;
    $group_value_content = '';

    // Default view mode.
    $view_mode = $this->options['view_mode'];

    // Change the view mode per row.
    if ($this->options['alternating']) {
      // Check for paging to determine the view mode.
      if (isset($_GET['page']) && isset($this->options['alternating_fieldset']['allpages']) && !$this->options['alternating_fieldset']['allpages']) {
        $view_mode = $this->options['view_mode'];
      }
      else {
        $view_mode = $this->options['alternating_fieldset']['item_' . $this->view->row_index];
      }
    }

    // Call the row render function.
    $content = $this->ds_views_row_render_entity($view_mode, $row);

    // Keep a static grouping for this view.
    if ($this->options['grouping']) {
      $raw_group_value = $row->{$this->options['grouping_fieldset']['group_field']};
      if (!isset($grouping[$view_name][$raw_group_value])) {
        $group_value = $raw_group_value;
        // Special function to format the heading value.
        if (!empty($this->options['grouping_fieldset']['group_field_function'])) {
          $function = $this->options['grouping_fieldset']['group_field_function'];
          if (function_exists($function)) {
            $group_value = $function($raw_group_value, $this->entities[$row->{$this->field_alias}]);
          }
        }
        $group_value_content = '<h2 class="grouping-title">' . $group_value . '</h2>';
        $grouping[$view_name][$raw_group_value] = $raw_group_value;
      }
    }

    // Grouping.
    if (!empty($grouping[$view_name])) {
      if (!empty($group_value_content)) {
        $content = $group_value_content . $content;
      }
      $content = '<div class="grouping-content">' . $content . '</div>';
    }

    // Return the content.
    return $content;
  }

  /**
   * Render a discrete entity based with the selected view mode.
   *
   * @param $view_mode
   *   The view mode which is set in the Views' options.
   * @param $row
   *   The current active row object being rendered.
   *
   * @return $content
   *   An entity view rendered as HTML
   */
  function ds_views_row_render_entity($view_mode, $row) {
    $row_function = 'ds_views_row_render_' . $this->base_table;
    $content = $row_function($this->entities[$row->{$this->field_alias}], $view_mode);
    return $content;
  }
}

/**
 * Render the node through the entity plugin.
 */
function ds_views_row_render_node($entity, $view_mode) {
  return drupal_render(node_view($entity, $view_mode));
}

/**
 * Render the comment through the entity plugin.
 */
function ds_views_row_render_comment($entity, $view_mode) {
  $node = node_load($entity->nid);
  return drupal_render(comment_view($entity, $node, $view_mode));
}

/**
 * Render the user through the entity plugin.
 */
function ds_views_row_render_users($entity, $view_mode) {
  return drupal_render(user_view($entity, $view_mode));
}

/**
 * Render the taxonomy term through the entity plugin.
 */
function ds_views_row_render_taxonomy_term_data($entity, $view_mode) {
  return drupal_render(taxonomy_term_view($entity, $view_mode));
}
