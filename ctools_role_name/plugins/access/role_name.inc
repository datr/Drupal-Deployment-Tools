<?php
// $Id: role.inc,v 1.7.2.3 2010/07/14 01:30:19 merlinofchaos Exp $

/**
 * @file
 * Plugin to provide access control based upon role membership.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("User: role name"),
  'description' => t('Control access by role. Uses role name instead of role id. Use this if you want to export this object to another site.'),
  'callback' => 'ctools_role_name_ctools_access_check',
  'default' => array('roles' => array()),
  'settings form' => 'ctools_role_name_ctools_access_settings',
  'settings form submit' => 'ctools_role_name_ctools_access_settings_submit',
  'summary' => 'ctools_role_name_ctools_access_summary',
  'required context' => new ctools_context_required(t('User'), 'user'),
);

/**
 * Settings form for the 'by role' access plugin
 */
function ctools_role_name_ctools_access_settings(&$form, &$form_state, $conf) {
  $form['settings']['roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Role'),
    '#default_value' => $conf['roles'],
    '#options' => ctools_role_name_get_roles(),
    '#description' => t('Only the checked roles will be granted access.'),
  );
}

/**
 * Compress the roles allowed to the minimum.
 */
function ctools_role_name_ctools_access_settings_submit(&$form, &$form_state) {
  $form_state['values']['settings']['roles'] = array_keys(array_filter($form_state['values']['settings']['roles']));
}

/**
 * Check for access.
 */
function ctools_role_name_ctools_access_check($conf, $context) {
  // As far as I know there should always be a context at this point, but this
  // is safe.
  if (empty($context) || empty($context->data) || !isset($context->data->roles)) {
    return FALSE;
  }

  $roles = array_values($context->data->roles);
  $roles[] = $context->data->uid ? 'authenticated user' : 'anonymous user';
  return (bool) array_intersect($conf['roles'], $roles);
}

/**
 * Provide a summary description based upon the checked roles.
 */
function ctools_role_name_ctools_access_summary($conf, $context) {
  if (!isset($conf['roles'])) {
    $conf['roles'] = array();
  }

  $names = array();
  foreach (array_filter($conf['roles']) as $role) {
    $names[] = $role;
  }

  if (empty($names)) {
    return t('@identifier can have any role', array('@identifier' => $context->identifier));
  }

  return format_plural(count($names), '@identifier has role "@roles"', '@identifier has one of "@roles"', array('@roles' => implode(', ', $names), '@identifier' => $context->identifier));
}

function ctools_role_name_get_roles() {
  static $roles = FALSE;
  if ($roles === FALSE) {
    foreach (ctools_get_roles() as $name) {
      $roles[$name] = $name;
    }
  }
  return $roles;
}