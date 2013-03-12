#include "..\CommonLibrary\FileSystemInformation.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

/***********************************************************************
DataSource
***********************************************************************/

class DataSource : public list::ItemProviderBase, public list::ListViewItemStyleProvider::IListViewItemView, public list::ListViewColumnItemArranger::IColumnItemView
{
	typedef List<list::ListViewColumnItemArranger::IColumnItemViewCallback*>		ColumnItemViewCallbackList;
private:
	List<Ptr<FileProperties>>		fileProperties;
	ColumnItemViewCallbackList		columnItemViewCallbacks;
	int								columnSizes[4];
public:
	DataSource()
	{
		for(int i=0;i<4;i++)
		{
			columnSizes[i]=i==0?230:120;
		}
	}

	void SetDirectory(const WString& directory)
	{
		int oldCount=fileProperties.Count();
		fileProperties.Clear();
		
		// Enumerate all directories and files in the Windows directory.
		List<WString> directories;
		List<WString> files;
		SearchDirectoriesAndFiles(directory, directories, files);
		FOREACH(WString, file, From(directories).Concat(files))
		{
			fileProperties.Add(new FileProperties(directory+L"\\"+file));
		}

		InvokeOnItemModified(0, oldCount, fileProperties.Count());
	}

	// GuiListControl::IItemProvider

	int Count()
	{
		return fileProperties.Count();
	}

	IDescriptable* RequestView(const WString& identifier)
	{
		if(identifier==list::ListViewItemStyleProvider::IListViewItemView::Identifier)
		{
			return this;
		}
		else if(identifier==list::ListViewColumnItemArranger::IColumnItemView::Identifier)
		{
			return this;
		}
		else
		{
			return 0;
		}
	}

	void ReleaseView(IDescriptable* view)
	{
	}

	// GuiListControl::IItemPrimaryTextView

	WString GetPrimaryTextViewText(int itemIndex)
	{
		return GetText(itemIndex);
	}

	bool ContainsPrimaryText(int itemIndex)
	{
		return true;
	}

	// list::ListViewItemStyleProvider::IListViewItemView
	
	Ptr<GuiImageData> GetSmallImage(int itemIndex)
	{
		if(0<=itemIndex && itemIndex<fileProperties.Count())
		{
			return fileProperties[itemIndex]->GetSmallIcon();
		}
		return 0;
	}

	Ptr<GuiImageData> GetLargeImage(int itemIndex)
	{
		if(0<=itemIndex && itemIndex<fileProperties.Count())
		{
			return fileProperties[itemIndex]->GetBigIcon();
		}
		return 0;
	}

	WString GetText(int itemIndex)
	{
		if(0<=itemIndex && itemIndex<fileProperties.Count())
		{
			return fileProperties[itemIndex]->GetDisplayName();
		}
		return L"";
	}

	WString GetSubItem(int itemIndex, int index)
	{
		if(0<=itemIndex && itemIndex<fileProperties.Count() && 0<=index && index<3)
		{
			switch(index)
			{
			case 0:
				return fileProperties[itemIndex]->GetTypeName();
			case 1:
				return FileTimeToString(fileProperties[itemIndex]->GetLastWriteTime());
			case 2:
				return FileSizeToString(fileProperties[itemIndex]->GetSize());
			}
		}
		return L"";
	}

	int GetDataColumnCount()
	{
		return 2;
	}

	int GetDataColumn(int index)
	{
		return index;
	}

	// list::ListViewColumnItemArranger::IColumnItemView
						
	bool AttachCallback(list::ListViewColumnItemArranger::IColumnItemViewCallback* value)
	{
		if(columnItemViewCallbacks.Contains(value))
		{
			return false;
		}
		else
		{
			columnItemViewCallbacks.Add(value);
			return true;
		}
	}

	bool DetachCallback(list::ListViewColumnItemArranger::IColumnItemViewCallback* value)
	{
		int index=columnItemViewCallbacks.IndexOf(value);
		if(index==-1)
		{
			return false;
		}
		else
		{
			columnItemViewCallbacks.Remove(value);
			return true;
		}
	}

