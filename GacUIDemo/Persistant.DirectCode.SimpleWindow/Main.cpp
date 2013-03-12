#include "..\..\Public\Source\GacUI.h"
#include <Windows.h>

using namespace vl::presentation::description;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

void GuiMain()
{
	// auto currentTheme=ITheme::GetCurrentTheme();
	Value currentTheme=Value::InvokeStatic(L"presentation::theme::ITheme", L"GetCurrentTheme");
	// auto windowStyle=currentTheme->CreateWindowStyle();
	Value windowStyle=currentTheme.Invoke(L"CreateWindowStyle");
	// auto window=new GuiWindow(windowStyle);
	Value window=Value::Create(L"presentation::controls::GuiWindow", (Value::xs(), windowStyle));

	// auto clientSize=Size(320, 240);
	Value clientSize=Value::From(L"x:320 y:240", GetTypeDescriptor(L"presentation::Size"));
	// window->SetText(L"Window By Reflection!");
	window.SetProperty(L"Text", WString(L"Window By Reflection!"));
	// window->SetClientSize(clientSize);
	window.SetProperty(L"ClientSize", clientSize);
	// window->GetContainerComposition()->SetPreferredMinSize(clientSize);
	window.GetProperty(L"ContainerComposition").SetProperty(L"PreferredMinSize", clientSize);
	// window->MoveToScreenCenter();
	window.Invoke(L"MoveToScreenCenter");

	// auto buttonStyle=currentTheme->CreateButtonStyle();
	Value buttonStyle=currentTheme.Invoke(L"CreateButtonStyle");
	// auto button=new GuiButton(buttonStyle);
	Value button=Value::Create(L"presentation::controls::GuiButton", (Value::xs(), buttonStyle));
	// button->GetBoundsComposition()->SetAlignmentToParent(Margin(60, 60, 60, 60));
	button.GetProperty(L"BoundsComposition").SetProperty(L"AlignmentToParent", Value::From(L"left:60 top:60 right:60 bottom:60", GetTypeDescriptor(L"presentation::Margin")));
	// button->SetText(L"Click Me!");
	button.SetProperty(L"Text", WString(L"Click Me!"));
	// FontProperties font;
	// font.fontFamily=L"Segoe UI";
	// font.size=16;
	// font.bold=true;
	// button->SetFont(font);
	button.SetProperty(L"Font", Value::From(L"fontFamily:{Segoe UI} size:16 bold:true", GetTypeDescriptor(L"presentation::FontProperties")));
	// window->AddChild(button);
	window.Invoke(L"AddChild", (Value::xs(), button));

	// button->Clicked.AttachLambda([=](GuiGraphicsComposition* sender, GuiEventArgs& arguments)
	// {
	//		button->SetText(L"You clicked but button!");
	// });
	Value handler=BoxParameter<Func<Value(Value, Value)>>(
		LAMBDA([&button](Value sender, Value arguments)
		{
			button.SetProperty(L"Text", WString(L"You clicked the button!"));
			return Value();
		}));
	button.AttachEvent(L"Clicked", handler);

	// GetApplication()->Run(window);
	Value application=Value::InvokeStatic(L"presentation::controls::GuiApplication", L"GetApplication");
	application.Invoke(L"Run", (Value::xs(), window));
	// delete window;
	window.DeleteRawPtr();
}