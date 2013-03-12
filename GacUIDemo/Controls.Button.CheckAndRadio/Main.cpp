#include "..\..\Public\Source\GacUI.h"
#include <Windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

class CheckAndRadioWindow : public GuiWindow
{
private:

	GuiCellComposition* CreateButtons(const WString& groupName, const WString& buttonName, bool checkBox, GuiSelectableButton::GroupController* groupController)
	{
		GuiCellComposition* cell=new GuiCellComposition;

		GuiControl* groupBox=g::NewGroupBox();
		groupBox->GetBoundsComposition()->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);
		groupBox->GetContainerComposition()->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);
		// all child controls should at least 10 pixels away from the group box
		groupBox->GetContainerComposition()->SetInternalMargin(Margin(10, 10, 10, 10));
		// dock the group box to fill the cell
		groupBox->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
		groupBox->SetText(groupName);
		// add the button to the cell
		cell->AddChild(groupBox->GetBoundsComposition());

		// create a stack to layout the 3 buttons from top to bottom shown like a list
		GuiStackComposition* stack=new GuiStackComposition;
		stack->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);
		stack->SetDirection(GuiStackComposition::Vertical);
		stack->SetAlignmentToParent(Margin(0, 0, 0, 0));
		stack->SetPadding(6);
		groupBox->GetContainerComposition()->AddChild(stack);

		// create buttons
		for(int i=0;i<3;i++)
		{
			GuiSelectableButton* button=checkBox?g::NewCheckBox():g::NewRadioButton();
			button->SetText(buttonName+itow(i+1));
			button->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			if(groupController)
			{
				button->SetGroupController(groupController);
			}

			GuiStackItemComposition* stackItem=new GuiStackItemComposition;
			stack->AddChild(stackItem);
			stackItem->AddChild(button->GetBoundsComposition());
		}

		return cell;
	}
public:
	CheckAndRadioWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		this->SetText(L"Controls.Button.CheckAndRadio");
		// limit the size that the window should always show the whole content without cliping it
		this->GetContainerComposition()->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);

		// create a table to layout the 2 group boxes
		GuiTableComposition* table=new GuiTableComposition;
		// make the table to have 2 rows
		table->SetRowsAndColumns(1, 2);
		table->SetRowOption(0, GuiCellOption::MinSizeOption());
		table->SetColumnOption(0, GuiCellOption::PercentageOption(0.5));
		table->SetColumnOption(1, GuiCellOption::PercentageOption(0.5));
		// dock the table to fill the window
		table->SetAlignmentToParent(Margin(4, 4, 4, 4));
		table->SetCellPadding(6);
		// add the table to the window;
		this->GetContainerComposition()->AddChild(table);

		// add group box for check boxes
		{
			GuiCellComposition* cell=CreateButtons(L"Check Boxes", L"This is a check box ", true, 0);
			table->AddChild(cell);
			// this cell is the left cell
			cell->SetSite(0, 0, 1, 1);
		}

		// add group box for radio buttons
		{
			// create a group controller to group those radio buttons together
			// so that select a radio button will unselect the previous one automatically
			GuiSelectableButton::GroupController* groupController=new GuiSelectableButton::MutexGroupController;
			this->AddComponent(groupController);

			GuiCellComposition* cell=CreateButtons(L"Radio buttons", L"This is a radio button ", false, groupController);
			table->AddChild(cell);
			// this cell is the right cell
			cell->SetSite(0, 1, 1, 1);
		}

		// call this to calculate the size immediately if any indirect content in the table changes
		// so that the window can calcaulte its correct size before calling the MoveToScreenCenter()
		this->ForceCalculateSizeImmediately();
		// move to the screen center
		this->MoveToScreenCenter();
	}

	~CheckAndRadioWindow()
	{
	}
};

void GuiMain()
{
	GuiWindow* window=new CheckAndRadioWindow();
	GetApplication()->Run(window);
	delete window;
}