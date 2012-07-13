# Gaclib 0.2.5.0


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
* Added 2 new demos 
    * Controls.Toolstrip.TextEditor
    * Controls.TreeView.FileExplorer
* Fixed bugs
* Encapsulated OS's common dialog into INativeDialogService
* Added undo/redo to text box
* Add item/node related events to list control and treeview control
* Refactor the whole website using ASP.NET MVC3 hosted in Windows Azure (the source code of the website is checked in codeplex, but not included in the release)