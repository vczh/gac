/***********************************************************************
Vczh Library++ 3.0
Developer: 陈梓瀚(vczh)
GacUI::SeasonItemTemplate

本文件使用Vczh GacUI Resource Code Generator工具自动生成
***********************************************************************/

#ifndef VCZH_GACUI_RESOURCE_CODE_GENERATOR_Demo_SeasonItemTemplate
#define VCZH_GACUI_RESOURCE_CODE_GENERATOR_Demo_SeasonItemTemplate

#include "DemoPartialClasses.h"

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

#endif
