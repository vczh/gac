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
XmlGrammarColorizer
***********************************************************************/

class XmlGrammarColorizer : public GuiGrammarColorizer
{
public:
	XmlGrammarColorizer()
		:GuiGrammarColorizer(CreateAutoRecoverParser(xml::XmlLoadTable()), L"XDocument")
	{
		SetColor(L"Boundary", Color(0, 0, 255));
		SetColor(L"Comment", Color(0, 128, 0));
		SetColor(L"TagName", Color(163, 21, 21));
		SetColor(L"AttName", Color(255, 0, 0));
		SetColor(L"AttValue", Color(128, 0, 255));
		EndSetColors();
	}
};

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
SymbolLookup
***********************************************************************/

class TypeSymbol : public Object
{
public:
	WString									typeName;
	Dictionary<WString, Ptr<TypeSymbol>>	subTypes;
	TypeSymbol*								parent;

	TypeSymbol()
		:parent(0)
	{
	}

	void CollectSubTypes(Ptr<ParsingTreeArray> types, Dictionary<ParsingTreeNode*, TypeSymbol*>& nodeTypeMap)
	{
		if(types)
		{
			for(int i=0;i<types->Count();i++)
			{
				Ptr<ParsingTreeObject> type=types->GetItem(i).Cast<ParsingTreeObject>();
				if(type)
				{
					CollectSubType(type, nodeTypeMap);
				}
			}
		}
	}

	void CollectSubType(Ptr<ParsingTreeObject> type, Dictionary<ParsingTreeNode*, TypeSymbol*>& nodeTypeMap)
	{
		Ptr<ParsingTreeToken> name=type->GetMember(L"name").Cast<ParsingTreeToken>();
		if(name && !subTypes.Keys().Contains(name->GetValue()))
		{
			Ptr<TypeSymbol> symbol=new TypeSymbol;
			symbol->typeName=name->GetValue();
			symbol->parent=this;
			subTypes.Add(symbol->typeName, symbol);
			symbol->CollectSubTypes(type->GetMember(L"subTypes").Cast<ParsingTreeArray>(), nodeTypeMap);
			nodeTypeMap.Add(type.Obj(), symbol.Obj());
		}
	}
};

class ParserDecl : public TypeSymbol
{
public:
	SortedList<WString>							tokens;
	SortedList<WString>							rules;
	Dictionary<ParsingTreeNode*, TypeSymbol*>	nodeTypeMap;

	ParserDecl(Ptr<ParsingTreeObject> parserDecl)
	{
		nodeTypeMap.Add(parserDecl.Obj(), this);
		Ptr<ParsingTreeArray> defs=parserDecl->GetMember(L"definitions").Cast<ParsingTreeArray>();
		if(defs)
		{
			vint count=defs->Count();
			for(vint i=0;i<count;i++)
			{
				Ptr<ParsingTreeObject> defObject=defs->GetItem(i).Cast<ParsingTreeObject>();
				if(defObject)
				{
					if(defObject->GetType()==L"TokenDef")
					{
						Ptr<ParsingTreeToken> name=defObject->GetMember(L"name").Cast<ParsingTreeToken>();
						if(name)
						{
							tokens.Add(name->GetValue());
						}
					}
					else if(defObject->GetType()==L"RuleDef")
					{
						Ptr<ParsingTreeToken> name=defObject->GetMember(L"name").Cast<ParsingTreeToken>();
						if(name)
						{
							rules.Add(name->GetValue());
						}
					}
					else
					{
						CollectSubType(defObject, nodeTypeMap);
					}
				}
			}
		}
	}
};

/***********************************************************************
ParserGrammarColorizer
***********************************************************************/

class ParserGrammarColorizer : public GuiGrammarColorizer
{
protected:
	Ptr<ParserDecl>							parsingTreeDecl;
	vint									tokenIdType;
	vint									tokenIdToken;
	vint									tokenIdRule;
	vint									semanticType;
	vint									semanticGrammar;

	TypeSymbol* FindScope(ParsingTreeNode* node)
	{
		if(!node) return 0;
		int index=parsingTreeDecl->nodeTypeMap.Keys().IndexOf(node);
		return index==-1?FindScope(node->GetParent()):parsingTreeDecl->nodeTypeMap.Values().Get(index);
	}

