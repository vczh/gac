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
ParserParsingAnalyzer
***********************************************************************/

class ParserParsingAnalyzer : public RepeatingParsingExecutor::IParsingAnalyzer
{
protected:
	RepeatingParsingExecutor*		executor;
	vint							_type = -1, _token = -1, _rule = -1, _field = -1, _enumValue = -1, _literal = -1;

	class Cache : public Object
	{
	public:
		Group<WString, WString>						typeNames;
		SortedList<WString>							tokenNames;
		SortedList<WString>							literalNames;
		SortedList<WString>							ruleNames;
		Group<WString, WString>						baseTypes;
		Group<WString, WString>						inheritedTypes;
		Group<WString, WString>						enumItems;
		Group<WString, Tuple<WString, WString>>		classFields;
	};
public:
	void Attach(RepeatingParsingExecutor* _executor)
	{
		executor = _executor;
		_type = executor->GetSemanticId(L"Type");
		_token = executor->GetSemanticId(L"Token");
		_rule = executor->GetSemanticId(L"Rule");
		_field = executor->GetSemanticId(L"Field");
		_enumValue = executor->GetSemanticId(L"EnumValue");
		_literal = executor->GetSemanticId(L"Literal");
	}

	void Detach(RepeatingParsingExecutor* _executor)
	{
		executor = nullptr;
	}

	/***********************************************************************
	CreateCache
	***********************************************************************/

	void CreateCache_SearchSubTypes(Ptr<Cache> cache, const WString prefix, Ptr<ParsingTreeObject> typeDef)
	{
		if (auto name = typeDef->GetMember(L"name").Cast<ParsingTreeToken>())
		{
			cache->typeNames.Add(prefix, name->GetValue());
			if (typeDef->GetType() == L"ClassTypeDef")
			{
				if (auto members = typeDef->GetMember(L"subTypes").Cast<ParsingTreeArray>())
				{
					FOREACH(Ptr<ParsingTreeNode>, subNode, members->GetItems())
					{
						if (auto subType = subNode.Cast<ParsingTreeObject>())
						{
							CreateCache_SearchSubTypes(cache, prefix + name->GetValue() + L".", subType);
						}
					}
				}
			}
		}
	}

	void CreateCache_CreateSymbols(Ptr<Cache> cache, Ptr<ParsingTreeObject> def)
	{
		if (def->GetType() == L"TokenDef")
		{
			if (auto name = def->GetMember(L"name").Cast<ParsingTreeToken>())
			{
				cache->tokenNames.Add(name->GetValue());
			}

			if (auto discard = def->GetMember(L"discard").Cast<ParsingTreeToken>())
			{
				if (discard->GetValue() == L"DiscardToken")
				{
					return;
				}
			}
			if (auto regex = def->GetMember(L"regex").Cast<ParsingTreeToken>())
			{
				auto escaped = DeserializeString(regex->GetValue());
				if (IsRegexEscapedLiteralString(escaped))
				{
					cache->literalNames.Add(SerializeString(UnescapeTextForRegex(escaped)));
				}
			}
		}
		else if (def->GetType() == L"RuleDef")
		{
			if (auto name = def->GetMember(L"name").Cast<ParsingTreeToken>())
			{
				cache->ruleNames.Add(name->GetValue());
			}
		}
		else
		{
			CreateCache_SearchSubTypes(cache, L"", def);
		}
	}

	void CreateCache_ResolveSymbols(Ptr<Cache> cache, const WString& prefix, Ptr<ParsingTreeObject> typeDef)
	{
		if (auto name = typeDef->GetMember(L"name").Cast<ParsingTreeToken>())
		{
			auto typePrefix = prefix + name->GetValue() + L".";
			if (typeDef->GetType() == L"EnumTypeDef")
			{
				if (auto members = typeDef->GetMember(L"members").Cast<ParsingTreeArray>())
				{
					FOREACH(Ptr<ParsingTreeNode>, subNode, members->GetItems())
					{
						if (auto member = subNode.Cast<ParsingTreeObject>())
						{
							if (auto nameToken = member->GetMember(L"name").Cast<ParsingTreeToken>())
							{
								cache->enumItems.Add(typePrefix, nameToken->GetValue());
							}
						}
					}
				}
			}
			else if (typeDef->GetType() == L"ClassTypeDef")
			{
				if (auto parentType = typeDef->GetMember(L"parentType").Cast<ParsingTreeObject>())
				{
					auto resolvedType = ResolveType(parentType.Obj(), cache, nullptr);
					if (resolvedType != L"")
					{
						cache->baseTypes.Add(prefix + name->GetValue(), resolvedType);
						cache->inheritedTypes.Add(resolvedType, prefix + name->GetValue());
					}
				}

				if (auto members = typeDef->GetMember(L"members").Cast<ParsingTreeArray>())
				{
					FOREACH(Ptr<ParsingTreeNode>, subNode, members->GetItems())
					{
						if (auto field = subNode.Cast<ParsingTreeObject>())
						{
							auto typeNode = field->GetMember(L"type").Cast<ParsingTreeObject>();
							auto nameToken = field->GetMember(L"name").Cast<ParsingTreeToken>();
							if (typeNode && nameToken)
							{
								auto resolvedType = ResolveType(typeNode.Obj(), cache, nullptr);
								cache->classFields.Add(typePrefix, Tuple<WString, WString>(nameToken->GetValue(), resolvedType));
							}
						}
					}
				}

				if (auto members = typeDef->GetMember(L"subTypes").Cast<ParsingTreeArray>())
				{
					FOREACH(Ptr<ParsingTreeNode>, subNode, members->GetItems())
					{
						if (auto subType = subNode.Cast<ParsingTreeObject>())
						{
							CreateCache_ResolveSymbols(cache, typePrefix, subType);
						}
					}
				}
			}
		}
	}

