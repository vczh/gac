#include "..\..\Public\Source\GacUI.h"
#include <Windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

void GuiMain()
{
	GuiWindow* window=g::NewWindow();
	window->SetText(L"Hello, world!");
	window->SetClientSize(Size(640, 480));
	window->MoveToScreenCenter();
	
	GuiControl* label=g::NewLabel();
	label->SetText(L"Welcome to GacUI Library!");
	{
		FontProperties font;
		font.fontFamily=L"Segoe UI";
		font.size=40;
		font.antialias=true;
		label->SetFont(font);
	}
	window->AddChild(label);

	GetApplication()->Run(window);
	delete window;
}