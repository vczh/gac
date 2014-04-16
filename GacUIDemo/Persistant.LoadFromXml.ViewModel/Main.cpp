#include "UI\Demo.h"
#include <Windows.h>

using namespace vl::collections;
using namespace vl::reflection::description;
using namespace demos;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

class ControlViewModel : public Object, public virtual IControlViewModel
{
private:
	vint			first = 0;
	vint			second = 0;
public:
	vint GetFirst()override
	{
		return first;
	}

	void SetFirst(const vint& value)override
	{
		if (first != value)
		{
			first = value;
			SumChanged();
		}
	}

	vint GetSecond()override
	{
		return second;
	}

	void SetSecond(const vint& value)override
	{
		if (second != value)
		{
			second = value;
			SumChanged();
		}
	}

	vint GetSum()override
	{
		return first + second;
	}

	vint GetProduct()override
	{
		return first * second;
	}

	void UpdateProduct()override
	{
		ProductChanged();
	}
};

class WindowViewModel : public Object, public virtual IWindowViewModel
{
private:
	Ptr<IControlViewModel>		controlViewModel;
public:
	WindowViewModel()
	{
		controlViewModel = new ControlViewModel;
	}

	Ptr<IControlViewModel> GetControlViewModel()override
	{
		return controlViewModel;
	}
};

void GuiMain()
{
	List<WString> errors;
	GetInstanceLoaderManager()->SetResource(L"Resource", GuiResource::LoadFromXml(L"..\\Resources\\XmlWindowResourceViewModel.xml", errors));
	MainWindow window(new WindowViewModel);
	window.ForceCalculateSizeImmediately();
	window.MoveToScreenCenter();
	GetApplication()->Run(&window);
}