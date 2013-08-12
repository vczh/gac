#include "..\CommonLibrary\FileSystemInformation.h"
#include <Windows.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

const int								SystemIconSize = 24;

/***********************************************************************
CustomTemplateWindowButtonStyle
***********************************************************************/

class CustomTemplateWindowButtonStyle : public GuiButton::IStyleController
{
protected:
	GuiBoundsComposition*				boundsComposition;
public:
	CustomTemplateWindowButtonStyle(Color borderColor)
	{
		boundsComposition=new GuiBoundsComposition;
		boundsComposition->SetAssociatedHitTestResult(INativeWindowListener::Client);
		boundsComposition->SetPreferredMinSize(Size(SystemIconSize, SystemIconSize));

		GuiSolidBorderElement* borderElement=GuiSolidBorderElement::Create();
		borderElement->SetColor(borderColor);
		boundsComposition->SetOwnedElement(borderElement);
	}

	~CustomTemplateWindowButtonStyle()
	{
	}

	compositions::GuiBoundsComposition* GetBoundsComposition()
	{
		return boundsComposition;
	}

	compositions::GuiGraphicsComposition* GetContainerComposition()
	{
		return boundsComposition;
	}

	void SetFocusableComposition(compositions::GuiGraphicsComposition* value)
	{
	}

	void SetText(const WString& value)
	{
	}

	void SetFont(const FontProperties& value)
	{
	}

	void SetVisuallyEnabled(bool value)
	{
	}

	void Transfer(GuiButton::ControlState value)
	{
	}
};

/***********************************************************************
CustomTemplateWindowStyle
***********************************************************************/

class CustomTemplateWindowStyle : public GuiWindow::IStyleController
{
protected:

	GuiWindow*							window;
	GuiTableComposition*				boundsComposition;
	GuiBoundsComposition*				iconComposition;
	GuiImageFrameElement*				iconElement;
	GuiBoundsComposition*				titleComposition;
	GuiSolidLabelElement*				titleElement;
	GuiBoundsComposition*				containerComposition;
	GuiButton*							minimumButton;
	GuiButton*							maximumButton;
	GuiButton*							closeButton;

	void AddBorderCell(int row, int column, int rowSpan, int columnSpan, INativeWindowListener::HitTestResult hitTestResult)
	{
		GuiCellComposition* cell=new GuiCellComposition;
		boundsComposition->AddChild(cell);
		cell->SetSite(row, column, rowSpan, columnSpan);
		cell->SetAssociatedHitTestResult(hitTestResult);
	}

	GuiBoundsComposition* AddTitleCell(int column, INativeWindowListener::HitTestResult hitTestResult, Color titleBackgroundColor)
	{
		GuiCellComposition* cell=new GuiCellComposition;
		boundsComposition->AddChild(cell);
		cell->SetSite(1, column, 1, 1);

		GuiSolidBackgroundElement* element=GuiSolidBackgroundElement::Create();
		element->SetColor(titleBackgroundColor);
		cell->SetOwnedElement(element);

		GuiBoundsComposition* composition=new GuiBoundsComposition;
		composition->SetAlignmentToParent(Margin(0, 0, 0, 0));
		composition->SetPreferredMinSize(Size(SystemIconSize, SystemIconSize));
		composition->SetAssociatedHitTestResult(hitTestResult);
		cell->AddChild(composition);

		return composition;
	}

	GuiButton* AddTitleButton(int column, Color titleBackgroundColor, Color titleForegroundColor)
	{
		GuiCellComposition* cell=new GuiCellComposition;
		boundsComposition->AddChild(cell);
		cell->SetSite(1, column, 1, 1);

		GuiSolidBackgroundElement* element=GuiSolidBackgroundElement::Create();
		element->SetColor(titleBackgroundColor);
		cell->SetOwnedElement(element);

		GuiButton* button=new GuiButton(new CustomTemplateWindowButtonStyle(titleForegroundColor));
		button->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 3, 3, 3));
		cell->AddChild(button->GetBoundsComposition());
		return button;
	}

	void minimumButton_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		window->GetNativeWindow()->ShowMinimized();
	}

	void maximumButton_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		if(window->GetNativeWindow()->GetSizeState()==INativeWindow::Restored)
		{
			window->GetNativeWindow()->ShowMaximized();
		}
		else
		{
			window->GetNativeWindow()->ShowRestored();
		}
	}

	void closeButton_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		window->GetNativeWindow()->Hide();
	}
