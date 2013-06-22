# Gaclib 0.5.2.0


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
* Add 4 demos
    * **Controls.DataGrid.ChemicalElements** This demo shows how to display data in a GuiVirtualDataGrid control using different styles for different cells.
    * **Controls.DataGrid.FileExplorer** This demo shows how to use GuiVirtualDataGrid with IStructuredDataProvider to do strong typed sorting and filtering.
    * **Controls.DataGrid.TableEditor** This demo shows how to use GuiStringGrid to perform simple table editing.
    * **Controls.DatePicker.DateAndLocale** This demo shows how to use GuiDatePicker and GuiDateComboBox to select a date under different locales and date formats.
* Added new features
    * **VirtualDateGrid**
    * **StringGrid**
    * **DatePicker**
    * **DateComboBox**
* Fixed bugs
    * Direct2D renderer uses the wrong scale under larget DPI setting
    * GetTreeViewData crashes in tree view control
    * Change the CheckBox graph because some users do not have "Windings" font