#include "UI\DataGridModel.h"
#include <Windows.h>

using namespace vl::collections;
using namespace vl::reflection::description;
using namespace vl::stream;
using namespace demos;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

class DataGridModel : public Object, public virtual IDataGridModel
{
protected:
	List<WString>					titles;
	List<Ptr<DataGridModelItem>>	people;

public:
	DataGridModel()
	{
		{
			titles.Add(L"CAO (Chief Algorithm Officer)");
			titles.Add(L"Principal Fisher");
			titles.Add(L"Software Engineer");
			titles.Add(L"Sponsor");
		}
		{
			auto item = MakePtr<DataGridModelItem>();
			item->name = L"Kula";
			item->organization = L"Opera";
			item->title = L"CAO (Chief Algorithm Officer)";
			people.Add(item);
		}
		{
			auto item = MakePtr<DataGridModelItem>();
			item->name = L"JeffChen";
			item->organization = L"Tencent";
			item->title = L"Principal Fisher";
			people.Add(item);
		}
		{
			auto item = MakePtr<DataGridModelItem>();
			item->name = L"MiliMeow";
			item->organization = L"Microsoft";
			item->title = L"Software Engineer";
			people.Add(item);
		}
		{
			auto item = MakePtr<DataGridModelItem>();
			item->name = L"Skogkatt";
			item->organization = L"360";
			item->title = L"Sponsor";
			people.Add(item);
		}
	}

	List<WString>& GetTitles()override
	{
		return titles;
	}

	List<Ptr<DataGridModelItem>>& GetPeople()override
	{
		return people;
	}
};

void GuiMain()
{
	{
		List<WString> errors;
		FileStream fileStream(L"..\\Resources\\XmlWindowDataGridModel.precompiled.compressed", FileStream::ReadOnly);
		LzwDecoder decoder;
		DecoderStream decoderStream(fileStream, decoder);
		auto resource = GuiResource::LoadPrecompiledBinary(decoderStream, errors);
		GetInstanceLoaderManager()->SetResource(L"Resource", resource);
	}

	MainWindow window(new DataGridModel);
	window.ForceCalculateSizeImmediately();
	window.MoveToScreenCenter();
	GetApplication()->Run(&window);
}