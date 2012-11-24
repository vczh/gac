# Gaclib 0.4.1.0


GPU Accelerated C++ User Interface/Script Engine

Website for this project: http://www.gaclib.net/

This is a mirror from http://gac.codeplex.com/

Here is a simple description to the code
* **GacUIDemo** Demo solution and projects
* **Public** 
    * **Source** GacUI library
    * **Document** HTML document. Please start at reference_gacui.html
    * **Content** Necessary CSS/JPG files for document

Improvements to the previous release
* Added 4 new features

    * **GuiSolidLabelElement** measures height when wrap line

    * **GuiGDIElement** enables programmers to directly access GDI resources in a window

    * **GuiDirect2DElement** enables programmers to directly access Direct2D resource in a window

    * **GuiDocumentElement** enables programmers to render rich text with embedded images

* Fixed bugs

    * Fixed bug about compositions with wrap line labels

    * Fixed but about multiple window disposing

    * Fixed Template.Window.CustomizedBorder flicking

* Added 1 new demo

    * Rendering.TextLayout.RichText