# Gaclib 0.5.1.0


GPU Accelerated C++ User Interface/Script Engine (using MS-PL license)

Website for this project: http://www.gaclib.net/

This is a mirror for the release from http://gac.codeplex.com/

Here is a simple description to the code
* **GacUIDemo** Demo solution and projects
* **Public** 
    * **Source** GacUI library
    * **Document** HTML document. Please start at reference_gacui.html
    * **Content** Necessary CSS/JPG files for document

Improvements to the previous release
* Updated 1 demo
    * **Template.Window.CustomizedBorder** Add an icon in the window title of the skin.
* Added new features

    * Parser generator supports ambiguous context-free grammar.
    * Reflection supports dictionary type.
    * Remove the old parser combinator library completely out of Gaclib.
    * Upgrade Fpmacro to use the new parser generator.
* Fixed bugs
    * Parser generator crashes when the input grammar contains conflict errors.
    * Reflection failed to unload and reload metadata.