<?php
function fileviewer_page($fid) {
  $file = file_load($fid);
  $filename = '' . $file->filename;
  $ext = strtolower(substr($filename, strrpos($filename, '.')+1));
  $variables['file'] = $file;
  switch ($ext) {
    case 'pdf':
      print theme('bookreader_viewer', $variables);
      exit();
//    case 'epub':
//      print theme('epubjs_viewer', $variables);
/*
      $filepath = file_create_url(file_build_uri(variable_get('fileviewer_path', 'fileviewer') . '/' . $file->fid));
      drupal_add_js(array('fileviewer' => array('epubFolder' => $filepath)), 'setting');
      drupal_add_js(drupal_get_path('module', 'fileviewer') . '/js/epubjs.js', array('scope' => 'footer'));
      drupal_add_css(drupal_get_path('module', 'fileviewer') . '/fileviewer.css');
      $content = '<iframe name="epubreader" id="epubreader">';
      $content .= '</iframe>';
      return $content;
*/
//      exit();
  }
//  drupal_set_message(t('This file format is not supported!'));
//  print theme('message');
  print theme('file_link', $variables);
  exit();
}
