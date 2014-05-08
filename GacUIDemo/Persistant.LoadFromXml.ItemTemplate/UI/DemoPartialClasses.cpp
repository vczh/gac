/***********************************************************************
Vczh Library++ 3.0
Developer: 陈梓瀚(vczh)
GacUI::Partial Classes

本文件使用Vczh GacUI Resource Code Generator工具自动生成
************************************************************************
DO NOT MODIFY
***********************************************************************/

#include "Demo.h"

namespace vl
{
	namespace reflection
	{
		namespace description
		{
			#define _ ,
			IMPL_TYPE_INFO(demos::MainWindow)
			IMPL_TYPE_INFO(demos::SeasonItemTemplate)

			BEGIN_CLASS_MEMBER(demos::MainWindow)
				CLASS_MEMBER_BASE(vl::presentation::controls::GuiWindow)
				CLASS_MEMBER_CONSTRUCTOR(demos::MainWindow*(), NO_PARAMETER)
			END_CLASS_MEMBER(demos::MainWindow)

			BEGIN_CLASS_MEMBER(demos::SeasonItemTemplate)
				CLASS_MEMBER_BASE(vl::presentation::templates::GuiListItemTemplate)
				CLASS_MEMBER_CONSTRUCTOR(demos::SeasonItemTemplate*(Ptr<presentation::controls::list::TextItem>), { L"ViewModel" })

				CLASS_MEMBER_PROPERTY_READONLY_FAST(ViewModel)
			END_CLASS_MEMBER(demos::SeasonItemTemplate)

			#undef _

			class DemoResourceLoader : public Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					ADD_TYPE_INFO(demos::MainWindow)
					ADD_TYPE_INFO(demos::SeasonItemTemplate)
				}

				void Unload(ITypeManager* manager)
				{
				}
			};

			class DemoResourcePlugin : public Object, public vl::presentation::controls::IGuiPlugin
			{
			public:
				void Load()override
				{
					GetGlobalTypeManager()->AddTypeLoader(new DemoResourceLoader);
				}

				void AfterLoad()override
				{
				}

				void Unload()override
				{
				}
			};
			GUI_REGISTER_PLUGIN(DemoResourcePlugin)
		}
	}
}

