#include "..\..\Public\Source\GacUI.h"
#include <Windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

class ColorPickerWindow : public GuiWindow
{
private:
	GuiSolidBackgroundElement*		colorElement;
	GuiLabel*						labelComponents[3];
	GuiScroll*						trackers[3];

	void UpdateColor()
	{
		int components[3];
		for(int i=0;i<3;i++)
		{
			components[i]=trackers[i]->GetPosition();
			labelComponents[i]->SetText(itow(components[i]));
		}
		colorElement->SetColor(Color((unsigned char)components[0], (unsigned char)components[1], (unsigned char)components[2]));
	}

	void Tracker_PositionChanged(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		UpdateColor();
	}
public:
	ColorPickerWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		this->SetText(L"Controls.Scroll.ColorPicker");
		this->GetContainerComposition()->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);

		GuiTableComposition* table=new GuiTableComposition;
		table->SetAlignmentToParent(Margin(0, 0, 0, 0));
		table->SetCellPadding(2);
		table->SetRowsAndColumns(4, 3);
		table->SetRowOption(0, GuiCellOption::PercentageOption(1.0));
		table->SetRowOption(1, GuiCellOption::MinSizeOption());
		table->SetRowOption(2, GuiCellOption::MinSizeOption());
		table->SetRowOption(3, GuiCellOption::MinSizeOption());
		table->SetColumnOption(0, GuiCellOption::MinSizeOption());
		table->SetColumnOption(1, GuiCellOption::AbsoluteOption(32));
		table->SetColumnOption(2, GuiCellOption::PercentageOption(1.0));
		this->GetContainerComposition()->AddChild(table);

		{
			colorElement=GuiSolidBackgroundElement::Create();

			GuiBoundsComposition* colorComposition=new GuiBoundsComposition;
			colorComposition->SetAlignmentToParent(Margin(0, 0, 0, 0));
			colorComposition->SetOwnedElement(colorElement);

			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 0, 1, 3);
			cell->AddChild(colorComposition);
		}

		const wchar_t* labels[]={L"R: ", L"G: ", L"B: "};
		for(int i=0;i<3;i++)
		{
			{
				GuiLabel* label=g::NewLabel();
				label->SetText(labels[i]);
				label->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));

				GuiCellComposition* cell=new GuiCellComposition;
				table->AddChild(cell);
				cell->SetSite(i+1, 0, 1, 1);
				cell->AddChild(label->GetBoundsComposition());
			}
			{
				labelComponents[i]=g::NewLabel();
				labelComponents[i]->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));

				GuiCellComposition* cell=new GuiCellComposition;
				table->AddChild(cell);
				cell->SetSite(i+1, 1, 1, 1);
				cell->AddChild(labelComponents[i]->GetBoundsComposition());
			}
			{
				trackers[i]=g::NewHTracker();
				trackers[i]->SetTotalSize(255);
				trackers[i]->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
				trackers[i]->PositionChanged.AttachMethod(this, &ColorPickerWindow::Tracker_PositionChanged);

				GuiCellComposition* cell=new GuiCellComposition;
				table->AddChild(cell);
				cell->SetSite(i+1, 2, 1, 1);
				cell->AddChild(trackers[i]->GetBoundsComposition());
			}
		}

		UpdateColor();

		// set the preferred minimum client size
		this->GetBoundsComposition()->SetPreferredMinSize(Size(640, 480));
		// call this to calculate the size immediately if any indirect content in the table changes
		// so that the window can calcaulte its correct size before calling the MoveToScreenCenter()
		this->ForceCalculateSizeImmediately();
		// move to the screen center
		this->MoveToScreenCenter();
	}

	~ColorPickerWindow()
	{
	}
};

void GuiMain()
{
	GuiWindow* window=new ColorPickerWindow();
	GetApplication()->Run(window);
	delete window;
}