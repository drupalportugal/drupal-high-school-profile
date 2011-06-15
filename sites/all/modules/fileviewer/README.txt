IMPORTANT: You need to install Poppler (http://poppler.freedesktop.org/, poppler-utils package in ubuntu for example) to convert PDF file into PNGs. The Poppler utils package converts the pdf into png images that is then displayed by the Internet Archive BookReader. If you are running your drupal installation in your own computer (i.e. a localhost installation) installing poppler utils in your computer will allow this conversion to proceed without a hitch. However you may need to contact the server adminstrator to install poppler utils if you are serving the site from a hosting provider. In case of a virtual server hosting environment you may be able to install the poppler-utils package yourself though. 

Installation:
Download the tar.gz or zip file and unzip it. Subsequently paste the folder fileviewer into the sites/all/modules folder. If you are using drupal 7 you can use the install module link at the admin/modules page in your drupal website where you can either browse to the downloaded archive (zip / tar.gz) or the link to archive from http://drupal.org/project/fileviewer page.

Usage:
In order to enable the display of the file using the fileviewer the following steps are required:
1. Go to admin/structure to access the structure page in your site. In drupal 7 clicking on the structure link on the black bar at the top of the page or the structure link in the adminstration page will take you to this page.
2. Click on content types to access your content types.
3. You will see a list of existing content types. Scroll to the content you want to modify.
4. Click on the manage fields link beside the content type you want to modify
5. Add a new field of the type file (from the select field type dropdown). Name the field to what you want. Clickin(Check http://drupal.org/node/1048922#comment-4047766). g on save will then take you to a form where you can edit the setting of the field. Make sure that you limit the allowed file types to pdf and epub if you want the embedded file feature to work else you can have ppt and tiff in addition to pdf. If you want more than one file to be attached just make the appropiate changes in the number of values dropdown. 
6. After you save these changes you will need to go to the manage display tab to actually start viewing the file you have uploaded using the embedded fileviewer. 
7. In the manage display you will find the beside the field you just created a dropdown menu. The options in the dropdown will be generic file, fileviewer, embed fileviewer, google docs viewer etc. 
8. If you have made sure that users can only upload a single PDF file then u can safely use the embed fileviewer option else its best to go with the fileviewer option (Check http://drupal.org/node/1048922#comment-4047766). 
9. The google docs viewer will add a view in google docs link beside your file that will take your user to view the file in the google doc viewer. It can handle pdf, ppt and tiff files at present. The google docs viewer option is good if you cant get poppler utils installed in the webserver you are hosting your website on.

For Drupal 7, only "Fileviewer", "Embed fileviewer" and "Google Docs Viewer" were provided for file field at DRUPAL_HOME/admin/structure/types/manage/*/display. The default option "Fileviewer" is recommended, because "Embed fileviewer" didn't support multiple file fields (Check http://drupal.org/node/1048922#comment-4047766). 

For Drupal 6, only "View with view fileviewer" was provided for file field at DRUPAL_HOME/admin/content/node-type/*/display. For Drupal 6, Internet Archive BookReader should be downloaded by yourself and extract the "BookReader" directory into sites/all/libraries.

