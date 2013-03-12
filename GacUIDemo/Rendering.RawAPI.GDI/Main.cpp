#include "..\..\Public\Source\GacUIWindows.h"
#include <math.h>

using namespace vl::presentation::windows;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	// SetupWindowsGDIRenderer() is required for GuiGDIElement
	return SetupWindowsGDIRenderer();
}

class GDIWindow : public GuiWindow
{
protected:
	static const int		Radius=200;
	static const int		LongScale=10;
	static const int		ShortScale=5;
	static const int		SecondLength=180;
	static const int		MinuteLength=150;
	static const int		HourLength=120;

	Ptr<WinPen>				borderPen;
	Ptr<WinPen>				secondPen;
	Ptr<WinPen>				minutePen;
	Ptr<WinPen>				hourPen;

	void CreateGDIResources()
	{
		borderPen=new WinPen(PS_SOLID, 1, RGB(0, 0, 0));
		secondPen=new WinPen(PS_SOLID, 1, RGB(0, 0, 255));
		minutePen=new WinPen(PS_SOLID, 3, RGB(0, 255, 0));
		hourPen=new WinPen(PS_SOLID, 5, RGB(255, 0, 0));
	}

	double GetAngle(double second)
	{
		return (second-15)*3.1416/30;
	}

	void DrawLine(windows::WinDC* dc, Ptr<WinPen> pen, double angle, int startLength, int endLength, int x, int y)
	{
		dc->SetPen(pen);
		double s=sin(angle);
		double c=cos(angle);
		int x1=(int)(c*startLength)+x+Radius;
		int y1=(int)(s*startLength)+y+Radius;
		int x2=(int)(c*endLength)+x+Radius;
		int y2=(int)(s*endLength)+y+Radius;
		dc->MoveTo(x1, y1);
		dc->LineTo(x2, y2);
	}

	// arguments.dc is WinDC, which is an encapsulation for HDC.
	void element_Rendering(GuiGraphicsComposition* sender, GuiGDIElementEventArgs& arguments)
	{
		int w=arguments.bounds.Width();
		int h=arguments.bounds.Height();
		int x=arguments.bounds.Left()+(w-Radius*2)/2;
		int y=arguments.bounds.Left()+(h-Radius*2)/2;

		arguments.dc->SetPen(borderPen);
		arguments.dc->Ellipse(x, y, x+Radius*2, y+Radius*2);
		for(int i=0;i<60;i++)
		{
			int scale=i%5==0?LongScale:ShortScale;
			double angle=GetAngle(i);
			DrawLine(arguments.dc, borderPen, angle, Radius-scale, Radius, x, y);
		}

		DateTime dt=DateTime::LocalTime();
		{
			double angle=GetAngle(dt.hour*5+dt.minute/12.0+dt.second/720.0);
			DrawLine(arguments.dc, hourPen, angle, 0, HourLength, x, y);
		}
		{
			double angle=GetAngle(dt.minute+dt.second/60.0);
			DrawLine(arguments.dc, minutePen, angle, 0, MinuteLength, x, y);
		}
		{
			double angle=GetAngle(dt.second);
			DrawLine(arguments.dc, secondPen, angle, 0, SecondLength, x, y);
		}
	}
public:
	GDIWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		SetText(L"Rendering.RawAPI.GDI");
		SetClientSize(Size(640, 480));
		GetBoundsComposition()->SetPreferredMinSize(Size(640, 480));
		MoveToScreenCenter();
		{
			CreateGDIResources();

			GuiGDIElement* element=GuiGDIElement::Create();
			element->Rendering.AttachMethod(this, &GDIWindow::element_Rendering);

			GuiBoundsComposition* composition=new GuiBoundsComposition;
			composition->SetAlignmentToParent(Margin(0, 0, 0, 0));
			composition->SetOwnedElement(element);
			GetContainerComposition()->AddChild(composition);
		}
	}
};

void GuiMain()
{
	GDIWindow window;
	GetApplication()->Run(&window);
}