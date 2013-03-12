#include "..\..\Public\Source\GacUI.h"
#include <Windows.h>

// for SortedList, CopyFrom and Select
using namespace vl::collections;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

/***********************************************************************
DataSource
***********************************************************************/

class DataSource : public list::ItemProviderBase, private list::TextItemStyleProvider::ITextItemView
{
protected:
	int				count;
public:
	DataSource()
		:count(100000)
	{
	}

	void SetCount(int newCount)
	{
		if(0<=newCount)
		{
			int oldCount=count;
			count=newCount;
				
			// this->InvokeOnItemModified(affected-items-start, affected-items-count, new-items-count);
			// this function notifies the list control to update it's content and scroll bars
			if(oldCount<newCount)
			{
				// insert
				this->InvokeOnItemModified(oldCount, 0, newCount-oldCount);
			}
			else if(oldCount>newCount)
			{
				// delete
				this->InvokeOnItemModified(newCount, oldCount-newCount, 0);
			}
		}
	}

	// GuiListControl::IItemProvider

	int Count()
	{
		return count;
	}

	IDescriptable* RequestView(const WString& identifier)
	{
		if(identifier==list::TextItemStyleProvider::ITextItemView::Identifier)
		{
			return this;
		}
		else if(identifier==GuiListControl::IItemPrimaryTextView::Identifier)
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

	// list::TextItemStyleProvider::ITextItemView

	WString GetText(int itemIndex)
	{
		return L"Item "+itow(itemIndex+1);
	}

	bool GetChecked(int itemIndex)
	{
		// DataSource don't support check state
		return false;
	}

	void SetCheckedSilently(int itemIndex, bool value)
	{
		// DataSource don't support check state
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
};

/***********************************************************************
VirtualModeWindow
***********************************************************************/

class VirtualModeWindow : public GuiWindow
{
private:
	GuiVirtualTextList*					listBox;
	GuiButton*							buttonIncrease;
	GuiButton*							buttonDecrease;
	DataSource*							dataSource;
	
	void buttonIncrease_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		dataSource->SetCount(dataSource->Count()+100000);
	}

	void buttonDecrease_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		dataSource->SetCount(dataSource->Count()-100000);
	}
public:
	VirtualModeWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		this->SetText(L"Controls.ListBox.VirtualMode");

		GuiTableComposition* table=new GuiTableComposition;
		table->SetRowsAndColumns(3, 2);
		table->SetCellPadding(3);
		table->SetAlignmentToParent(Margin(0, 0, 0, 0));

		table->SetRowOption(0, GuiCellOption::MinSizeOption());
		table->SetRowOption(1, GuiCellOption::MinSizeOption());
		table->SetRowOption(2, GuiCellOption::PercentageOption(1.0));

		table->SetColumnOption(0, GuiCellOption::PercentageOption(1.0));
		table->SetColumnOption(1, GuiCellOption::MinSizeOption());

		this->GetContainerComposition()->AddChild(table);
		
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 0, 3, 1);

			dataSource=new DataSource;
			listBox=new GuiVirtualTextList(GetCurrentTheme()->CreateTextListStyle(), GetCurrentTheme()->CreateTextListItemStyle(), dataSource);
			listBox->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			listBox->SetHorizontalAlwaysVisible(false);
			cell->AddChild(listBox->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 1, 1, 1);

			buttonIncrease=g::NewButton();
			buttonIncrease->SetText(L"Increase 100000 Items");
			buttonIncrease->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			buttonIncrease->Clicked.AttachMethod(this, &VirtualModeWindow::buttonIncrease_Clicked);
			cell->AddChild(buttonIncrease->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(1, 1, 1, 1);

			buttonDecrease=g::NewButton();
			buttonDecrease->SetText(L"Decrease 100000 Items");
			buttonDecrease->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			buttonDecrease->Clicked.AttachMethod(this, &VirtualModeWindow::buttonDecrease_Clicked);
			cell->AddChild(buttonDecrease->GetBoundsComposition());
		}

		// set the preferred minimum client size
		this->GetBoundsComposition()->SetPreferredMinSize(Size(480, 480));
		// call this to calculate the size immediately if any indirect content in the table changes
		// so that the window can calcaulte its correct size before calling the MoveToScreenCenter()
		this->ForceCalculateSizeImmediately();
		// move to the screen center
		this->MoveToScreenCenter();
	}
};

void GuiMain()
{
	GuiWindow* window=new VirtualModeWindow;
	GetApplication()->Run(window);
	delete window;
}