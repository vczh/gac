# Gaclib 0.4.2.0


GPU Accelerated C++ User Interface/Script Engine

Website for this project: http://www.gaclib.net/

This is a mirror for the release from http://gac.codeplex.com/

Here is a simple description to the code
* **GacUIDemo** Demo solution and projects
* **Public** 
    * **Source** GacUI library
    * **Document** HTML document. Please start at reference_gacui.html
    * **Content** Necessary CSS/JPG files for document

Improvements to the previous release
* Added 1 new demo

    * **Controls.TextBox.AdvancedColorizer** that shows how to write a text code with context sensitive
* Added new features

    * x64 supports
    * Linq to C++
    * Linq to Xml
    * Linq to Json
    * Parser framework and generator (generate parser cpp code from a grammar definition)
    * Locale awared functions, like compare string with locale, format string with locale, etc
    * Test buffer encoding, that can test a byte buffer and try to guess the encoding of the text as accurately as possible
* Fixed bugs

    * Close main form using GuiWindow::Close() will not exit the application