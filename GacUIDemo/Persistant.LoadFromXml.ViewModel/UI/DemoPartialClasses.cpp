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
			IMPL_TYPE_INFO(demos::IControlViewModel)
			IMPL_TYPE_INFO(demos::IWindowViewModel)
			IMPL_TYPE_INFO(demos::CalculatorControl)
			IMPL_TYPE_INFO(demos::MainWindow)

			BEGIN_CLASS_MEMBER(demos::IControlViewModel)
				CLASS_MEMBER_BASE(vl::reflection::IDescriptable)
				CLASS_MEMBER_PROPERTY_FAST(First)
				CLASS_MEMBER_PROPERTY_FAST(Second)
				CLASS_MEMBER_EVENT(SumChanged)
				CLASS_MEMBER_PROPERTY_EVENT_READONLY_FAST(Sum, SumChanged)
				CLASS_MEMBER_EVENT(ProductChanged)
				CLASS_MEMBER_PROPERTY_EVENT_READONLY_FAST(Product, ProductChanged)
				CLASS_MEMBER_METHOD(UpdateProduct, NO_PARAMETER);
			END_CLASS_MEMBER(demos::IControlViewModel)

			BEGIN_CLASS_MEMBER(demos::IWindowViewModel)
				CLASS_MEMBER_BASE(vl::reflection::IDescriptable)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(ControlViewModel)
			END_CLASS_MEMBER(demos::IWindowViewModel)

			BEGIN_CLASS_MEMBER(demos::CalculatorControl)
				CLASS_MEMBER_BASE(vl::presentation::controls::GuiCustomControl)
				CLASS_MEMBER_CONSTRUCTOR(demos::CalculatorControl*(Ptr<demos::IControlViewModel>), { L"ViewModel" })

				CLASS_MEMBER_PROPERTY_READONLY_FAST(ViewModel)
			END_CLASS_MEMBER(demos::CalculatorControl)

			BEGIN_CLASS_MEMBER(demos::MainWindow)
				CLASS_MEMBER_BASE(vl::presentation::controls::GuiWindow)
				CLASS_MEMBER_CONSTRUCTOR(demos::MainWindow*(Ptr<demos::IWindowViewModel>), { L"ViewModel" })

				CLASS_MEMBER_PROPERTY_READONLY_FAST(ViewModel)
			END_CLASS_MEMBER(demos::MainWindow)

			#undef _

			class DemoResourceLoader : public Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					ADD_TYPE_INFO(demos::IControlViewModel)
					ADD_TYPE_INFO(demos::IWindowViewModel)
					ADD_TYPE_INFO(demos::CalculatorControl)
					ADD_TYPE_INFO(demos::MainWindow)
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

