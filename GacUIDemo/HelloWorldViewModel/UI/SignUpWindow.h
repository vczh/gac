/***********************************************************************
Vczh Library++ 3.0
Developer: 陈梓瀚(vczh)
GacUI::SignUpWindow

本文件使用Vczh GacUI Resource Code Generator工具自动生成
***********************************************************************/

#ifndef VCZH_GACUI_RESOURCE_CODE_GENERATOR_HelloWorld_SignUpWindow
#define VCZH_GACUI_RESOURCE_CODE_GENERATOR_HelloWorld_SignUpWindow

#include "HelloWorldPartialClasses.h"

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

#endif