	TypeSymbol* FindType(TypeSymbol* scope, const WString& name)
	{
		if(!scope) return 0;
		if(name==L"") return 0;
		int index=scope->subTypes.Keys().IndexOf(name);
		if(index!=-1) return scope->subTypes.Values().Get(index).Obj();
		return FindType(scope->parent, name);
	}

	TypeSymbol* FindType(TypeSymbol* scope, ParsingTreeObject* object)
	{
		if(scope && object)
		{
			Ptr<ParsingTreeToken> name=object->GetMember(L"name").Cast<ParsingTreeToken>();
			if(name)
			{
				WString typeName=name->GetValue();
				if(object->GetType()==L"PrimitiveTypeObj")
				{
					return FindType(scope, typeName);
				}
				else if(object->GetType()==L"SubTypeObj")
				{
					TypeSymbol* type=FindType(scope, object->GetMember(L"parentType").Cast<ParsingTreeObject>().Obj());
					if(type)
					{
						int index=type->subTypes.Keys().IndexOf(typeName);
						if(index!=-1) return type->subTypes.Values().Get(index).Obj();
					}
				}
			}
		}
		return 0;
	}

	void OnParsingFinished(bool generatedNewNode, RepeatingParsingExecutor* parsingExecutor)override
	{
		if(generatedNewNode)
		{
			Ptr<ParsingTreeObject> node=parsingExecutor->ThreadSafeGetTreeNode();
			parsingTreeDecl=new ParserDecl(node);
			node=0;
			parsingExecutor->ThreadSafeReturnTreeNode();
		}
		GuiGrammarColorizer::OnParsingFinished(generatedNewNode, parsingExecutor);
	}

	void OnSemanticColorize(ParsingTreeToken* foundToken, ParsingTreeObject* tokenParent, const WString& type, const WString& field, vint semantic, vint& token)override
	{
		if(semantic==semanticType)
		{
			TypeSymbol* scope=FindScope(tokenParent);
			if(FindType(scope, tokenParent))
			{
				token=tokenIdType;
			}
		}
		else if(semantic==semanticGrammar)
		{
			WString name=foundToken->GetValue();
			if(parsingTreeDecl->tokens.Contains(name))
			{
				token=tokenIdToken;
			}
			else if(parsingTreeDecl->rules.Contains(name))
			{
				token=tokenIdRule;
			}
		}
	}
public:
	ParserGrammarColorizer()
		:GuiGrammarColorizer(CreateBootstrapAutoRecoverParser(), L"ParserDecl")
	{
		SetColor(L"Keyword", Color(0, 0, 255));
		SetColor(L"Attribute", Color(0, 0, 255));
		SetColor(L"String", Color(163, 21, 21));
		SetColor(L"Type", Color(43, 145, 175));
		SetColor(L"Token", Color(163, 73, 164));
		SetColor(L"Rule", Color(255, 127, 39));
		EndSetColors();

		tokenIdType=GetTokenId(L"Type");
		tokenIdToken=GetTokenId(L"Token");
		tokenIdRule=GetTokenId(L"Rule");
		semanticType=GetSemanticId(L"Type");
		semanticGrammar=GetSemanticId(L"Grammar");
	}

	~ParserGrammarColorizer()
	{
		EnsureColorizerFinished();
	}
};

/***********************************************************************
ColorizerConfigurationGrid
***********************************************************************/

class ColorizerConfigurationGrid : public GuiVirtualDataGrid
{
public:
	struct ColorItem
	{
		WString								name;
		Color								color;
	};

	class ColorColumnProvider : public list::StrongTypedFieldColumnProvider<ColorItem, Color>
	{
	public:
		ColorColumnProvider(list::StrongTypedDataProvider<ColorItem>* _dataProvider)
			:list::StrongTypedFieldColumnProvider<ColorItem, Color>(_dataProvider, &ColorItem::color)
		{
		}

		void VisualizeCell(vint row, list::IDataVisualizer* dataVisualizer)override
		{
			list::StrongTypedFieldColumnProvider<ColorItem, Color>::VisualizeCell(row, dataVisualizer);
			list::ListViewSubColumnDataVisualizer* visualizer=dataVisualizer->GetVisualizer<list::ListViewSubColumnDataVisualizer>();
			if(visualizer)
			{
				ColorItem rowData;
				Color cellData;
				dataProvider->GetRowData(row, rowData);
				GetCellData(rowData, cellData);
				visualizer->GetTextElement()->SetColor(cellData);
			}
		}
	};

