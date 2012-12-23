<?php

/**
 * Provides administration form for datex module.
 * @return drupal form
 */
function datex_api_menu_callback() {
  $form = array();

  $form['datex_api_intl'] = array(
    '#type' => 'radios',
    '#title' => t('Date conversion method'),
    '#default_value' => variable_get('datex_api_intl', 0),
    '#description' => t('Which method datex should use. Intl is experimental'),
    '#options' => array(1 => 'Use INTL', 0 => t('Use datex internal methods.')),
  );

  return system_settings_form($form);
}
