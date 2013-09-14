#include "..\..\Public\Source\GacUI.h"
#include "..\..\Public\Source\Vlpp.h"
#include <Windows.h>

using namespace vl;
using namespace vl::collections;
using namespace vl::stream;
using namespace vl::regex;
using namespace vl::regex_internal;
using namespace vl::parsing;
using namespace vl::parsing::tabling;
using namespace vl::parsing::definitions;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

/***********************************************************************
SymbolLookup
***********************************************************************/

Ptr<ParsingScopeSymbol> CreateSymbolFromNode(Ptr<ParsingTreeObject> obj, RepeatingParsingExecutor* executor, ParsingScopeFinder* finder);
void CreateSubSymbols(ParsingScopeSymbol* symbol, Ptr<ParsingTreeObject> node, const WString& memberName, RepeatingParsingExecutor* executor, ParsingScopeFinder* finder);

class GrammarSymbol : public ParsingScopeSymbol
{
public:
	GrammarSymbol(Ptr<ParsingTreeObject> node, RepeatingParsingExecutor* executor, ParsingScopeFinder* finder, const WString& semantic)
		:ParsingScopeSymbol(finder->Node(node->GetMember(L"name")).Cast<ParsingTreeToken>()->GetValue(), executor->GetSemanticId(semantic))
	{
		SetNode(node);
	}
};

class EnumFieldSymbol : public GrammarSymbol
{
protected:
	WString GetDisplayInternal(vint semanticId)
	{
		return literalString;
	}
public:
	WString literalString;

	EnumFieldSymbol(Ptr<ParsingTreeObject> node, RepeatingParsingExecutor* executor, ParsingScopeFinder* finder)
		:GrammarSymbol(node, executor, finder, L"EnumValue")
	{
		WString value=finder->Node(node->GetMember(L"name")).Cast<ParsingTreeToken>()->GetValue();
		literalString=SerializeString(value);
	}
};

class ClassFieldSymbol : public GrammarSymbol
{
public:
	ClassFieldSymbol(Ptr<ParsingTreeObject> node, RepeatingParsingExecutor* executor, ParsingScopeFinder* finder)
		:GrammarSymbol(node, executor, finder, L"Field")
	{
	}
};

class TypeSymbol : public GrammarSymbol
{
public:
	TypeSymbol(Ptr<ParsingTreeObject> node, RepeatingParsingExecutor* executor, ParsingScopeFinder* finder)
		:GrammarSymbol(node, executor, finder, L"Type")
	{
	}
};

class EnumSymbol : public TypeSymbol
{
public:
	EnumSymbol(Ptr<ParsingTreeObject> node, RepeatingParsingExecutor* executor, ParsingScopeFinder* finder)
		:TypeSymbol(node, executor, finder)
	{
		CreateScope();
		CreateSubSymbols(this, node, L"members", executor, finder);
	}
};

class ClassSymbol : public TypeSymbol
{
public:
	ClassSymbol(Ptr<ParsingTreeObject> node, RepeatingParsingExecutor* executor, ParsingScopeFinder* finder)
		:TypeSymbol(node, executor, finder)
	{
		CreateScope();
		CreateSubSymbols(this, node, L"members", executor, finder);
		CreateSubSymbols(this, node, L"subTypes", executor, finder);
	}
};

class TokenSymbol : public GrammarSymbol
{
protected:
	WString GetDisplayInternal(vint semanticId)
	{
		return semanticId==literalId?literalString:ParsingScopeSymbol::GetDisplayInternal(semanticId);
	}
public:
	vint literalId;
	WString literalString;

	TokenSymbol(Ptr<ParsingTreeObject> node, RepeatingParsingExecutor* executor, ParsingScopeFinder* finder)
		:GrammarSymbol(node, executor, finder, L"Token")
		,literalId(-1)
	{
		WString value=finder->Node(node->GetMember(L"regex")).Cast<ParsingTreeToken>()->GetValue();
		WString regex=DeserializeString(value);
		if(IsRegexEscapedListeralString(regex))
		{
			literalString=SerializeString(UnescapeTextForRegex(regex));
			AddSemanticId(literalId=executor->GetSemanticId(L"Literal"));
		}
	}
};

class RuleSymbol : public GrammarSymbol
{
public:
	RuleSymbol(Ptr<ParsingTreeObject> node, RepeatingParsingExecutor* executor, ParsingScopeFinder* finder)
		:GrammarSymbol(node, executor, finder, L"Rule")
	{
	}
};

class ParserDefSymbol : public ParsingScopeSymbol
{
public:
	ParserDefSymbol(Ptr<ParsingTreeObject> node, RepeatingParsingExecutor* executor, ParsingScopeFinder* finder)
	{
		SetNode(node.Obj());
		CreateScope();
		CreateSubSymbols(this, node, L"definitions", executor, finder);
	}
};


void CreateSubSymbols(ParsingScopeSymbol* symbol, Ptr<ParsingTreeObject> node, const WString& memberName, RepeatingParsingExecutor* executor, ParsingScopeFinder* finder)
{
	if(Ptr<ParsingTreeArray> members=finder->Node(node->GetMember(memberName)).Cast<ParsingTreeArray>())
	{
		FOREACH(Ptr<ParsingTreeNode>, node, members->GetItems())
		{
			if(Ptr<ParsingTreeObject> obj=finder->Node(node).Cast<ParsingTreeObject>())
			{
				symbol->GetScope()->AddSymbol(CreateSymbolFromNode(obj, executor, finder));
			}
		}
	}
}