	int GetColumnCount()
	{
		return 4;
	}

	WString GetColumnText(int index)
	{
		switch(index)
		{
		case 0:
			return L"Name";
		case 1:
			return L"Type";
		case 2:
			return L"Date";
		case 3:
			return L"Size";
		default:
			return L"";
		}
	}

	int GetColumnSize(int index)
	{
		if(0<=index && index<4)
		{
			return columnSizes[index];
		}
		else
		{
			return -1;
		}
	}

	void SetColumnSize(int index, int value)
	{
		if(0<=index && index<4)
		{
			columnSizes[index]=value;
			for(int i=0;i<columnItemViewCallbacks.Count();i++)
			{
				columnItemViewCallbacks[i]->OnColumnChanged();
			}
		}
	}

	GuiMenu* GetDropdownPopup(int index)
	{
		return 0;
	}

	GuiListViewColumnHeader::ColumnSortingState GetSortingState(int index)
	{
		return GuiListViewColumnHeader::NotSorted;
	}
};

/***********************************************************************
VirtualModeWindow
***********************************************************************/

class VirtualModeWindow : public GuiWindow
{
private:
	DataSource*						dataSource;
	GuiVirtualListView*				listView;
	GuiComboBoxListControl*			comboView;
	GuiSinglelineTextBox*			textDirectory;
	GuiButton*						buttonRefresh;

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

	void buttonRefresh_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		WString directory=textDirectory->GetText();
		if(directory.Length()>0 && directory[directory.Length()-1]==L'\\')
		{
			directory=directory.Left(directory.Length()-1);
		}
		dataSource->SetDirectory(directory);
	}
public:
	VirtualModeWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		this->SetText(L"Controls.ListView.VirtualMode");

		GuiTableComposition* table=new GuiTableComposition;
		table->SetCellPadding(4);
		table->SetAlignmentToParent(Margin(0, 0, 0, 0));
		table->SetRowsAndColumns(2, 3);
		table->SetRowOption(0, GuiCellOption::MinSizeOption());
		table->SetRowOption(1, GuiCellOption::PercentageOption(1.0));
		table->SetColumnOption(0, GuiCellOption::MinSizeOption());
		table->SetColumnOption(1, GuiCellOption::PercentageOption(1.0));
		table->SetColumnOption(2, GuiCellOption::MinSizeOption());
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 0, 1, 1);

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
			comboView->SelectedIndexChanged.AttachMethod(this, &VirtualModeWindow::comboView_SelectedIndexChanged);
			cell->AddChild(comboView->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 1, 1, 1);

			textDirectory=g::NewTextBox();
			textDirectory->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			cell->AddChild(textDirectory->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 2, 1, 1);

			buttonRefresh=g::NewButton();
			buttonRefresh->SetText(L"<- Load this directory");
			buttonRefresh->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			buttonRefresh->Clicked.AttachMethod(this, &VirtualModeWindow::buttonRefresh_Clicked);
			cell->AddChild(buttonRefresh->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(1, 0, 1, 3);

			dataSource=new DataSource;

			listView=new GuiVirtualListView(GetCurrentTheme()->CreateListViewStyle(), dataSource);
			listView->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			listView->SetHorizontalAlwaysVisible(false);
			listView->SetVerticalAlwaysVisible(false);
			listView->SetMultiSelect(true);
			cell->AddChild(listView->GetBoundsComposition());
		}
		this->GetBoundsComposition()->AddChild(table);

		textDirectory->SetText(GetWindowsDirectory());
		dataSource->SetDirectory(textDirectory->GetText());

		// set the preferred minimum client size
		this->GetBoundsComposition()->SetPreferredMinSize(Size(640, 480));
		// call this to calculate the size immediately if any indirect content in the table changes
		// so that the window can calcaulte its correct size before calling the MoveToScreenCenter()
		this->ForceCalculateSizeImmediately();
		// move to the screen center
		this->MoveToScreenCenter();
	}
};

/***********************************************************************
GuiMain
***********************************************************************/

void GuiMain()
{
	GuiWindow* window=new VirtualModeWindow;
	GetApplication()->Run(window);
	delete window;
}