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

typedef List<Ptr<ParsingScopeSymbol>> TypeList;
typedef Ptr<TypeList> PtrTypeList;

PtrTypeList SearchAllTypes(ParsingTreeObject* obj, ParsingScopeFinder* finder)
{
	PtrTypeList allTypes=new TypeList;
	ParsingScope* scope=finder->GetScopeFromNode(obj);
	while(scope)
	{
		ParsingScope* parentScope=finder->ParentScope(scope->GetOwnerSymbol());
		if(parentScope)
		{
			scope=parentScope;
		}
		else
		{
			break;
		}
	}

	CopyFrom(
		*allTypes.Obj(),
		From(finder->GetSymbolsRecursively(scope))
			.Where([](Ptr<ParsingScopeSymbol> symbol)
			{
				return symbol.Cast<TypeSymbol>();
			})
		);
	vint last=0;

	while(true)
	{
		vint count=allTypes->Count();
		CopyFrom(
			*allTypes.Obj(),
			From(*allTypes.Obj())
				.Skip(last)
				.SelectMany([=](Ptr<ParsingScopeSymbol> symbol)
				{
					return finder->GetSymbols(symbol->GetScope());
				})
				.Where([](Ptr<ParsingScopeSymbol> symbol)
				{
					return symbol.Cast<TypeSymbol>();
				}),
			true
			);
		if(allTypes->Count()==count)
		{
			break;
		}
		last=count;
	}

	return allTypes;
}

PtrTypeList IntersectTypes(PtrTypeList firstTypes, PtrTypeList secondTypes)
{
	if(!firstTypes)
	{
		return secondTypes;
	}
	else if(!secondTypes)
	{
		return firstTypes;
	}
	else
	{
		PtrTypeList types=new TypeList;
		CopyFrom(*types.Obj(), From(*firstTypes.Obj()).Intersect(*secondTypes.Obj()));
		return types;
	}
}

PtrTypeList SearchGrammarTypes(ParsingTreeObject* obj, ParsingScopeFinder* finder)
{
	if(obj->GetType()==L"SequenceGrammarDef" || obj->GetType()==L"AlternativeGrammarDef")
	{
		PtrTypeList firstTypes=SearchGrammarTypes(finder->Node(obj->GetMember(L"first")).Cast<ParsingTreeObject>().Obj(), finder);
		PtrTypeList secondTypes=SearchGrammarTypes(finder->Node(obj->GetMember(L"second")).Cast<ParsingTreeObject>().Obj(), finder);
		return IntersectTypes(firstTypes, secondTypes);
	}
	else if(
		obj->GetType()==L"LoopGrammarDef"
		|| obj->GetType()==L"OptionalGrammarDef"
		|| obj->GetType()==L"AssignGrammarDef"
		|| obj->GetType()==L"UseGrammarDef"
		|| obj->GetType()==L"SetterGrammarDef")
	{
		return SearchGrammarTypes(finder->Node(obj->GetMember(L"grammar")).Cast<ParsingTreeObject>().Obj(), finder);
	}
	else if(obj->GetType()==L"CreateGrammarDef")
	{
		Ptr<ParsingScopeSymbol> type=FindReferencedSymbols(finder->Node(obj->GetMember(L"type")).Cast<ParsingTreeObject>().Obj(), finder)
			.Where([](Ptr<ParsingScopeSymbol> symbol)
			{
				return symbol.Cast<TypeSymbol>();
			})
			.First(0);
		if(type)
		{
			PtrTypeList types=new List<Ptr<ParsingScopeSymbol>>;
			types->Add(type);
			return types;
		}
	}
	return 0;
}

LazyList<Ptr<ParsingScopeSymbol>> DetermineGrammarTypes(ParsingTreeObject* obj, ParsingScopeFinder* finder)
{
	PtrTypeList selectedTypes;
	ParsingTreeObject* currentObj=obj;
	ParsingTreeObject* lastObj=0;
	while(currentObj)
	{
		if(currentObj->GetType()==L"SequenceGrammarDef")
		{
			ParsingTreeObject* first=dynamic_cast<ParsingTreeObject*>(finder->Node(currentObj->GetMember(L"first").Obj()));
			ParsingTreeObject* second=dynamic_cast<ParsingTreeObject*>(finder->Node(currentObj->GetMember(L"second").Obj()));
			PtrTypeList alternativeTypes=lastObj==first?SearchGrammarTypes(second, finder):SearchGrammarTypes(first, finder);
			selectedTypes=IntersectTypes(selectedTypes, alternativeTypes);
		}
		else if(currentObj->GetType()==L"CreateGrammarDef")
		{
			Ptr<ParsingScopeSymbol> type=FindReferencedSymbols(finder->Node(currentObj->GetMember(L"type")).Cast<ParsingTreeObject>().Obj(), finder)
				.Where([](Ptr<ParsingScopeSymbol> symbol)
				{
					return symbol.Cast<TypeSymbol>();
				})
				.First(0);
			if(type)
			{
				PtrTypeList types=new List<Ptr<ParsingScopeSymbol>>;
				types->Add(type);
				selectedTypes=types;
			}
		}
		else if(currentObj->GetType()==L"AssignGrammarDef" || currentObj->GetType()==L"SetterGrammarDef")
		{
			ParsingTreeObject* grammar=dynamic_cast<ParsingTreeObject*>(finder->Node(currentObj->GetMember(L"grammar").Obj()));
			PtrTypeList alternativeTypes=SearchGrammarTypes(grammar, finder);
			selectedTypes=IntersectTypes(selectedTypes, alternativeTypes);
		}
		lastObj=currentObj;
		currentObj=dynamic_cast<ParsingTreeObject*>(finder->ParentNode(currentObj));
	}

	return selectedTypes?selectedTypes:SearchAllTypes(obj, finder);
}

