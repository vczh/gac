#include "..\..\Public\Source\VlppWorkflow.h"
#include "..\..\Public\Source\GacUIReflection.h"
#include <Windows.h>

using namespace vl::collections;
using namespace vl::reflection::description;
using namespace vl::parsing;
using namespace vl::workflow;
using namespace vl::workflow::analyzer;
using namespace vl::workflow::runtime;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

void GuiMain()
{
	List<Ptr<ParsingError>> errors;
	List<WString> codes;
	codes.Add(LR"workflow(
module helloworld;
using system::*;
using system::reflection::*;
using presentation::*;
using presentation::controls::*;
using presentation::compositions::*;

func AddRow(table : GuiTableComposition*, row : int, title : string) : GuiSinglelineTextBox*
{
	{
		var cell = new GuiCellComposition*();
		table.AddChild(cell);
		cell.SetSite(row, 0, 1, 1);

		var label = new GuiLabel*();
		label.Text = title;
		label.BoundsComposition.AlignmentToParent = cast Margin "left:0 top:0 right:0 bottom:0";
		cell.AddChild(label.BoundsComposition);
	}
	{
		var cell = new GuiCellComposition*();
		table.AddChild(cell);
		cell.SetSite(row, 1, 1, 1);

		var textBox = new GuiSinglelineTextBox*();
		textBox.BoundsComposition.AlignmentToParent = cast Margin "left:0 top:0 right:0 bottom:0";
		textBox.BoundsComposition.PreferredMinSize = cast Size $"x:0 y:$(textBox.Font.size * 2)";
		cell.AddChild(textBox.BoundsComposition);
		return textBox;
	}
}

func CreateWindow() : GuiWindow*
{
	var window = new GuiWindow*();
	window.Text = "Scriptable GacUI!";
	window.ContainerComposition.PreferredMinSize = cast Size "x:300 y:200";

	var table = new GuiTableComposition*();
	window.ContainerComposition.AddChild(table);
	table.AlignmentToParent = cast Margin "left:0 top:0 right:0 bottom:0";
	table.CellPadding = 5;
	table.SetRowsAndColumns(4, 2);

	for(row in range[0, 2])
	{
		table.SetRowOption(row, cast GuiCellOption "composeType:MinSize");
	}
	table.SetColumnOption(0, cast GuiCellOption "composeType:MinSize");
	table.SetColumnOption(1, cast GuiCellOption "composeType:Percentage percentage:1.0");

	var textBox1 = AddRow(table, 0, "A = ");
	var textBox2 = AddRow(table, 1, "B = ");
	var textBox3 = AddRow(table, 2, "A + B = ");
	textBox1.Text = 1;
	textBox2.Text = 2;
	textBox3.Readonly = true;

	var subscription = window.AddSubscription(bind(cast int textBox1.Text + cast int textBox2.Text ?? "<error>"));
	subscription.Subscribe(func(value : object):void
	{
		textBox3.Text = cast string value;
	});
	subscription.Update();

	window.ForceCalculateSizeImmediately();
	window.MoveToScreenCenter();
	return window;
}

)workflow");

	WfLoadTypes();
	auto table = WfLoadTable();
	auto assembly = Compile(table, codes, errors);
	auto globalContext = MakePtr<WfRuntimeGlobalContext>(assembly);
	auto threadContext = MakePtr<WfRuntimeThreadContext>(globalContext);

	auto initializeFunction = LoadFunction<void()>(globalContext, L"<initialize>");
	auto createWindowFunction = LoadFunction<GuiWindow*()>(globalContext, L"CreateWindow");
	initializeFunction();
	Ptr<GuiWindow> window = createWindowFunction();
	GetApplication()->Run(window.Obj());
}