VirtualTouchScreen
=============
A camera based application to have full control of mouse using hand gesture base on yellow colour tracking.

-----------------------------------------------------------------

Requirement
-----------
-OpenCV 2.3.1
-Visual Studio 2010


Setup Configuration
-------------------
-add these directories inside the project properties (C/C++\General\Additional Include Directories) 
	<dir..>\opencv\build\include\opencv
	<dir..>\VirtualTouchscreen\VirtualTouchScreen(cvBlobs library)
	<dir..>\opencv\build\include
-add these linker library inside the project properties (Linker\General\Additional Library Directories)
	<dir..>\opencv\build\x86\vc12\lib
-add these lib insede the project properties (Linker\Input\Additional Dependencies)
	opencv_core231d.lib
	opencv_imgproc231d.lib
	opencv_highgui231d.lib
	opencv_ml231d.lib
	opencv_video231d.lib
	tbb_debug.lib
	opencv_features2d231d.lib
	opencv_calib3d231d.lib
	opencv_objdetect231d.lib
	opencv_contrib231d.lib
	opencv_legacy231d.lib
	opencv_flann231d.lib
-add these inside the system environment variable
	<dir..>\opencv\build\include\x86\vc10\bin


Git Basic
---------
- Create an [GitHub](https://github.com/) account then install [GitHub Windows Client](http://windows.github.com/).
- Execute sync before you start work on your project.
- Commit then push or sync to server after make changes. Practice small commit as you can for each changes so that you able to rollback at any point.


Useful Tools
-------------
- [Notepad++](http://notepad-plus-plus.org/) - A powerful lightweight  notepad almost can parse all programming languages if you only want to have a quick view or modification.
- [MarkPad](http://code52.org/DownmarkerWPF/) - An editor for markdown language (readme).


References
------------
- [OpenCV](http://opencv.org/)
- [Tracking Objects Using Contour](http://8a52labs.wordpress.com/category/opencv/)