public:
	CustomTemplateWindowStyle()
		:window(0)
	{
		Color borderColor(0, 122, 204);
		Color titleBackgroundColor(45, 45, 48);
		Color backgroundColor(30, 30, 30);
		Color titleColor(153, 153, 153);

		boundsComposition=new GuiTableComposition;
		boundsComposition->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);
		boundsComposition->SetRowsAndColumns(4, 7);
		boundsComposition->SetRowOption(0, GuiCellOption::AbsoluteOption(5));
		boundsComposition->SetRowOption(1, GuiCellOption::MinSizeOption());
		boundsComposition->SetRowOption(2, GuiCellOption::PercentageOption(1.0));
		boundsComposition->SetRowOption(3, GuiCellOption::AbsoluteOption(5));
		boundsComposition->SetColumnOption(0, GuiCellOption::AbsoluteOption(5));
		boundsComposition->SetColumnOption(1, GuiCellOption::MinSizeOption());
		boundsComposition->SetColumnOption(2, GuiCellOption::PercentageOption(1.0));
		boundsComposition->SetColumnOption(3, GuiCellOption::MinSizeOption());
		boundsComposition->SetColumnOption(4, GuiCellOption::MinSizeOption());
		boundsComposition->SetColumnOption(5, GuiCellOption::MinSizeOption());
		boundsComposition->SetColumnOption(6, GuiCellOption::AbsoluteOption(5));
		{
			GuiSolidBackgroundElement* element=GuiSolidBackgroundElement::Create();
			element->SetColor(borderColor);
			boundsComposition->SetOwnedElement(element);
		}
		AddBorderCell(0, 0, 1, 1, INativeWindowListener::BorderLeftTop);
		AddBorderCell(0, 6, 1, 1, INativeWindowListener::BorderRightTop);
		AddBorderCell(3, 0, 1, 1, INativeWindowListener::BorderLeftBottom);
		AddBorderCell(3, 6, 1, 1, INativeWindowListener::BorderRightBottom);
		AddBorderCell(1, 0, 2, 1, INativeWindowListener::BorderLeft);
		AddBorderCell(1, 6, 2, 1, INativeWindowListener::BorderRight);
		AddBorderCell(0, 1, 1, 5, INativeWindowListener::BorderTop);
		AddBorderCell(3, 1, 1, 5, INativeWindowListener::BorderBottom);
		{
			{
				iconComposition=AddTitleCell(1, INativeWindowListener::Icon, titleBackgroundColor);
				iconComposition->SetAlignmentToParent(Margin(3, 3, 3, 3));

				Ptr<INativeImage> icon=GetCurrentController()->ImageService()->CreateImageFromFile(GetApplication()->GetExecutableFolder()+L"..\\Resources\\BigDoc.png");
				iconElement=GuiImageFrameElement::Create();
				iconElement->SetImage(icon, 0);
				iconElement->SetStretch(true);
				iconComposition->SetOwnedElement(iconElement);
			}
			{
				titleComposition=AddTitleCell(2, INativeWindowListener::Title, titleBackgroundColor);
				titleComposition->SetAlignmentToParent(Margin(0, 3, 3, 3));

				titleElement=GuiSolidLabelElement::Create();
				titleElement->SetColor(titleColor);
				titleElement->SetAlignments(Alignment::Left, Alignment::Center);
				titleComposition->SetOwnedElement(titleElement);
			}
			{
				minimumButton=AddTitleButton(3, titleBackgroundColor, titleColor);
				minimumButton->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 3, 3, 3));
				minimumButton->Clicked.AttachMethod(this, &CustomTemplateWindowStyle::minimumButton_Clicked);
			}
			{
				maximumButton=AddTitleButton(4, titleBackgroundColor, titleColor);
				maximumButton->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 3, 3, 3));
				maximumButton->Clicked.AttachMethod(this, &CustomTemplateWindowStyle::maximumButton_Clicked);
			}
			{
				closeButton=AddTitleButton(5, titleBackgroundColor, titleColor);
				closeButton->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 3, 3, 3));
				closeButton->Clicked.AttachMethod(this, &CustomTemplateWindowStyle::closeButton_Clicked);
			}
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			boundsComposition->AddChild(cell);
			cell->SetSite(2, 1, 1, 5);
			cell->SetAssociatedHitTestResult(INativeWindowListener::Client);

			containerComposition=new GuiBoundsComposition;
			containerComposition->SetAlignmentToParent(Margin(0, 0, 0, 0));
			cell->AddChild(containerComposition);

			GuiSolidBackgroundElement* element=GuiSolidBackgroundElement::Create();
			element->SetColor(backgroundColor);
			containerComposition->SetOwnedElement(element);
		}
	}

	GuiBoundsComposition* GetBoundsComposition()override
	{
		return boundsComposition;
	}

	GuiGraphicsComposition* GetContainerComposition()override
	{
		return containerComposition;
	}

	void SetFocusableComposition(compositions::GuiGraphicsComposition* value)override
	{
	}

	void SetText(const WString& value)override
	{
		titleElement->SetText(value);
		boundsComposition->ForceCalculateSizeImmediately();
	}

	void SetFont(const FontProperties& value)override
	{
	}

	void SetVisuallyEnabled(bool value)override
	{
	}

	void AttachWindow(GuiWindow* _window)override
	{
		window=_window;
	}

	void InitializeNativeWindowProperties()override
	{
		// remove all native windows features
		if(window->GetNativeWindow())
		{
			window->GetNativeWindow()->EnableCustomFrameMode();
			window->GetNativeWindow()->SetBorder(false);
		}
	}

	bool GetMaximizedBox()override
	{
		return true;
	}

	void SetMaximizedBox(bool visible)override
	{
	}

	bool GetMinimizedBox()override
	{
		return true;
	}

	void SetMinimizedBox(bool visible)override
	{
	}

	bool GetBorder()override
	{
		return true;
	}

	void SetBorder(bool visible)override
	{
	}

	bool GetSizeBox()override
	{
		return true;
	}

	void SetSizeBox(bool visible)override
	{
	}

	bool GetIconVisible()override
	{
		return true;
	}

	void SetIconVisible(bool visible)override
	{
	}

	bool GetTitleBar()override
	{
		return true;
	}

	void SetTitleBar(bool visible)override
	{
	}
};

