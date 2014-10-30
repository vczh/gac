# Gaclib 0.7.5.0


GPU Accelerated C++ User Interface/Script Engine (using MS-PL license)

Website for this project: http://www.gaclib.net/

This repository contains the latest release for the GacUI project, including packed library source codes, development tools, demos and documents. Complete source code can be accessed in http://gac.codeplex.com/ .

Here is a simple description to the code
* **GacUIDemo** Demo solution and projects
* **Public** 
    * **Source** GacUI library
    * **Document** HTML document. Please start at reference_gacui.html
    * **Content** Necessary CSS/JPG files for document

* New Features
    * Add supporting about mouse scrolling on some content controls.
    * Add the "Alt" property on controls so that you can press [ALT] and then press a series of letter keys to navigate to your target control. The navigation is multiple level, if you navigate into tab pages or menu items.
    * Supports compressable binary format to serialize the XML resource for a lot of performance improving.
    * When you are using GacUI under Windows, you have a chance to handle window messages.
    * Add control template for tooltip control.

Improvements to the previous release
* Fix Bugs
    * Fix the Direct2D lost device problem.
    * Fix a GuiDocumentViewer editing bug.