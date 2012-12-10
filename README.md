# External Library Dependency
This project uses ImageMagick (version 6.8.0-4).

# How to Compile and Run

Download the version of ImageMagick specified above from their website [ImageMagick: Downloads](http://www.imagemagick.org/script/download.php) and make a symbolic link with the name imagemagick like so : 

`ln -s imagemagick dir/to/downloaded/ImageMagick`

Then, you can run `make`, which compiles and run the air hocky game. If you simply compile and run the airhocky it cannot run since the program uses the dynamic library, which needs environmental variables to be properly set up.
