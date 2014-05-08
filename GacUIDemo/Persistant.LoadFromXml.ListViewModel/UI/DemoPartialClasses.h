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
	class ISeason : public virtual vl::reflection::IDescriptable, public vl::reflection::Description<ISeason>
	{
	public:

		virtual WString GetSeason() = 0;

		virtual WString GetDescription() = 0;

		virtual Ptr<description::IValueObservableList> GetChildren() = 0;
	};
}

namespace demos
{
	class IViewModel : public virtual vl::reflection::IDescriptable, public vl::reflection::Description<IViewModel>
	{
	public:

		virtual Ptr<description::IValueObservableList> GetSeasons() = 0;

		virtual Ptr<description::IValueObservableList> GetComplexSeasons() = 0;

		virtual Ptr<demos::ISeason> GetTreeSeasons() = 0;

		virtual void AddSeason() = 0;
		virtual void AddComplexSeason() = 0;
		virtual void AddTreeSeason() = 0;
	};
}

namespace demos
{
	template<typename TImpl>
	class MainWindow_ : public vl::presentation::controls::GuiWindow, public vl::presentation::GuiInstancePartialClass<vl::presentation::controls::GuiWindow>, public vl::reflection::Description<TImpl>
	{
	private:
		Ptr<demos::IViewModel> ViewModel_;
	protected:
		vl::presentation::controls::GuiBindableTextList* listSeasons3;
		vl::presentation::controls::GuiBindableListView* listViewSeasons3;
		vl::presentation::controls::GuiBindableTreeView* treeViewSeasons3;

		void InitializeComponents(Ptr<demos::IViewModel> ViewModel)
		{
			ViewModel_ = ViewModel;
			if (InitializeFromResource())
			{
				GUI_INSTANCE_REFERENCE(listSeasons3);
				GUI_INSTANCE_REFERENCE(listViewSeasons3);
				GUI_INSTANCE_REFERENCE(treeViewSeasons3);
			}
			else
			{
				ViewModel_ = 0;
			}
		}
	public:
		MainWindow_()
			:vl::presentation::controls::GuiWindow(vl::presentation::theme::GetCurrentTheme()->CreateWindowStyle())
			,vl::presentation::GuiInstancePartialClass<vl::presentation::controls::GuiWindow>(L"demos::MainWindow")
			,listSeasons3(0)
			,listViewSeasons3(0)
			,treeViewSeasons3(0)
		{
		}

		Ptr<demos::IViewModel> GetViewModel()
		{
			return ViewModel_;
		}
	};

	class MainWindow;
}

namespace vl
{
	namespace reflection
	{
		namespace description
		{
			DECL_TYPE_INFO(demos::ISeason)
			DECL_TYPE_INFO(demos::IViewModel)
			DECL_TYPE_INFO(demos::MainWindow)
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
		MainWindow(Ptr<demos::IViewModel> ViewModel);
	};
}


MainWindow.cpp :
namespace demos
{
	// #region CLASS_MEMBER_GUIEVENT_HANDLER (DO NOT PUT OTHER CONTENT IN THIS #region.)

	// #endregion CLASS_MEMBER_GUIEVENT_HANDLER

	MainWindow::MainWindow(Ptr<demos::IViewModel> ViewModel)
	{
		InitializeComponents(ViewModel);
	}
}


*/

#endif
