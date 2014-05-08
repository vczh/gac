#include "UI\Demo.h"
#include <Windows.h>

using namespace vl::collections;
using namespace vl::reflection::description;
using namespace demos;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

class Season :public Object, public virtual ISeason
{
protected:
	WString								season;
	WString								description;
	list::ObservableList<Ptr<ISeason>>	children;

public:
	Season(const WString& _season, const WString& _description)
		:season(_season)
		, description(_description)
	{
	}

	WString GetSeason()override
	{
		return season;
	}

	WString GetDescription()override
	{
		return description;
	}

	Ptr<IValueObservableList> GetChildren()override
	{
		return children.GetWrapper();
	}

	void Add(Ptr<Season> child)
	{
		children.Add(child);
	}
};

class ViewModel : public Object, public virtual IViewModel
{
protected:
	list::ObservableList<WString>					seasons;
	list::ObservableList<Ptr<ISeason>>				complexSeasons;
	Ptr<Season>										treeSeasons;

public:
	ViewModel()
	{
		seasons.Add(L"Spring");
		seasons.Add(L"Summer");
		seasons.Add(L"Autumn");
		seasons.Add(L"Winter");

		complexSeasons.Add(new Season(L"Spring", L"Mar - May"));
		complexSeasons.Add(new Season(L"Summer", L"Jun - Aug"));
		complexSeasons.Add(new Season(L"Autumn", L"Sep - Nov"));
		complexSeasons.Add(new Season(L"Winter", L"Dec - Feb"));

		treeSeasons = new Season(L"", L"");
		treeSeasons->Add(new Season(L"Spring", L"Mar - May"));
		treeSeasons->Add(new Season(L"Summer", L"Jun - Aug"));
		treeSeasons->Add(new Season(L"Autumn", L"Sep - Nov"));
		treeSeasons->Add(new Season(L"Winter", L"Dec - Feb"));
	}

	Ptr<IValueObservableList> GetSeasons()override
	{
		return seasons.GetWrapper();
	}

	Ptr<IValueObservableList> GetComplexSeasons()override
	{
		return complexSeasons.GetWrapper();
	}

	Ptr<ISeason> GetTreeSeasons()override
	{
		return treeSeasons;
	}

	void AddSeason()override
	{
		seasons.Add(L"Unknown Season No." + itow(seasons.Count() + 1));
	}

	void AddComplexSeason()override
	{
		complexSeasons.Add(new Season(L"Unknown Season No." + itow(complexSeasons.Count() + 1), L"N/A"));
	}

	void AddTreeSeason()override
	{
		auto first = UnboxValue<Ptr<ISeason>>(treeSeasons->GetChildren()->Get(0)).Cast<Season>();
		first->Add(new Season(L"Unknown Season No." + itow(first->GetChildren()->GetCount() + 1), L"N/A"));
	}
};

void GuiMain()
{
	List<WString> errors;
	GetInstanceLoaderManager()->SetResource(L"Resource", GuiResource::LoadFromXml(L"..\\Resources\\XmlWindowResourceListViewModel.xml", errors));
	MainWindow window(new ViewModel);
	window.ForceCalculateSizeImmediately();
	window.MoveToScreenCenter();
	GetApplication()->Run(&window);
}