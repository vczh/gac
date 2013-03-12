#include "..\..\Public\Source\GacUI.h"
#include <Windows.h>

using namespace vl::parsing::xml;
using namespace vl::stream;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

class TooltipWindow : public GuiWindow
{
private:

	void tooltipLabel_ActiveHyperlinkExecuted(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		GuiDocumentLabel* label=dynamic_cast<GuiDocumentLabel*>(sender->GetRelatedControl());
		GetCurrentController()->DialogService()->ShowMessageBox(
			GetNativeWindow(),
			label->GetActiveHyperlinkReference(),
			GetText());
	}

	void AddTooltip(GuiControl* control, Ptr<GuiResource> resource, const WString& path, vint width)
	{
		GuiDocumentLabel* tooltipLabel=g::NewDocumentLabel();
		tooltipLabel->ActiveHyperlinkExecuted.AttachMethod(this, &TooltipWindow::tooltipLabel_ActiveHyperlinkExecuted);
		control->SetTooltipControl(tooltipLabel);
		control->SetTooltipWidth(width);

		Ptr<DocumentModel> tooltipDocument=resource->GetValueByPath(path).Cast<DocumentModel>();
		tooltipLabel->SetDocument(tooltipDocument);
	}
public:
	TooltipWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		SetText(L"Controls.Tooltip.RichTextAndResource");
		SetClientSize(Size(440, 280));
		GetContainerComposition()->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);
		MoveToScreenCenter();
		Ptr<GuiResource> resource=GuiResource::LoadFromXml(L"..\\Resources\\XmlResource.xml");

		{
			GuiButton* tooltipButton=g::NewButton();
			tooltipButton->SetText(L"This is a button with tooltip.");
			tooltipButton->GetBoundsComposition()->SetAlignmentToParent(Margin(100, 100, 100, 100));
			AddChild(tooltipButton);
			AddTooltip(tooltipButton, resource, L"TooltipDocument", 200);
		}

		const wchar_t* images[]=
		{
			L"Images/_New.png",
			L"Images/_Open.png",
			L"Images/_Save.png",
			L"Images/_SaveAs.png",
		};
		const wchar_t* paths[]=
		{
			L"Tooltips/New",
			L"Tooltips/Open",
			L"Tooltips/Save",
			L"Tooltips/SaveAs",
		};

		GuiToolstripToolbar* toolbar=g::NewToolbar();
		toolbar->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, -1));
		AddChild(toolbar);
		for(vint i=0;i<4;i++)
		{
			GuiToolstripButton* button=g::NewToolbarButton();
			button->SetImage(resource->GetValueByPath(images[i]).Cast<GuiImageData>());
			AddTooltip(button, resource, paths[i], 100);
			toolbar->GetToolstripItems().Add(button);
		}
	}
};

void GuiMain()
{
	TooltipWindow window;
	GetApplication()->Run(&window);
}