	class ColorItemProvider : public list::StrongTypedDataProvider<ColorItem>
	{
	protected:
		List<ColorItem>						items;
	public:
		ColorItemProvider()
		{
			AddSortableFieldColumn(L"Configuration", &ColorItem::name)
				->SetVisualizerFactory(new list::CellBorderDataVisualizer::Factory(new list::ListViewMainColumnDataVisualizer::Factory))
				->SetSize(160);
			AddStrongTypedColumn<Color>(L"Color", new ColorColumnProvider(this))
				->SetVisualizerFactory(new list::CellBorderDataVisualizer::Factory(new list::ListViewSubColumnDataVisualizer::Factory))
				->SetSize(120);
		}

		vint GetRowCount()override
		{
			return items.Count();
		}

		void GetRowData(vint row, ColorItem& rowData)override
		{
			rowData=items[row];
		}

		void ReadConfigurationFromColorizer(GuiGrammarColorizer* colorizer)
		{
			vint oldCount=items.Count();
			items.Clear();
			FOREACH(WString, name, colorizer->GetColorNames())
			{
				ColorItem item;
				item.name=name;
				item.color=colorizer->GetColor(name).normal.text;
				items.Add(item);
			}
			commandExecutor->OnDataProviderItemModified(0, oldCount, items.Count());
		}
	};

protected:
	ColorItemProvider*						dataProvider;
public:
	ColorizerConfigurationGrid()
		:GuiVirtualDataGrid(GetCurrentTheme()->CreateListViewStyle(), dataProvider=new ColorItemProvider)
	{
	}

	ColorItemProvider* GetDataProvider()
	{
		return dataProvider;
	}
};

/***********************************************************************
TextBoxColorizerWindow
***********************************************************************/

class TextBoxColorizerWindow : public GuiWindow
{
protected:
	GuiSelectableButton*					radioGrammar;
	GuiSelectableButton*					radioXml;
	GuiSelectableButton*					radioJson;
	GuiTab*									tabIntellisense;
	GuiMultilineTextBox*					textBoxGrammar;
	GuiMultilineTextBox*					textBoxEditor;
	ColorizerConfigurationGrid*				gridConfiguration;

	void SwitchLanguage(const WString& sampleCodePath, GuiGrammarColorizer* colorizer, const WString& grammarCode)
	{
		{
			// clear the colorizer first in order to prevent the previous colorizer from parsing code in another language
			// which always wastes time in error recovering
			textBoxEditor->SetColorizer(0);

			// paste the code in another language
			FileStream fileStream(sampleCodePath, FileStream::ReadOnly);
			BomDecoder decoder;
			DecoderStream decoderStream(fileStream, decoder);
			StreamReader reader(decoderStream);
			textBoxEditor->SetText(reader.ReadToEnd());
			textBoxEditor->Select(TextPos(), TextPos());

			// set the new colorizer that fit the language
			textBoxEditor->SetColorizer(colorizer);
		}
		{
			textBoxGrammar->SetText(grammarCode);
			textBoxGrammar->Select(TextPos(), TextPos());
		}
		gridConfiguration->GetDataProvider()->ReadConfigurationFromColorizer(colorizer);
	}

	void SwitchLanguage(const WString& sampleCodePath, GuiGrammarColorizer* colorizer, Ptr<ParsingDefinition> definition)
	{
		MemoryStream stream;
		{
			StreamWriter writer(stream);
			Log(definition, writer);
		}
		stream.SeekFromBegin(0);
		StreamReader reader(stream);
		SwitchLanguage(sampleCodePath, colorizer, reader.ReadToEnd());
	}

	void radioGrammar_SelectedChanged(GuiGraphicsComposition* composition, GuiEventArgs& arguments)
	{
		if(radioGrammar->GetSelected())
		{
			SwitchLanguage(L"..\\Resources\\CalculatorDefinition.txt", new ParserGrammarColorizer, CreateParserDefinition());
		}
	}

	void radioXml_SelectedChanged(GuiGraphicsComposition* composition, GuiEventArgs& arguments)
	{
		if(radioXml->GetSelected())
		{
			SwitchLanguage(L"..\\Resources\\XmlResource.xml", new XmlGrammarColorizer, xml::XmlGetParserTextBuffer());
		}
	}