LazyList<Ptr<ParsingScopeSymbol>> FindPossibleSymbols(ParsingTreeObject* obj, const WString& field, ParsingScopeFinder* finder)
{
	ParsingScope* scope=finder->GetScopeFromNode(obj);
	if(obj->GetType()==L"PrimitiveTypeObj")
	{
		if(field==L"name")
		{
			return finder->GetSymbolsRecursively(scope);
		}
	}
	else if(obj->GetType()==L"SubTypeObj")
	{
		if(field==L"name")
		{
			if(Ptr<ParsingTreeObject> parentType=obj->GetMember(L"parentType").Cast<ParsingTreeObject>())
			{
				WString name=obj->GetMember(L"name").Cast<ParsingTreeToken>()->GetValue();
				LazyList<Ptr<ParsingScopeSymbol>> types=FindReferencedSymbols(parentType.Obj(), finder);
				return types
					.SelectMany([=](Ptr<ParsingScopeSymbol> type)
					{
						return finder->GetSymbols(type->GetScope());
					});
			}
		}
	}
	else if(obj->GetType()==L"PrimitiveGrammarDef")
	{
		if(field==L"name")
		{
			return finder->GetSymbolsRecursively(scope);
		}
	}
	else if(obj->GetType()==L"TextGrammarDef")
	{
		if(field==L"text")
		{
			return From(finder->GetSymbolsRecursively(scope))
				.Where([](Ptr<ParsingScopeSymbol> symbol)
				{
					return symbol.Cast<TokenSymbol>();
				});
		}
	}
	else if(obj->GetType()==L"AssignGrammarDef")
	{
		if(field==L"memberName")
		{
			return DetermineGrammarTypes(obj, finder)
				.SelectMany([=](Ptr<ParsingScopeSymbol> type)
				{
					return finder->GetSymbols(type->GetScope());
				})
				.Where([](Ptr<ParsingScopeSymbol> type)
				{
					return type.Cast<ClassFieldSymbol>();
				});
		}
	}
	else if(obj->GetType()==L"SetterGrammarDef")
	{
		if(field==L"memberName")
		{
			return DetermineGrammarTypes(obj, finder)
				.SelectMany([=](Ptr<ParsingScopeSymbol> type)
				{
					return finder->GetSymbols(type->GetScope());
				})
				.Where([](Ptr<ParsingScopeSymbol> type)
				{
					return type.Cast<ClassFieldSymbol>();
				});
		}
		else if(field==L"value")
		{
			WString memberName=finder->Node(obj->GetMember(L"memberName")).Cast<ParsingTreeToken>()->GetValue();
			Ptr<ParsingScopeSymbol> field=FindPossibleSymbols(obj, L"memberName", finder)
				.Where([=](Ptr<ParsingScopeSymbol> type)
				{
					return type->GetName()==memberName;
				})
				.First(0);
			if(field)
			{
				Ptr<ParsingTreeObject> type=finder->Node(field->GetNode()->GetMember(L"type")).Cast<ParsingTreeObject>();
				return FindReferencedSymbols(type.Obj(), finder)
					.SelectMany([=](Ptr<ParsingScopeSymbol> type)
					{
						return finder->GetSymbols(type->GetScope());
					})
					.Where([](Ptr<ParsingScopeSymbol> type)
					{
						return type.Cast<EnumFieldSymbol>();
					});
			}
		}
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
		return ::FindPossibleSymbols(obj, field, finder);
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
ParserGrammarAutoComplete
***********************************************************************/

class ParserGrammarAutoComplete : public GuiGrammarAutoComplete
{
public:
	ParserGrammarAutoComplete(Ptr<ParserGrammarExecutor> executor)
		:GuiGrammarAutoComplete(executor)
	{
	}
};

/***********************************************************************
TextBoxColorizerWindow
***********************************************************************/

class TextBoxAutoCompleteWindow : public GuiWindow
{
protected:
	GuiMultilineTextBox*					textBoxEditor;

	Ptr<ParserGrammarColorizer>				colorizer;
	Ptr<ParserGrammarAutoComplete>			autoComplete;
	Ptr<ParserGrammarExecutor>				executor;

public:
	TextBoxAutoCompleteWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		SetText(L"Editor.AutoComplete.Grammar");
		SetClientSize(Size(800, 600));

		textBoxEditor=g::NewMultilineTextBox();
		textBoxEditor->SetVerticalAlwaysVisible(false);
		textBoxEditor->SetHorizontalAlwaysVisible(false);
		textBoxEditor->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
		this->GetBoundsComposition()->AddChild(textBoxEditor->GetBoundsComposition());

		{
			executor=new ParserGrammarExecutor;
			colorizer=new ParserGrammarColorizer(executor);
			autoComplete=new ParserGrammarAutoComplete(executor);

			textBoxEditor->SetColorizer(colorizer);
			textBoxEditor->SetAutoComplete(autoComplete);

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

	~TextBoxAutoCompleteWindow()
	{
	}
};

/***********************************************************************
GuiMain
***********************************************************************/

void GuiMain()
{
	TextBoxAutoCompleteWindow window;
	GetApplication()->Run(&window);
}