Ptr<ParsingScopeSymbol> CreateSymbolFromNode(Ptr<ParsingTreeObject> obj, RepeatingParsingExecutor* executor, ParsingScopeFinder* finder)
{
	if(obj->GetType()==L"EnumMemberDef")
	{
		return new EnumFieldSymbol(obj, executor, finder);
	}
	else if(obj->GetType()==L"EnumTypeDef")
	{
		return new EnumSymbol(obj, executor, finder);
	}
	else if(obj->GetType()==L"ClassMemberDef")
	{
		return new ClassFieldSymbol(obj, executor, finder);
	}
	else if(obj->GetType()==L"ClassTypeDef")
	{
		return new ClassSymbol(obj, executor, finder);
	}
	else if(obj->GetType()==L"TokenDef")
	{
		return new TokenSymbol(obj, executor, finder);
	}
	else if(obj->GetType()==L"RuleDef")
	{
		return new RuleSymbol(obj, executor, finder);
	}
	else if(obj->GetType()==L"ParserDef")
	{
		return new ParserDefSymbol(obj, executor, finder);
	}
	else
	{
		return 0;
	}
}

LazyList<Ptr<ParsingScopeSymbol>> FindReferencedSymbols(ParsingTreeObject* obj, ParsingScopeFinder* finder)
{
	ParsingScope* scope=finder->GetScopeFromNode(obj);
	if(obj->GetType()==L"PrimitiveTypeObj")
	{
		WString name=obj->GetMember(L"name").Cast<ParsingTreeToken>()->GetValue();
		return finder->GetSymbolsRecursively(scope, name);
	}
	else if(obj->GetType()==L"SubTypeObj")
	{
		if(Ptr<ParsingTreeObject> parentType=obj->GetMember(L"parentType").Cast<ParsingTreeObject>())
		{
			WString name=obj->GetMember(L"name").Cast<ParsingTreeToken>()->GetValue();
			LazyList<Ptr<ParsingScopeSymbol>> types=FindReferencedSymbols(parentType.Obj(), finder);
			return types
				.SelectMany([=](Ptr<ParsingScopeSymbol> type)
				{
					return finder->GetSymbols(type->GetScope(), name);
				});
		}
	}
	else if(obj->GetType()==L"PrimitiveGrammarDef")
	{
		WString name=obj->GetMember(L"name").Cast<ParsingTreeToken>()->GetValue();
		return finder->GetSymbolsRecursively(scope, name);
	}
	return LazyList<Ptr<ParsingScopeSymbol>>();
}

/***********************************************************************
GrammarLanguageProvider
***********************************************************************/

class GrammarLanguageProvider : public Object, public ILanguageProvider
{
public:
	Ptr<ParsingScopeSymbol> CreateSymbolFromNode(Ptr<ParsingTreeObject> obj, RepeatingParsingExecutor* executor, ParsingScopeFinder* finder)
	{
		return ::CreateSymbolFromNode(obj, executor, finder);
	}

	LazyList<Ptr<ParsingScopeSymbol>> FindReferencedSymbols(ParsingTreeObject* obj, parsing::ParsingScopeFinder* finder)
	{
		return ::FindReferencedSymbols(obj, finder);
	}

	LazyList<Ptr<ParsingScopeSymbol>> FindPossibleSymbols(ParsingTreeObject* obj, const WString& field, ParsingScopeFinder* finder)
	{
		return LazyList<Ptr<ParsingScopeSymbol>>();
	}
};

/***********************************************************************
ParserGrammarExecutor
***********************************************************************/

class ParserGrammarExecutor : public RepeatingParsingExecutor
{
protected:

	void OnContextFinishedAsync(RepeatingParsingOutput& context)override
	{
		context.finder=new ParsingScopeFinder();
		context.symbol=CreateSymbolFromNode(context.node, this, context.finder.Obj());
		context.finder->InitializeQueryCache(context.symbol.Obj());
	}
public:
	ParserGrammarExecutor()
		:RepeatingParsingExecutor(CreateBootstrapAutoRecoverParser(), L"ParserDecl", new GrammarLanguageProvider)
	{
	}
};

/***********************************************************************
ParserGrammarColorizer
***********************************************************************/

class ParserGrammarColorizer : public GuiGrammarColorizer
{
public:
	ParserGrammarColorizer(Ptr<ParserGrammarExecutor> executor)
		:GuiGrammarColorizer(executor)
	{
		SetColor(L"Keyword", Color(0, 0, 255));
		SetColor(L"Attribute", Color(0, 0, 255));
		SetColor(L"String", Color(163, 21, 21));
		SetColor(L"Type", Color(43, 145, 175));
		SetColor(L"Token", Color(163, 73, 164));
		SetColor(L"Rule", Color(255, 127, 39));
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
		SetText(L"Editor.Colorizer.Grammar");
		SetClientSize(Size(800, 600));

		textBoxEditor=g::NewMultilineTextBox();
		textBoxEditor->SetVerticalAlwaysVisible(false);
		textBoxEditor->SetHorizontalAlwaysVisible(false);
		textBoxEditor->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
		textBoxEditor->SetColorizer(new ParserGrammarColorizer(new ParserGrammarExecutor));
		this->GetBoundsComposition()->AddChild(textBoxEditor->GetBoundsComposition());

		{
			FileStream fileStream(L"..\\Resources\\CalculatorDefinition.txt", FileStream::ReadOnly);
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