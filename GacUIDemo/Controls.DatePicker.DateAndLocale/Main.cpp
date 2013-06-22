#include "..\..\Public\Source\GacUI.h"
#include <Windows.h>

using namespace vl::collections;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

/***********************************************************************
DatePickerWindow
***********************************************************************/

class DatePickerWindow : public GuiWindow
{
private:
	GuiLabel*									labelLocale;
	GuiTextList*								listLocales;
	GuiLabel*									labelFormat;
	GuiTextList*								listFormats;
	
	GuiLabel*									labelDate;
	GuiDateComboBox*							dateComboBox;
	GuiLabel*									labelPicker;
	GuiDatePicker*								datePicker;

	void FillLocales()
	{
		listLocales->GetItems().Clear();
		List<Locale> locales;
		Locale::Enumerate(locales);

		FOREACH(Locale, locale, locales)
		{
			listLocales->GetItems().Add(new list::TextItem(locale.GetName()));
		}
		CopyFrom(
			listLocales->GetItems(),
			From(locales)
				.Select([](const Locale& locale)->WString{return locale.GetName();})
				.OrderBy([](const WString& a, const WString& b){return WString::Compare(a, b);})
				.Select([](const WString& locale){return new list::TextItem(locale);})
			);
		listLocales->SetSelected(0, true);
	}

	void FillFormats(const Locale& locale)
	{
		List<WString> formats;
		locale.GetLongDateFormats(formats);
		locale.GetShortDateFormats(formats);

		CopyFrom(
			listFormats->GetItems(),
			From(formats)
				.Select([](const WString& format){return new list::TextItem(format);})
			);
		listFormats->SetSelected(0, true);
	}

	void listLocales_SelectionChanged(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		if(listLocales->GetSelectedItems().Count()>0)
		{
			Locale locale(listLocales->GetItems().Get(listLocales->GetSelectedItems().Get(0))->GetText());
			datePicker->SetDateLocale(locale);
			dateComboBox->GetDatePicker()->SetDateLocale(locale);
			FillFormats(locale);
		}
	}

	void listFormats_SelectionChanged(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		if(listFormats->GetSelectedItems().Count()>0)
		{
			WString format=listFormats->GetItems().Get(listFormats->GetSelectedItems().Get(0))->GetText();
			datePicker->SetDateFormat(format);
			dateComboBox->GetDatePicker()->SetDateFormat(format);
		}
	}

	void datePicker_TextChanged(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		labelPicker->SetText(L"¡ý: "+datePicker->GetText());

		/* equivalent code that can put in GuiDatePicker::DateChanged

		DateTime date=datePicker->GetDate();
		Locale locale=datePicker->GetDateLocale();
		WString format=datePicker->GetDateFormat();
		WString dateText=locale.FormatDate(format, date);
		labelPicker->SetText(L"¡ý: "+dateText);
		*/
	}
public:
	DatePickerWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		this->SetText(L"Controls.DatePicker.DateAndLocale");

		GuiTableComposition* table=new GuiTableComposition;
		table->SetAlignmentToParent(Margin(0, 0, 0, 0));
		table->SetCellPadding(5);
		table->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);

		table->SetRowsAndColumns(4, 3);
		table->SetRowOption(0, GuiCellOption::MinSizeOption());
		table->SetRowOption(1, GuiCellOption::MinSizeOption());
		table->SetRowOption(2, GuiCellOption::PercentageOption(1.0));
		table->SetRowOption(3, GuiCellOption::MinSizeOption());
		table->SetColumnOption(0, GuiCellOption::PercentageOption(0.5));
		table->SetColumnOption(1, GuiCellOption::PercentageOption(0.5));
		table->SetColumnOption(2, GuiCellOption::MinSizeOption());

		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 0, 1, 1);

			labelLocale=g::NewLabel();
			labelLocale->SetText(L"Select Locale:");
			labelLocale->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			cell->AddChild(labelLocale->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(1, 0, 3, 1);

			listLocales=g::NewTextList();
			listLocales->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			listLocales->SelectionChanged.AttachMethod(this, &DatePickerWindow::listLocales_SelectionChanged);
			cell->AddChild(listLocales->GetBoundsComposition());
		}

		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 1, 1, 1);

			labelFormat=g::NewLabel();
			labelFormat->SetText(L"Select Format:");
			labelFormat->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			cell->AddChild(labelFormat->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(1, 1, 3, 1);

			listFormats=g::NewTextList();
			listFormats->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			listFormats->SelectionChanged.AttachMethod(this, &DatePickerWindow::listFormats_SelectionChanged);
			cell->AddChild(listFormats->GetBoundsComposition());
		}
		
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 2, 1, 1);

			labelDate=g::NewLabel();
			labelDate->SetText(L"Select Date:");
			labelDate->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			cell->AddChild(labelDate->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(1, 2, 1, 1);

			dateComboBox=g::NewDateComboBox();
			dateComboBox->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			cell->AddChild(dateComboBox->GetBoundsComposition());
		}
		
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(2, 2, 1, 1);

			labelPicker=g::NewLabel();
			labelPicker->SetText(L"¡ý: ");
			labelPicker->GetBoundsComposition()->SetAlignmentToParent(Margin(0, -1, 0, 0));
			cell->AddChild(labelPicker->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(3, 2, 1, 1);

			datePicker=g::NewDatePicker();
			datePicker->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			datePicker->TextChanged.AttachMethod(this, &DatePickerWindow::datePicker_TextChanged);
			cell->AddChild(datePicker->GetBoundsComposition());
		}
		FillLocales();

		this->GetBoundsComposition()->AddChild(table);

		// set the preferred minimum client size
		this->GetBoundsComposition()->SetPreferredMinSize(Size(640, 320));
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
	GuiWindow* window=new DatePickerWindow;
	GetApplication()->Run(window);
	delete window;
}