	void radioJson_SelectedChanged(GuiGraphicsComposition* composition, GuiEventArgs& arguments)
	{
		if(radioJson->GetSelected())
		{
			SwitchLanguage(L"..\\Resources\\JsonSample.txt", new JsonGrammarColorizer, json::JsonGetParserTextBuffer());
		}
	}
public:
	TextBoxColorizerWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		SetText(L"Controls.TextBox.ContextColorizer");
		SetClientSize(Size(800, 600));

		GuiSelectableButton::MutexGroupController* controller=new GuiSelectableButton::MutexGroupController;
		AddComponent(controller);

		GuiTableComposition* table=new GuiTableComposition;
		table->SetAlignmentToParent(Margin(0, 0, 0, 0));
		GetBoundsComposition()->AddChild(table);

		table->SetCellPadding(5);
		table->SetRowsAndColumns(2, 4);
		table->SetRowOption(0, GuiCellOption::MinSizeOption());
		table->SetRowOption(1, GuiCellOption::PercentageOption(1.0));
		table->SetColumnOption(0, GuiCellOption::MinSizeOption());
		table->SetColumnOption(1, GuiCellOption::MinSizeOption());
		table->SetColumnOption(2, GuiCellOption::MinSizeOption());
		table->SetColumnOption(3, GuiCellOption::PercentageOption(1.0));
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 0, 1, 1);

			radioGrammar=g::NewRadioButton();
			radioGrammar->SetGroupController(controller);
			radioGrammar->SetText(L"Grammar");
			radioGrammar->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			radioGrammar->SelectedChanged.AttachMethod(this, &TextBoxColorizerWindow::radioGrammar_SelectedChanged);
			cell->AddChild(radioGrammar->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 1, 1, 1);

			radioXml=g::NewRadioButton();
			radioXml->SetGroupController(controller);
			radioXml->SetText(L"Xml");
			radioXml->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			radioXml->SelectedChanged.AttachMethod(this, &TextBoxColorizerWindow::radioXml_SelectedChanged);
			cell->AddChild(radioXml->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(0, 2, 1, 1);

			radioJson=g::NewRadioButton();
			radioJson->SetGroupController(controller);
			radioJson->SetText(L"Json");
			radioJson->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			radioJson->SelectedChanged.AttachMethod(this, &TextBoxColorizerWindow::radioJson_SelectedChanged);
			cell->AddChild(radioJson->GetBoundsComposition());
		}
		{
			GuiCellComposition* cell=new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(1, 0, 1, 4);

			tabIntellisense=g::NewTab();
			tabIntellisense->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			cell->AddChild(tabIntellisense->GetBoundsComposition());
			{
				GuiTabPage* page=tabIntellisense->CreatePage();
				page->SetText(L"Code Editor");

				textBoxEditor=g::NewMultilineTextBox();
				textBoxEditor->SetHorizontalAlwaysVisible(false);
				textBoxEditor->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
				page->GetContainer()->GetBoundsComposition()->AddChild(textBoxEditor->GetBoundsComposition());
			}
			{
				GuiTabPage* page=tabIntellisense->CreatePage();
				page->SetText(L"Grammar");

				textBoxGrammar=g::NewMultilineTextBox();
				textBoxGrammar->SetReadonly(true);
				textBoxGrammar->SetHorizontalAlwaysVisible(false);
				textBoxGrammar->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
				page->GetContainer()->GetBoundsComposition()->AddChild(textBoxGrammar->GetBoundsComposition());
				textBoxGrammar->SetColorizer(new ParserGrammarColorizer);
			}
			{
				GuiTabPage* page=tabIntellisense->CreatePage();
				page->SetText(L"Configuration");

				gridConfiguration=new ColorizerConfigurationGrid;
				gridConfiguration->SetVerticalAlwaysVisible(false);
				gridConfiguration->SetHorizontalAlwaysVisible(false);
				gridConfiguration->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
				page->GetContainer()->GetBoundsComposition()->AddChild(gridConfiguration->GetBoundsComposition());
			}
		}
		radioGrammar->SetSelected(true);

		// set the preferred minimum client 600
		this->GetBoundsComposition()->SetPreferredMinSize(Size(800, 480));
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
	GuiWindow* window=new TextBoxColorizerWindow();
	GetApplication()->Run(window);
	delete window;
}