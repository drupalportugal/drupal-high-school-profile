<?php
// $Id: imce-page.tpl.php,v 1.9 2010/03/17 20:55:38 ufku Exp $
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $GLOBALS['language']->language; ?>" xml:lang="<?php print $GLOBALS['language']->language; ?>">

<head>
  <title><?php print t('File viewer'); ?></title>
  <script type="text/javascript">
    var epub_dir = "<?php print file_create_url(file_build_uri(variable_get('fileviewer_path', 'fileviewer') . '/' . $file->fid));?>";
  </script>
  <?php drupal_add_js(drupal_get_path('module', 'fileviewer') . '/epubjs/js/jquery-1.3.2.min.js');?>
  <?php drupal_add_js(drupal_get_path('module', 'fileviewer') . '/epubjs/js/jquery-ui-1.7.1.custom.min.js');?>
  <?php drupal_add_js(drupal_get_path('module', 'fileviewer') . '/epubjs/js/mousewheel.js');?>
  <?php drupal_add_css(drupal_get_path('module', 'fileviewer') . '/epub/css/epubjs.css', array('scope' => 'footer'));?>
  <?php drupal_add_css(drupal_get_path('module', 'fileviewer') . '/epub/css/jquery-ui-1.7.1.custom.css');?>

  <?php drupal_add_js(drupal_get_path('module', 'fileviewer') . '/epubjs/js/epubjs.js', array('scope' => 'footer'));?>
  <?php print drupal_get_html_head(); ?>
  <?php $vars['scripts'] = drupal_get_js('header'); ?>
  <?php function_exists('jquery_update_preprocess_page') && jquery_update_preprocess_page($vars); ?>
  <?php print $vars['scripts']; ?>
  <style media="all" type="text/css">/*Quick-override*/</style>
</head>

  <body>
    <div id="book" class="clear">
      <div id="toc-container"><ol id="toc"></ol></div>
      <h1 id="content-title" ></h1>
      <div id="total-size"><div id="remaining"></div></div>
      <div id="content"></div>
    </div>
    <div id="info">
    <p>Usage: 'n' for next page, 'p' for previous page, 'j' for next chapter, 'k' for previous chapter</p>
    <p>Current support: Firefox, Safari, IE 7. As of April 7, 2009.</p> 
    <p>Resizable window, see lower-right corner. Mousewheel support to paginate forward and back.</p>
    <p>Available from <a href="http://code.google.com/p/epub-tools/source/browse/#svn/trunk/epubtools/epubjs">Google Code</a></p> 
    </div>
    <?php print drupal_get_js('footer'); ?>
    <?php print drupal_get_css(); ?>
  </body>
</html>  
