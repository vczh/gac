/***********************************************************************
Vczh Library++ 3.0
Developer: 陈梓瀚(vczh)
GacUI::Partial Classes

本文件使用Vczh GacUI Resource Code Generator工具自动生成
************************************************************************
DO NOT MODIFY
***********************************************************************/

#ifndef VCZH_GACUI_RESOURCE_CODE_GENERATOR_Demo_PARTIAL_CLASSES
#define VCZH_GACUI_RESOURCE_CODE_GENERATOR_Demo_PARTIAL_CLASSES

#include "..\..\..\Public\Source\GacUIReflection.h"

namespace demos
{
	template<typename TImpl>
	class MainWindow_ : public vl::presentation::controls::GuiWindow, public vl::presentation::GuiInstancePartialClass<vl::presentation::controls::GuiWindow>, public vl::reflection::Description<TImpl>
	{
	private:
	protected:
		vl::presentation::controls::GuiTextList* listSeasons;

		void InitializeComponents()
		{
			if (InitializeFromResource())
			{
				GUI_INSTANCE_REFERENCE(listSeasons);
			}
			else
			{
			}
		}
	public:
		MainWindow_()
			:vl::presentation::GuiInstancePartialClass<vl::presentation::controls::GuiWindow>(L"demos::MainWindow")
			,vl::presentation::controls::GuiWindow(vl::presentation::theme::GetCurrentTheme()->CreateWindowStyle())
			,listSeasons(0)
		{
		}
	};

	class MainWindow;
}

namespace demos
{
	template<typename TImpl>
	class SeasonItemTemplate_ : public vl::presentation::templates::GuiListItemTemplate, public vl::presentation::GuiInstancePartialClass<vl::presentation::templates::GuiListItemTemplate>, public vl::reflection::Description<TImpl>
	{
	private:
		Ptr<presentation::controls::list::TextItem> ViewModel_;
	protected:
		vl::presentation::templates::GuiListItemTemplate* self;

		void InitializeComponents(Ptr<presentation::controls::list::TextItem> ViewModel)
		{
			ViewModel_ = ViewModel;
			if (InitializeFromResource())
			{
				GUI_INSTANCE_REFERENCE(self);
			}
			else
			{
				ViewModel_ = 0;
			}
		}
	public:
		SeasonItemTemplate_()
			:vl::presentation::GuiInstancePartialClass<vl::presentation::templates::GuiListItemTemplate>(L"demos::SeasonItemTemplate")
			,self(0)
		{
		}

		Ptr<presentation::controls::list::TextItem> GetViewModel()
		{
			return ViewModel_;
		}
	};

	class SeasonItemTemplate;
}

namespace vl
{
	namespace reflection
	{
		namespace description
		{
			DECL_TYPE_INFO(demos::MainWindow)
			DECL_TYPE_INFO(demos::SeasonItemTemplate)
		}
	}
}

/*
MainWindow.h :
namespace demos
{
	class MainWindow : public MainWindow_<MainWindow>
	{
		friend class MainWindow_<MainWindow>;
		friend struct vl::reflection::description::CustomTypeDescriptorSelector<MainWindow>;
	protected:

		// #region CLASS_MEMBER_GUIEVENT_HANDLER (DO NOT PUT OTHER CONTENT IN THIS #region.)
		// #endregion CLASS_MEMBER_GUIEVENT_HANDLER
	public:
		MainWindow();
	};
}


MainWindow.cpp :
namespace demos
{
	// #region CLASS_MEMBER_GUIEVENT_HANDLER (DO NOT PUT OTHER CONTENT IN THIS #region.)

	// #endregion CLASS_MEMBER_GUIEVENT_HANDLER

	MainWindow::MainWindow()
	{
		InitializeComponents();
	}
}


SeasonItemTemplate.h :
namespace demos
{
	class SeasonItemTemplate : public SeasonItemTemplate_<SeasonItemTemplate>
	{
		friend class SeasonItemTemplate_<SeasonItemTemplate>;
		friend struct vl::reflection::description::CustomTypeDescriptorSelector<SeasonItemTemplate>;
	protected:

		// #region CLASS_MEMBER_GUIEVENT_HANDLER (DO NOT PUT OTHER CONTENT IN THIS #region.)
		// #endregion CLASS_MEMBER_GUIEVENT_HANDLER
	public:
		SeasonItemTemplate(Ptr<presentation::controls::list::TextItem> ViewModel);
	};
}


SeasonItemTemplate.cpp :
namespace demos
{
	// #region CLASS_MEMBER_GUIEVENT_HANDLER (DO NOT PUT OTHER CONTENT IN THIS #region.)

	// #endregion CLASS_MEMBER_GUIEVENT_HANDLER

	SeasonItemTemplate::SeasonItemTemplate(Ptr<presentation::controls::list::TextItem> ViewModel)
	{
		InitializeComponents(ViewModel);
	}
}


*/

#endif
