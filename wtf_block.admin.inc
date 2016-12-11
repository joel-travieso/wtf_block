<?php
/**
 * @file
 * Logic for the module functionality.
 */

/**
 * Returns the render array for the wtf_block form.
 */
function wtf_block_config_form($form, &$form_state) {
  global $theme_key;
  module_load_include('inc', 'block', 'block.admin');
  //Get default and admin themes, and previously stored configurations
  $default_theme = variable_get('theme_default');
  $admin_theme = variable_get('admin_theme');
  $current_config = variable_get('wtf_block_region');
  //Create a container to wrap all the region inputs, so we have a submit value ready to be sent to db without processing
  $form['container'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
  );
  //Region selection inputs for each theme
  foreach (list_themes() as $theme_key => $theme_info) {
    $is_default = $theme_key == $default_theme;
    $is_admin = $theme_key == $admin_theme;
    $form['container']['container_' . $theme_key] = array(
      '#type' => 'fieldset', 
      '#title' => $theme_info->info['name'] . ($is_default ? ' (Default)' : ($is_admin ? ' (Administrative)' : '')),
      '#weight' => 5, 
      '#collapsible' => TRUE, 
      '#collapsed' => !$is_admin && !$is_default,
      '#weight' => $is_default ? -1 : (int)!$is_admin,
    );
    $form['container']['container_' . $theme_key]['region'] = array(
      '#type' => 'select',
      '#title' => t('Region'),
	    '#description' => t('The region where all blocks will go.'), 
      '#options' => array_merge(array('0' => 'No region'), system_region_list($theme_key, REGIONS_VISIBLE)) ,
      '#default_value' => isset($current_config['container_' . $theme_key]['region']) ? $current_config['container_' . $theme_key]['region'] : 0,
    );

	  //Get all existent blocks
	  $blocks = block_admin_display_prepare_blocks($theme_key);
	  $blocks_info = array();
	  foreach ($blocks as $block) {
	    $blocks_info[$block['bid']] = $block['info'];
	  }
	  $form['container']['container_' . $theme_key]['whitelist'] = array(
	    '#type' => 'fieldset', 
	    '#title' => t('Whitelist'), 
	    '#description' => t('Blocks that can bypass the WTF configuration.'), 
	    '#collapsible' => TRUE, 
	    '#collapsed' => TRUE,
	    '#weight' => 2,
      '#states' => array(
        'invisible' => array('select[name="container[container_' . $theme_key . '][region]"]' => array('value' => 0)),
      ),
	  );
	  //Block checkboxes
	  $form['container']['container_' . $theme_key]['whitelist']['blocks'] = array(
	    '#type' => 'checkboxes',
	    '#title' => t('Choose:'),
	    '#options' => $blocks_info,
	    '#default_value' => $current_config['container_' . $theme_key]['whitelist']['blocks'],
	  );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 3,
  );
  return $form;
}

/**
 * Submission handler for the wtf_block_config_form form.
 */
function wtf_block_config_form_submit($form, &$form_state) {
  //We don't need to do anything extra thanks to the way the form fields are built, just store
  variable_set('wtf_block_region', $form_state['values']['container']);
}
