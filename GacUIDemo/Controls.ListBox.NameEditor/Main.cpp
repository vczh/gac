#include "..\..\Public\Source\GacUI.h"
#include <Windows.h>

// for SortedList, CopyFrom and Select
using namespace vl::collections;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

class NameEditorWindow : public GuiWindow
{
private:
	GuiTextList*						listBox;
	GuiSinglelineTextBox*				textBox;
	GuiButton*							buttonAdd;
	GuiButton*							buttonRemove;
	
	void buttonAdd_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		// add the specified name at the end of the list box
		listBox->GetItems().Add(new list::TextItem(textBox->GetText()));
		textBox->SelectAll();
		textBox->SetFocus();
	}

	void buttonRemove_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		// remove the selected items using item index
		listBox->GetItems().RemoveAt(listBox->GetSelectedItems()[0]);
	}

	void listBox_SelectionChanged(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		// disable the button if no item is selected
		buttonRemove->SetEnabled(listBox->GetSelectedItems().Count()>0);
	}
public:
	NameEditorWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		this->SetText(L"Controls.ListBox.NameEditor");

		GuiTableComposition* table=new GuiTableComposition;
		table->SetRowsAndColumns(4, 3);
		table->SetCellPadding(3);
		table->SetAlignmentToParent(Margin(0, 0, 0, 0));

		table->SetRowOption(0, GuiCellOption::MinSizeOption());
		table->SetRowOption(1, GuiCellOption::MinSizeOption());
		table->SetRowOption(2, GuiCellOption::MinSizeOption());
		table->SetRowOption(3, GuiCellOption::PercentageOption(1.0));

		table->SetColumnOption(0, GuiCellOption::PercentageOption(1.0));
		table->SetColumnOption(1, GuiCellOption::MinSizeOption());
		table->SetColumnOption(2, GuiCellOption::MinSizeOption());

		this->GetContainerComposition()->AddChild(table);
		
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 0, 4, 1);

			listBox=g::NewTextList();
			listBox->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			listBox->SetHorizontalAlwaysVisible(false);
			listBox->SelectionChanged.AttachMethod(this, &NameEditorWindow::listBox_SelectionChanged);
			cell->AddChild(listBox->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 1, 1, 1);

			GuiLabel* label=g::NewLabel();
			label->SetText(L"Name to add: ");
			label->GetBoundsComposition()->SetAlignmentToParent(Margin(0, -1, 0, 0));
			cell->AddChild(label->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 2, 1, 1);

			textBox=g::NewTextBox();
			textBox->GetBoundsComposition()->SetPreferredMinSize(Size(120, 23));
			textBox->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			cell->AddChild(textBox->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(1, 1, 1, 2);

			buttonAdd=g::NewButton();
			buttonAdd->SetText(L"Add");
			buttonAdd->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			buttonAdd->Clicked.AttachMethod(this, &NameEditorWindow::buttonAdd_Clicked);
			cell->AddChild(buttonAdd->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(2, 1, 1, 2);

			buttonRemove=g::NewButton();
			buttonRemove->SetText(L"Delete");
			buttonRemove->SetEnabled(false);
			buttonRemove->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			buttonRemove->Clicked.AttachMethod(this, &NameEditorWindow::buttonRemove_Clicked);
			cell->AddChild(buttonRemove->GetBoundsComposition());
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
	GuiWindow* window=new NameEditorWindow;
	GetApplication()->Run(window);
	delete window;
}