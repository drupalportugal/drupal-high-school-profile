<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $GLOBALS['language']->language; ?>" xml:lang="<?php print $GLOBALS['language']->language; ?>">

<head>
  <title><?php print t('File viewer'); ?></title>
  <?php
      $filepath = fileviewer_path($variables['file']->fid);
      $pdf_folder = file_create_url(file_build_uri(variable_get('fileviewer_path', 'fileviewer') . '/' . $file->fid));
      drupal_add_js(array('fileviewer' => array('pdfFolder' => $pdf_folder)), 'setting');
      drupal_add_js(array('fileviewer' => array('filename' => $file->filename)), 'setting');
      drupal_add_js(array('fileviewer' => array('filepath' => file_create_url($file->uri))), 'setting');
      $page = count(file_scan_directory($filepath, '/.*\.png/'));
      drupal_add_js(array('fileviewer' => array('page' => $page)), 'setting');
      global $base_url;
//      drupal_add_js(array('fileviewer' => array('imagesBaseURL' => $base_url . '/' . drupal_get_path('module', 'fileviewer') . '/BookReader/images')), 'setting');
      drupal_add_css(drupal_get_path('module', 'fileviewer') . '/BookReader/BookReader.css');
      drupal_add_css(drupal_get_path('module', 'fileviewer') . '/fileviewer.css');
      drupal_add_js(drupal_get_path('module', 'fileviewer') . '/BookReader/jquery-ui-1.8.5.custom.min.js');
      drupal_add_js(drupal_get_path('module', 'fileviewer') . '/BookReader/dragscrollable.js');
      drupal_add_js(drupal_get_path('module', 'fileviewer') . '/BookReader/jquery.colorbox-min.js');
      drupal_add_js(drupal_get_path('module', 'fileviewer') . '/BookReader/jquery.bt.min.js');
      drupal_add_js(drupal_get_path('module', 'fileviewer') . '/BookReader/BookReader.js');
      drupal_add_js(drupal_get_path('module', 'fileviewer') . '/js/bookreader.js', array('scope' => 'footer'));
  ?>
  <?php print drupal_get_html_head(); ?>
  <?php print drupal_get_css(); ?>
  <?php $vars['scripts'] = drupal_get_js('header'); ?>
  <?php function_exists('jquery_update_preprocess_page') && jquery_update_preprocess_page($vars); ?>
  <?php print $vars['scripts']; ?>
  <style media="all" type="text/css">/*Quick-override*/</style>
</head>

<body class="fileviwer">
  <div id="filevieer-messages"><?php print theme('status_messages'); ?></div>
  <div class="" style="background-color: ##939598;">
    <div id="BookReader">
    </div>
  </div>
  <?php print drupal_get_js('footer'); ?>
</body>

</html>
