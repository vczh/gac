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
		textBoxEditor->SetColorizer(new ParserGrammarColorizer);
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