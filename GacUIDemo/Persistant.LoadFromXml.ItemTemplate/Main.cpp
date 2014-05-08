#include "UI\Demo.h"
#include <Windows.h>

using namespace vl::collections;
using namespace vl::reflection::description;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

void GuiMain()
{
	List<WString> errors;
	GetInstanceLoaderManager()->SetResource(L"Resource", GuiResource::LoadFromXml(L"..\\Resources\\XmlWindowResourceItemTemplate.xml", errors));
	demos::MainWindow window;
	window.ForceCalculateSizeImmediately();
	window.MoveToScreenCenter();
	GetApplication()->Run(&window);
}