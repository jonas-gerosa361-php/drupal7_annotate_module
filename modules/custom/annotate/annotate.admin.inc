<?php

/**
 * @file
 *  Administration page callbacks for the annotate module.
 */

 /**
  * Form builder. Configure annotations.
  *
  * @ingroup forms
  * @see system_settings_form().
  */
function annotate_admin_settings() {
  $types = node_type_get_types();
  foreach ($types as $node_type) {
    $options[$node_type->type] = $node_type->name;
  }

  $form['annotate_node_types'] = [
    '#type' => 'checkboxes',
    '#title' => t('Users may annotate these content types'),
    '#options' => $options,
    '#default_value' => variable_get('annotate_node_types', ['page']),
    '#description' => t('A text field will be available on these content types to make user-specific notes.'),
  ];

  $form['#submit'][] = 'annotate_admin_settings_submit';
  return system_settings_form($form);
}

/**
 * Process annotation settings submission.
 */
function annotate_admin_settings_submit($form, $form_state) {
  foreach ($form_state['values']['annotate_node_types'] as $key => $value) {
    if (!$value) {
      $instance = field_info_instance('node', 'annotation', $key);
      if (!empty($instance)) {
        field_delete_instance($instance);
        watchdog('Annotation', 'Deleted annotation field from content type: %key', ['%key' => $key]);
      }
    }
    else {
      $instance = field_info_instance('node', 'annotation', $key);
      if (empty($instance)) {
        $instance = [
          'field_name' => 'annotation',
          'entity_type' => 'node',
          'bundle' => $key,
          'label' => t('Annotation'),
          'widget_type' => 'text_textarea_with_summary',
          'settings' => ['display_summary' => TRUE],
          'display' => [
            'default' => [
              'type' => 'text_default',
            ],
          ],
          'teaser' => [
            'type' => 'text_summary_or_timmed',
          ],
        ];
      }
      $instance = field_create_instance($instance);
      watchdog('Annotation', 'Added annotation field to content type: %key', ['%key' => $key]);
    }
  }
}
