

--- README  -------------------------------------------------------------

Field Slideshow

Provides a Slideshow format for displaying Image and Media fields, using the JQuery Cycle plugin.

Compared to Views slideshows, building the slideshow from multiple nodes, this module builds it from a single node, if you're using a multi-valued Image field.


--- INSTALLATION & USAGE-------------------------------------------------

1 - Extract the archive into sites/{sitename}/modules directory
2 - Enable Field Slideshow at /admin/modules, (Image, Field, and Field UI required as well)
3 - Create or edit a content type at /admin/structure/types and include an Image field. Edit this image field to make it so that multiple image files may be added ("Number of values" setting at admin/structure/types/manage/{content type}/fields/{field_image}).
4 - Go to "Manage display" for your content type (/admin/structure/types/manage/{content type}/display) and switch the format of your multiple image field from Image to Slideshow.
5 - Click the settings wheel in the slideshow-formatted multple image field to edit advanced settings
6 - Save! and here you go.


--- AVAILABLE OPTIONS ---------------------------------------------------

Image style
Caption
Link
Transition effect
Transition speed
Timeout
Order
Pager (numbers, thumbnails, or Carousel)
Prev/Next controls
Pause on hover

Integration with Colorbox.



Written by Jerome Danthinne
http://www.grincheux.be
