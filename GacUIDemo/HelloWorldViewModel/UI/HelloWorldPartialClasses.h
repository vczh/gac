/***********************************************************************
Vczh Library++ 3.0
Developer: 陈梓瀚(vczh)
GacUI::Partial Classes

本文件使用Vczh GacUI Resource Code Generator工具自动生成
************************************************************************
DO NOT MODIFY
***********************************************************************/

#ifndef VCZH_GACUI_RESOURCE_CODE_GENERATOR_HelloWorld_PARTIAL_CLASSES
#define VCZH_GACUI_RESOURCE_CODE_GENERATOR_HelloWorld_PARTIAL_CLASSES

#include "..\..\..\Public\Source\GacUIReflection.h"

namespace helloworld
{
	class IViewModel : public virtual vl::reflection::IDescriptable, public vl::reflection::Description<IViewModel>
	{
	public:

		virtual WString GetUserName() = 0;
		virtual void SetUserName(WString value) = 0;

		virtual WString GetUserNameError() = 0;
		vl::Event<void()> UserNameErrorChanged;

		virtual WString GetPassword() = 0;
		virtual void SetPassword(WString value) = 0;

		virtual WString GetPasswordError() = 0;
		vl::Event<void()> PasswordErrorChanged;

		virtual bool Commit(bool signIn) = 0;
	};
}

namespace helloworld
{
	template<typename TImpl>
	class SignUpWindow_ : public vl::presentation::controls::GuiWindow, public vl::presentation::GuiInstancePartialClass<vl::presentation::controls::GuiWindow>, public vl::reflection::Description<TImpl>
	{
	private:
		Ptr<helloworld::IViewModel> ViewModel_;
	protected:
		vl::presentation::controls::GuiButton* buttonSignUp;
		vl::presentation::controls::GuiSinglelineTextBox* textBoxPassword;
		vl::presentation::controls::GuiSinglelineTextBox* textBoxUserName;

		void InitializeComponents(Ptr<helloworld::IViewModel> ViewModel)
		{
			ViewModel_ = ViewModel;
			if (InitializeFromResource())
			{
				GUI_INSTANCE_REFERENCE(buttonSignUp);
				GUI_INSTANCE_REFERENCE(textBoxPassword);
				GUI_INSTANCE_REFERENCE(textBoxUserName);
			}
			else
			{
				ViewModel_ = 0;
			}
		}
	public:
		SignUpWindow_()
			:vl::presentation::controls::GuiWindow(vl::presentation::theme::GetCurrentTheme()->CreateWindowStyle())
			,vl::presentation::GuiInstancePartialClass<vl::presentation::controls::GuiWindow>(L"helloworld::SignUpWindow")
			,buttonSignUp(0)
			,textBoxPassword(0)
			,textBoxUserName(0)
		{
		}

		Ptr<helloworld::IViewModel> GetViewModel()
		{
			return ViewModel_;
		}
	};

	class SignUpWindow;
}

namespace vl
{
	namespace reflection
	{
		namespace description
		{
			DECL_TYPE_INFO(helloworld::IViewModel)
			DECL_TYPE_INFO(helloworld::SignUpWindow)
		}
	}
}

/*
SignUpWindow.h :
namespace helloworld
{
	class SignUpWindow : public SignUpWindow_<SignUpWindow>
	{
		friend class SignUpWindow_<SignUpWindow>;
		friend struct vl::reflection::description::CustomTypeDescriptorSelector<SignUpWindow>;
	protected:

		// #region CLASS_MEMBER_GUIEVENT_HANDLER (DO NOT PUT OTHER CONTENT IN THIS #region.)
		// #endregion CLASS_MEMBER_GUIEVENT_HANDLER
	public:
		SignUpWindow(Ptr<helloworld::IViewModel> ViewModel);
	};
}


SignUpWindow.cpp :
namespace helloworld
{
	// #region CLASS_MEMBER_GUIEVENT_HANDLER (DO NOT PUT OTHER CONTENT IN THIS #region.)

	// #endregion CLASS_MEMBER_GUIEVENT_HANDLER

	SignUpWindow::SignUpWindow(Ptr<helloworld::IViewModel> ViewModel)
	{
		InitializeComponents(ViewModel);
	}
}


*/

#endif
