#include "UI\HelloWorld.h"
#include <Windows.h>

using namespace vl::collections;
using namespace vl::regex;
using namespace vl::reflection::description;
using namespace helloworld;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

#undef GetUserName

class ViewModel : public Object, public virtual IViewModel
{
private:
	WString userName;
	WString password;

	Regex regexLcLetters;
	Regex regexUcLetters;
	Regex regexNumbers;

public:
	ViewModel()
		:regexLcLetters(L"[a-z]")
		, regexUcLetters(L"[A-Z]")
		, regexNumbers(L"[0-9]")
	{
	}

	WString GetUserName()override
	{
		return userName;
	}

	void SetUserName(WString value)override
	{
		userName = value;
		UserNameErrorChanged();
	}

	WString GetUserNameError()override
	{
		if (userName == L"")
		{
			return L"User name should not be empty.";
		}
		return L"";
	}

	WString GetPassword()override
	{
		return password;
	}

	void SetPassword(WString value)override
	{
		password = value;
		PasswordErrorChanged();
	}

	WString GetPasswordError()override
	{
		if (password == L"")
		{
			return L"Password should not be empty.";
		}

		bool containsLowerCases = regexLcLetters.Match(password);
		bool containsUpperCases = regexUcLetters.Match(password);
		bool containsNumbers = regexNumbers.Match(password);
		if (!containsLowerCases || !containsUpperCases || !containsNumbers)
		{
			return L"Password should contains at least one lower case letter, one upper case letter and one digit.";
		}

		return L"";
	}

	bool Commit(bool signIn)override
	{
		return !signIn;
	}
};

void GuiMain()
{
	List<WString> errors;
	GetInstanceLoaderManager()->SetResource(L"Resource", GuiResource::LoadFromXml(L"..\\Resources\\HelloWorldViewModel.xml", errors));
	SignUpWindow window(new ViewModel);
	window.ForceCalculateSizeImmediately();
	window.MoveToScreenCenter();
	GetApplication()->Run(&window);
}