/***********************************************************************
CustomTemplateWindow
***********************************************************************/

extern void FillData(GuiListView* listView);

class CustomTemplateWindow : public GuiWindow
{
private:
	List<Ptr<GuiWindow>>			newWindows;
	GuiButton*						buttonNewWindow;
	GuiListView*					listView;
	GuiComboBoxListControl*			comboView;

	void buttonNewWindow_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		GuiWindow* newWindow=new CustomTemplateWindow;
		newWindows.Add(newWindow);
		newWindow->Show();
	}

	void comboView_SelectedIndexChanged(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		switch(comboView->GetSelectedIndex())
		{
		case 0:
			listView->ChangeItemStyle(new list::ListViewBigIconContentProvider);
			break;
		case 1:
			listView->ChangeItemStyle(new list::ListViewSmallIconContentProvider);
			break;
		case 2:
			listView->ChangeItemStyle(new list::ListViewListContentProvider);
			break;
		case 3:
			listView->ChangeItemStyle(new list::ListViewDetailContentProvider);
			break;
		case 4:
			listView->ChangeItemStyle(new list::ListViewTileContentProvider);
			break;
		case 5:
			listView->ChangeItemStyle(new list::ListViewInformationContentProvider);
			break;
		}
	}
public:
	CustomTemplateWindow()
		:GuiWindow(new CustomTemplateWindowStyle)
	{
		this->SetText(L"Template.Window.CustomizedBorder");

		GuiTableComposition* table=new GuiTableComposition;
		table->SetCellPadding(4);
		table->SetAlignmentToParent(Margin(0, 0, 0, 0));
		table->SetRowsAndColumns(2, 3);
		table->SetRowOption(0, GuiCellOption::MinSizeOption());
		table->SetRowOption(1, GuiCellOption::PercentageOption(1.0));
		table->SetColumnOption(0, GuiCellOption::MinSizeOption());
		table->SetColumnOption(1, GuiCellOption::MinSizeOption());
		table->SetColumnOption(2, GuiCellOption::PercentageOption(1.0));
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 0, 1, 1);

			buttonNewWindow=g::NewButton();
			buttonNewWindow->SetText(L"New Window ...");
			buttonNewWindow->Clicked.AttachMethod(this, &CustomTemplateWindow::buttonNewWindow_Clicked);

			cell->AddChild(buttonNewWindow->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 1, 1, 1);

			GuiTextList* comboSource=g::NewTextList();
			comboSource->GetItems().Add(new list::TextItem(L"Big Icon"));
			comboSource->GetItems().Add(new list::TextItem(L"Small Icon"));
			comboSource->GetItems().Add(new list::TextItem(L"List"));
			comboSource->GetItems().Add(new list::TextItem(L"Detail"));
			comboSource->GetItems().Add(new list::TextItem(L"Tile"));
			comboSource->GetItems().Add(new list::TextItem(L"Information"));
			comboSource->SetHorizontalAlwaysVisible(false);

			comboView=g::NewComboBox(comboSource);
			comboView->SetSelectedIndex(0);
			comboView->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, -1, 0));
			comboView->GetBoundsComposition()->SetPreferredMinSize(Size(160, 0));
			comboView->SelectedIndexChanged.AttachMethod(this, &CustomTemplateWindow::comboView_SelectedIndexChanged);
			cell->AddChild(comboView->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(1, 0, 1, 3);

			listView=g::NewListViewBigIcon();
			listView->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			listView->SetHorizontalAlwaysVisible(false);
			listView->SetVerticalAlwaysVisible(false);
			listView->SetMultiSelect(true);
			cell->AddChild(listView->GetBoundsComposition());
		}
		this->GetContainerComposition()->AddChild(table);
		FillData(listView);

		// set the preferred minimum client size
		this->GetBoundsComposition()->SetPreferredMinSize(Size(640, 480));
		// call this to calculate the size immediately if any indirect content in the table changes
		// so that the window can calcaulte its correct size before calling the MoveToScreenCenter()
		this->ForceCalculateSizeImmediately();
		// move to the screen center
		this->MoveToScreenCenter();
	}

	~CustomTemplateWindow()
	{
	}
};

