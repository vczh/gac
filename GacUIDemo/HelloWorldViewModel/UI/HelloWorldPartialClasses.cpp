/***********************************************************************
Vczh Library++ 3.0
Developer: 陈梓瀚(vczh)
GacUI::Partial Classes

本文件使用Vczh GacUI Resource Code Generator工具自动生成
************************************************************************
DO NOT MODIFY
***********************************************************************/

#include "HelloWorld.h"

namespace vl
{
	namespace reflection
	{
		namespace description
		{
			#define _ ,
			IMPL_TYPE_INFO(helloworld::IViewModel)
			IMPL_TYPE_INFO(helloworld::SignUpWindow)

			BEGIN_CLASS_MEMBER(helloworld::IViewModel)
				CLASS_MEMBER_BASE(vl::reflection::IDescriptable)
				CLASS_MEMBER_PROPERTY_FAST(UserName)
				CLASS_MEMBER_EVENT(UserNameErrorChanged)
				CLASS_MEMBER_PROPERTY_EVENT_READONLY_FAST(UserNameError, UserNameErrorChanged)
				CLASS_MEMBER_PROPERTY_FAST(Password)
				CLASS_MEMBER_EVENT(PasswordErrorChanged)
				CLASS_MEMBER_PROPERTY_EVENT_READONLY_FAST(PasswordError, PasswordErrorChanged)
				CLASS_MEMBER_METHOD(Commit, { L"signIn" });
			END_CLASS_MEMBER(helloworld::IViewModel)

			BEGIN_CLASS_MEMBER(helloworld::SignUpWindow)
				CLASS_MEMBER_BASE(vl::presentation::controls::GuiWindow)
				CLASS_MEMBER_CONSTRUCTOR(helloworld::SignUpWindow*(Ptr<helloworld::IViewModel>), { L"ViewModel" })

				CLASS_MEMBER_PROPERTY_READONLY_FAST(ViewModel)
			END_CLASS_MEMBER(helloworld::SignUpWindow)

			#undef _

			class HelloWorldResourceLoader : public Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					ADD_TYPE_INFO(helloworld::IViewModel)
					ADD_TYPE_INFO(helloworld::SignUpWindow)
				}

				void Unload(ITypeManager* manager)
				{
				}
			};

			class HelloWorldResourcePlugin : public Object, public vl::presentation::controls::IGuiPlugin
			{
			public:
				void Load()override
				{
					GetGlobalTypeManager()->AddTypeLoader(new HelloWorldResourceLoader);
				}

				void AfterLoad()override
				{
				}

				void Unload()override
				{
				}
			};
			GUI_REGISTER_PLUGIN(HelloWorldResourcePlugin)
		}
	}
}

