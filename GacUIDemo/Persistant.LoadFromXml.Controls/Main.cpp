#include "..\..\Public\Source\GacUIReflection.h"
#include <Windows.h>

using namespace vl::reflection::description;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

class MainWindowInstance : public GuiInstance<GuiWindow>
{
protected:
	GuiTextList*					listResources;
	GuiButton*						buttonShow;

	void ShowWindowInResource(const WString& name)
	{
		auto scope = LoadInstance(GetResource(), L"XmlWindowDemos/" + name + L"/MainWindowResource");
		auto window = UnboxValue<GuiWindow*>(scope->rootInstance);

		window->ForceCalculateSizeImmediately();
		window->MoveToScreenCenter();
		window->Show();

		window->WindowClosed.AttachLambda([=](GuiGraphicsComposition* sender, GuiEventArgs& arguments)
		{
			GetApplication()->InvokeInMainThread([=]()
			{
				delete window;
			});
		});
	}

	void listResources_SelectionChanged(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		buttonShow->SetEnabled(listResources->GetSelectedItems().Count() == 1);
	}

	void listResources_ItemLeftButtonDoubleClick(GuiGraphicsComposition* sender, GuiItemMouseEventArgs& arguments)
	{
		ShowWindowInResource(listResources->GetItems()[arguments.itemIndex]->GetText());
	}

	void buttonShow_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	{
		vint itemIndex = listResources->GetSelectedItems()[0];
		ShowWindowInResource(listResources->GetItems()[itemIndex]->GetText());
	}
public:
	MainWindowInstance(Ptr<GuiResource> resource)
		:GuiInstance(resource, L"XmlWindowDemos/MainWindow/MainWindowResource")
	{
		GUI_INSTANCE_REFERENCE(listResources);
		GUI_INSTANCE_REFERENCE(buttonShow);
		
		listResources->SelectionChanged.AttachMethod(this, &MainWindowInstance::listResources_SelectionChanged);
		listResources->ItemLeftButtonDoubleClick.AttachMethod(this, &MainWindowInstance::listResources_ItemLeftButtonDoubleClick);
		buttonShow->Clicked.AttachMethod(this, &MainWindowInstance::buttonShow_Clicked);

		GetInstance()->ForceCalculateSizeImmediately();
		GetInstance()->MoveToScreenCenter();
	}

	~MainWindowInstance()
	{
		delete GetInstance();
	}
};

void GuiMain()
{
	auto resource=GuiResource::LoadFromXml(L"..\\Resources\\XmlWindowResource.xml");
	MainWindowInstance instance(resource);
	GetApplication()->Run(instance.GetInstance());
}