/***********************************************************************
FillData
***********************************************************************/

void FillList(GuiListView* listView, const WString& path, List<WString>& files)
{
	// Fill all information about a directory or a file.
	FOREACH(WString, file, files)
	{
		Ptr<list::ListViewItem> item=new list::ListViewItem;
		WString fullPath=path+L"\\"+file;

		// Get large icon.
		item->largeImage=GetFileIcon(fullPath, SHGFI_LARGEICON | SHGFI_ICON);
		// Get small icon.
		item->smallImage=GetFileIcon(fullPath, SHGFI_SMALLICON | SHGFI_ICON);
		// Get display name
		item->text=GetFileDisplayName(fullPath);
		// Get type name
		item->subItems.Add(GetFileTypeName(fullPath));
		// Get last write time
		item->subItems.Add(FileTimeToString(GetFileLastWriteTime(fullPath)));
		// Get file size
		item->subItems.Add(FileSizeToString(GetFileSize(fullPath)));

		listView->GetItems().Add(item);
	}
}

void FillData(GuiListView* listView)
{
	// Get the Windows directory, normally L"C:\Windows".
	wchar_t folderPath[MAX_PATH]={0};
	HRESULT hr=SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, 0, folderPath);
	if(FAILED(hr)) return;

	// Enumerate all directories and files in the Windows directory.
	List<WString> directories;
	List<WString> files;
	SearchDirectoriesAndFiles(folderPath, directories, files);

	// Set all columns. The first column is the primary column. All others are sub columns.
	listView->GetItems().GetColumns().Add(new list::ListViewColumn(L"Name", 230));
	listView->GetItems().GetColumns().Add(new list::ListViewColumn(L"Type", 120));
	listView->GetItems().GetColumns().Add(new list::ListViewColumn(L"Date", 120));
	listView->GetItems().GetColumns().Add(new list::ListViewColumn(L"Size", 120));

	// Set all data columns (important sub solumns). The first sub item is 0. The primary column is not counted in.
	listView->GetItems().GetDataColumns().Add(0);	// Type
	listView->GetItems().GetDataColumns().Add(1);	// Data

	// Fill all directories and files into the list view
	FillList(listView, folderPath, directories);
	FillList(listView, folderPath, files);
}

/***********************************************************************
GuiMain
***********************************************************************/

void GuiMain()
{
	GuiWindow* window=new CustomTemplateWindow;
	GetApplication()->Run(window);
	delete window;
}