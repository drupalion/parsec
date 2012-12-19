<?php

/**
* @file
* Provide views data and handlers for date fields.
*/

/*
 * Implements hook_views_data_alter().
 * 
 * Checks to see if field is if type 'date', If so, Change it's handler from
 * default date handler to handler defiened by datex. In this handler, Date will
 * be converted to Jalali format.
 */
function datex_views_data_alter(&$data) {
  foreach ($data as $table => $config) {
    foreach ($config as $item => $item_config) {
      if (isset($item_config['field']['handler']) && $item_config['field']['handler'] == 'views_handler_field_date') {
        $data[$table][$item]['field']['handler'] = 'views_handler_field_datex';
      }
    }
  }
 
  return $data;
}
