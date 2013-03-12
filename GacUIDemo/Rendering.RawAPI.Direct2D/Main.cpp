#include "..\..\Public\Source\GacUIWindows.h"
#include <math.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	// SetupWindowsDirect2DRenderer() is required for GuiDirect2DElement
	return SetupWindowsDirect2DRenderer();
}

class Direct2DWindow : public GuiWindow
{
protected:
	static const int						Radius=200;
	static const int						LongScale=10;
	static const int						ShortScale=5;
	static const int						SecondLength=180;
	static const int						MinuteLength=150;
	static const int						HourLength=120;

	ComPtr<ID2D1SolidColorBrush>			borderBrush;
	ComPtr<ID2D1SolidColorBrush>			secondBrush;
	ComPtr<ID2D1SolidColorBrush>			minuteBrush;
	ComPtr<ID2D1SolidColorBrush>			hourBrush;

	float GetAngle(float second)
	{
		return (second-15.0f)*3.1416f/30.0f;
	}

	void DrawLine(ID2D1RenderTarget* rt, ComPtr<ID2D1SolidColorBrush> brush, float angle, int width, int startLength, int endLength, int x, int y)
	{
		float s=sin(angle);
		float c=cos(angle);
		float x1=(c*startLength)+(float)(x+Radius);
		float y1=(s*startLength)+(float)(y+Radius);
		float x2=(c*endLength)+(float)(x+Radius);
		float y2=(s*endLength)+(float)(y+Radius);
		rt->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), brush.Obj(), (float)width);
	}

	// arguments.rt is ID2D1RenderTarget.
	void element_Rendering(GuiGraphicsComposition* sender, GuiDirect2DElementEventArgs& arguments)
	{
		int w=arguments.bounds.Width();
		int h=arguments.bounds.Height();
		int x=arguments.bounds.Left()+(w-Radius*2)/2;
		int y=arguments.bounds.Left()+(h-Radius*2)/2;

		arguments.rt->DrawEllipse(D2D1::Ellipse(D2D1::Point2F((float)(x+Radius), (float)(y+Radius)), (float)Radius, (float)Radius), borderBrush.Obj());
		for(int i=0;i<60;i++)
		{
			int scale=i%5==0?LongScale:ShortScale;
			float angle=GetAngle((float)i);
			DrawLine(arguments.rt, borderBrush, angle, 1, Radius-scale, Radius, x, y);
		}

		DateTime dt=DateTime::LocalTime();
		{
			float angle=GetAngle(dt.hour*5+dt.minute/12.0f+dt.second/720.0f);
			DrawLine(arguments.rt, hourBrush, angle, 5, 0, HourLength, x, y);
		}
		{
			float angle=GetAngle(dt.minute+dt.second/60.0f);
			DrawLine(arguments.rt, minuteBrush, angle, 3, 0, MinuteLength, x, y);
		}
		{
			float angle=GetAngle((float)dt.second);
			DrawLine(arguments.rt, secondBrush, angle, 1, 0, SecondLength, x, y);
		}
	}

	// The render target is going to be destroyed, any binded resources should be released.
	void element_BeforeRenderTargetChanged(GuiGraphicsComposition* sender, GuiDirect2DElementEventArgs& arguments)
	{
		borderBrush=0;
		secondBrush=0;
		minuteBrush=0;
		hourBrush=0;
	}

	// The new render target is prepared, any binded resources are allowed to recreate now.
	void element_AfterRenderTargetChanged(GuiGraphicsComposition* sender, GuiDirect2DElementEventArgs& arguments)
	{
		ID2D1SolidColorBrush* brush;
		{
			brush=0;
			arguments.rt->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f), D2D1::BrushProperties(), &brush);
			borderBrush=brush;
		}
		{
			brush=0;
			arguments.rt->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 1.0f), D2D1::BrushProperties(), &brush);
			secondBrush=brush;
		}
		{
			brush=0;
			arguments.rt->CreateSolidColorBrush(D2D1::ColorF(0.0f, 1.0f, 0.0f), D2D1::BrushProperties(), &brush);
			minuteBrush=brush;
		}
		{
			brush=0;
			arguments.rt->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.0f, 0.0f), D2D1::BrushProperties(), &brush);
			hourBrush=brush;
		}
	}
public:
	Direct2DWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		SetText(L"Rendering.RawAPI.Direct2D");
		SetClientSize(Size(640, 480));
		GetBoundsComposition()->SetPreferredMinSize(Size(640, 480));
		MoveToScreenCenter();
		{
			GuiDirect2DElement* element=GuiDirect2DElement::Create();
			element->Rendering.AttachMethod(this, &Direct2DWindow::element_Rendering);
			element->BeforeRenderTargetChanged.AttachMethod(this, &Direct2DWindow::element_BeforeRenderTargetChanged);
			element->AfterRenderTargetChanged.AttachMethod(this, &Direct2DWindow::element_AfterRenderTargetChanged);

			GuiBoundsComposition* composition=new GuiBoundsComposition;
			composition->SetAlignmentToParent(Margin(0, 0, 0, 0));
			composition->SetOwnedElement(element);
			GetContainerComposition()->AddChild(composition);
		}
	}
};

void GuiMain()
{
	Direct2DWindow window;
	GetApplication()->Run(&window);
}