(function($) {
  Drupal.behaviors.field_slideshow = {
    attach: function(context) {

      for (i in Drupal.settings.field_slideshow) {
        var settings = Drupal.settings.field_slideshow[i];
        var slideshow = $('.' + i);

        if (!slideshow.hasClass('field-slideshow-processed')) {
          slideshow.addClass('field-slideshow-processed');

          // Add padding if needed
          var max_outerWidth = 0;
          var max_outerHeight = 0;
          $('.field-slideshow-slide', slideshow).each(function() {
            $this = $(this);
            max_outerWidth = Math.max(max_outerWidth, $this.outerWidth(true));
            max_outerHeight = Math.max(max_outerHeight, $this.outerHeight(true));
          });
          slideshow.css({
            'padding-right': (max_outerWidth - parseInt(slideshow.css('width'))) + 'px',
            'padding-bottom': (max_outerHeight - parseInt(slideshow.css('height'))) + 'px'
          });

          // Add options
          var options = {
            resizing: 0,
            fx: settings.fx,
            speed: settings.speed,
            timeout: parseInt(settings.timeout)
          }

          if (settings.speed == "0" && settings.timeout == "0") options.fastOnEvent = true;
          if (settings.controls) {
            options.prev = "#" + i + "-controls .prev";
            options.next = "#" + i + "-controls .next";
          }
          if (settings.pause) options.pause = true;

          if (settings.pager != '') {
            options.pager = "#" + i + "-pager";
            if (settings.pager == 'image' || settings.pager == 'carousel') {
              options.pagerAnchorBuilder = function(idx, slide) {
                return '#' + i + '-pager li:eq(' + idx + ') a';
              };
              if (settings.pager == 'carousel') {
                var carouselops = { 
                  btnNext:  "#" + i + "-carousel-wrapper .carousel-next",
                  btnPrev:  "#" + i + "-carousel-wrapper .carousel-prev",
                  visible:  parseInt(settings.carousel_visible),
                  scroll:   parseInt(settings.carousel_scroll),
                  speed:    parseInt(settings.carousel_speed),
                  vertical: settings.carousel_vertical,
                  circular: false
                };
                $("#" + i + "-carousel").jCarouselLite(carouselops)
              }
            }
          }

          // Cycle!
          slideshow.cycle(options); 

        }

      }

    }
  }
})(jQuery);