	Ptr<Object> CreateCacheAsync(const RepeatingParsingOutput& output)
	{
		auto cache = MakePtr<Cache>();
		if (auto definitions = output.node->GetMember(L"definitions").Cast<ParsingTreeArray>())
		{
			FOREACH(Ptr<ParsingTreeNode>, subNode, definitions->GetItems())
			{
				if (auto obj = subNode.Cast<ParsingTreeObject>())
				{
					CreateCache_CreateSymbols(cache, obj);
				}
			}
				
			FOREACH(Ptr<ParsingTreeNode>, subNode, definitions->GetItems())
			{
				if (auto obj = subNode.Cast<ParsingTreeObject>())
				{
					CreateCache_ResolveSymbols(cache, L"", obj);
				}
			}
		}
		return cache;
	}

	/***********************************************************************
	GetSemanticIdForToken
	***********************************************************************/

	void GetTypeScopes(ParsingTreeObject* typeObj, List<WString>& typeScopes, const RepeatingPartialParsingOutput* output)
	{
		typeScopes.Add(L"");
		auto typeScope = GetParent(typeObj, output);
		while (typeScope)
		{
			if (auto typeScopeObj = dynamic_cast<ParsingTreeObject*>(typeScope))
			{
				if (typeScopeObj->GetType() == L"ClassTypeDef")
				{
					if (auto classNameToken = GetMember(typeScopeObj, L"name", output).Cast<ParsingTreeToken>())
					{
						auto className = classNameToken->GetValue();
						for (vint i = 0; i < typeScopes.Count(); i++)
						{
							typeScopes[i] = className + L"." + typeScopes[i];
						}
						typeScopes.Add(L"");
					}
				}
			}
			typeScope = GetParent(typeScope, output);
		}
	}

	WString ResolveType(ParsingTreeObject* typeObj, Ptr<Cache> cache, const RepeatingPartialParsingOutput* output)
	{
		if (auto nameToken = GetMember(typeObj, L"name", output).Cast<ParsingTreeToken>())
		{
			auto name = nameToken->GetValue();
			if (typeObj->GetType() == L"PrimitiveTypeObj")
			{
				List<WString> typeScopes;
				GetTypeScopes(typeObj, typeScopes, output);

				FOREACH(WString, typeScope, typeScopes)
				{
					if (cache->typeNames.Contains(typeScope, name))
					{
						return typeScope + name;
					}
				}
			}
			else if (typeObj->GetType() == L"SubTypeObj")
			{
				if (auto parentTypeObj = GetMember(typeObj, L"parentType", output).Cast<ParsingTreeObject>())
				{
					auto resolvedType = ResolveType(parentTypeObj.Obj(), cache, output);
					if (cache->typeNames.Contains(resolvedType + L".", name))
					{
						return resolvedType + L"." + name;
					}
				}
			}
		}
		return L"";
	}

	vint GetSemanticIdForTokenAsync(const ParsingTokenContext& tokenContext, const RepeatingParsingOutput& output)
	{
		auto cache = output.cache.Cast<Cache>();
		if (tokenContext.tokenParent->GetType() == L"PrimitiveTypeObj" || tokenContext.tokenParent->GetType() == L"SubTypeObj")
		{
			if (tokenContext.field == L"name") // Type
			{
				if (ResolveType(tokenContext.tokenParent, cache, nullptr) != L"")
				{
					return _type;
				}
			}
		}
		else if (tokenContext.tokenParent->GetType() == L"PrimitiveGrammarDef")
		{
			if (tokenContext.field == L"name") // Token, Rule
			{
				auto value = tokenContext.foundToken->GetValue();
				if (cache->tokenNames.Contains(value))
				{
					return _token;
				}
				else if (cache->ruleNames.Contains(value))
				{
					return _rule;
				}
			}
		}
		return -1;
	}

	void GetCandidateItemsAsync(const ParsingTokenContext& tokenContext, const RepeatingPartialParsingOutput& partialOutput, collections::List<ParsingCandidateItem>& candidateItems)
	{
	}
};

/***********************************************************************
ParserGrammarExecutor
***********************************************************************/

class ParserGrammarExecutor : public RepeatingParsingExecutor
{
public:
	ParserGrammarExecutor()
		:RepeatingParsingExecutor(CreateBootstrapAutoRecoverParser(), L"ParserDecl", new ParserParsingAnalyzer)
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