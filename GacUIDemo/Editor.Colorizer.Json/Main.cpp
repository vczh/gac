#include "..\..\Public\Source\GacUI.h"
#include "..\..\Public\Source\Vlpp.h"
#include <Windows.h>

using namespace vl;
using namespace vl::collections;
using namespace vl::stream;
using namespace vl::regex;
using namespace vl::parsing;
using namespace vl::parsing::tabling;
using namespace vl::parsing::definitions;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

/***********************************************************************
JsonGrammarColorizer
***********************************************************************/

class JsonGrammarColorizer : public GuiGrammarColorizer
{
public:
	JsonGrammarColorizer()
		:GuiGrammarColorizer(CreateAutoRecoverParser(json::JsonLoadTable()), L"JRoot")
	{
		SetColor(L"Boundary", Color(255, 0, 0));
		SetColor(L"Keyword", Color(0, 0, 255));
		SetColor(L"AttName", Color(64, 64, 64));
		SetColor(L"Number", Color(128, 0, 255));
		SetColor(L"String", Color(163, 21, 2));
		EndSetColors();
	}
};

/***********************************************************************
TextBoxColorizerWindow
***********************************************************************/

class TextBoxColorizerWindow : public GuiWindow
{
protected:
	GuiMultilineTextBox*					textBoxEditor;

public:
	TextBoxColorizerWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		SetText(L"Editor.Colorizer.Json");
		SetClientSize(Size(800, 600));

		textBoxEditor=g::NewMultilineTextBox();
		textBoxEditor->SetVerticalAlwaysVisible(false);
		textBoxEditor->SetHorizontalAlwaysVisible(false);
		textBoxEditor->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
		textBoxEditor->SetColorizer(new JsonGrammarColorizer);
		this->GetBoundsComposition()->AddChild(textBoxEditor->GetBoundsComposition());

		{
			FileStream fileStream(L"..\\Resources\\JsonSample.txt", FileStream::ReadOnly);
			BomDecoder decoder;
			DecoderStream decoderStream(fileStream, decoder);
			StreamReader reader(decoderStream);
			textBoxEditor->SetText(reader.ReadToEnd());
			textBoxEditor->Select(TextPos(), TextPos());
		}

		// set the preferred minimum client 600
		this->GetBoundsComposition()->SetPreferredMinSize(Size(800, 600));
		// call this to calculate the size immediately if any indirect content in the table changes
		// so that the window can calcaulte its correct size before calling the MoveToScreenCenter()
		this->ForceCalculateSizeImmediately();
		// move to the screen center
		this->MoveToScreenCenter();
	}

	~TextBoxColorizerWindow()
	{
	}
};

/***********************************************************************
GuiMain
***********************************************************************/

void GuiMain()
{
	TextBoxColorizerWindow window;
	GetApplication()->Run(&window);
}