
(function($) {

Drupal.DisplaySuite = Drupal.DisplaySuite || {};
Drupal.DisplaySuite.fieldopened = '';
  
/**
 * Field template.
 */
Drupal.behaviors.settingsToggle = {
  attach: function (context) {  
  
    // Remove click from link.
    $('.ft-link').click(function(e) {
      e.preventDefault();
    });

    // Bind update button.
    $('#field-display-overview .ft-update').click(function() {
      
      // Close the settings.
      var settings = $(this).parents('.field-template');
      settings.hide();
      $(this).parents('tr').removeClass('field-formatter-settings-editing');
      
      // Check the label.
      var row = $(this).parents('tr');
      var label = $('.label-change', settings).val();
      var original = $('.original-label', row).val();
      if (label != '') {
        new_label = label + ' (Original: ' + original + ')<input type="hidden" class="original-label" value="' + original + '">';
        $('.field-label-row', row).html(new_label);
      }
      else {
        new_label = original + '<input type="hidden" class="original-label" value="' + original + '">';
        $('.field-label-row', row).html(new_label);       
      }
      return false;
    });   
    
    // Bind on field template select button.
    $('.ds-extras-field-template').change(function() {
      ds_show_expert_settings(this);
    });
    
    // Add click event to field settings link.
    $('.ft-link').click(function() {
      
      $(this).parents('tr').siblings().removeClass('field-formatter-settings-editing');
      $(this).parents('tr').addClass('field-formatter-settings-editing');
      
      var settings = $(this).siblings('.field-template');
      if (Drupal.DisplaySuite.fieldopened != '' && Drupal.DisplaySuite.fieldopened != settings.attr('id')) {
        $('#' + Drupal.DisplaySuite.fieldopened).hide();
      }

      if (settings.is(':visible')) {
        $(this).parents('tr').removeClass('field-formatter-settings-editing');
        settings.hide();
      }
      else {
        // Slide down.
        ds_show_expert_settings(settings, true);
        settings.slideDown('normal');
      }
      // Store the opened setting.
      Drupal.DisplaySuite.fieldopened = settings.attr('id');
    });
    
    // Show / hide settings on field template form.
    function ds_show_expert_settings(element, open) {
      if (undefined == open) {
        var field = $(element).parents('.field-template');
      }
      else {
        field = element;
      }
      ft = $('.ds-extras-field-template', field).val();
      if (ft == 'theme_ds_field_expert') {
        // Show second and third label.
        if ($('.lb .form-item:nth-child(1)', field).is(':visible')) {
          $('.lb .form-item:nth-child(2), .lb .form-item:nth-child(3)', field).show();
        }
        // Remove margin from update button.
        $('.ft-update', field).css({'margin-top': '-10px'});
        // Show wrappers.
        $('.ow, .fis, .fi', field).show();
      }
      else {
        // Hide second and third label.
        $('.lb .form-item:nth-child(2), .lb .form-item:nth-child(3)', field).hide();
        // Add margin on update button.
        $('.ft-update', field).css({'margin-top': '10px'});
        // Hide wrappers.
        $('.ow, .fis, .fi', field).hide();
      }
      
      // Styles.
      if (ft != 'theme_ds_field_expert' && ft != 'theme_ds_field_reset') {
        $('.field-styles', field).show();
      }
      else {
        $('.field-styles', field).hide();        
      }
    }
  }
};
  
/**
 * Row handlers for the 'Manage display' screen.
 */
Drupal.fieldUIDisplayOverview = Drupal.fieldUIDisplayOverview || {};

Drupal.fieldUIDisplayOverview.ds = function (row, data) {

  this.row = row;
  this.name = data.name;
  this.region = data.region;
  this.tableDrag = data.tableDrag;

  this.$regionSelect = $('select.ds-field-region', row);
  this.$regionSelect.change(Drupal.fieldUIOverview.onChange);

  return this;
};

Drupal.fieldUIDisplayOverview.ds.prototype = {

  /**
   * Returns the region corresponding to the current form values of the row.
   */
  getRegion: function () {
    return this.$regionSelect.val();
  },

  /**
   * Reacts to a row being changed regions.
   *
   * This function is called when the row is moved to a different region, as a
   * result of either :
   * - a drag-and-drop action 
   * - user input in one of the form elements watched by the
   *   Drupal.fieldUIOverview.onChange change listener.
   *
   * @param region
   *   The name of the new region for the row.
   * @return
   *   A hash object indicating which rows should be AJAX-updated as a result
   *   of the change, in the format expected by
   *   Drupal.displayOverview.AJAXRefreshRows().
   */
  regionChange: function (region) {

    this.$regionSelect.val(region);    

    var refreshRows = {};
    refreshRows[this.name] = this.$regionSelect.get(0);
    
    return refreshRows;
  },
};

})(jQuery);
