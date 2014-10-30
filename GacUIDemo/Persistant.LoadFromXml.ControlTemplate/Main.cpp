#include "UI\HelloWorld.h"
#include <Windows.h>

using namespace vl::collections;
using namespace vl::stream;
using namespace vl::reflection::description;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

void GuiMain()
{
	List<WString> errors;
	{
		FileStream stream(L"..\\Resources\\XmlWindowResourceControlTemplate.precompiled.bin", FileStream::ReadOnly);
		auto resource = GuiResource::LoadPrecompiledBinary(stream, errors);
		GetInstanceLoaderManager()->SetResource(L"Resource", resource);
	}
	demos::MainWindow window;
	window.ForceCalculateSizeImmediately();
	window.MoveToScreenCenter();
	GetApplication()->Run(&window);
}