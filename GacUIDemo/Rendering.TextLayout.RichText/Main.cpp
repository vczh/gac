#include "..\..\Public\Source\GacUI.h"
#include <Windows.h>

using namespace vl::stream;
using namespace vl::regex;
using namespace vl::collections;
using namespace vl::parsing::xml;
using namespace vl::presentation::elements::text;

namespace document
{
	class GifAnimation : public Object, public IGuiGraphicsAnimation
	{
	protected:
		unsigned __int64				startTime;
		Ptr<DocumentImageRun>			imageRun;
		int								paragraphIndex;
		GuiDocumentViewer*				documentViewer;
	public:
		GifAnimation(Ptr<DocumentImageRun> _imageRun, int _paragraphIndex, GuiDocumentViewer* _documentViewer)
			:imageRun(_imageRun)
			,paragraphIndex(_paragraphIndex)
			,documentViewer(_documentViewer)
			,startTime(DateTime::LocalTime().totalMilliseconds)
		{
		}

		int GetTotalLength()
		{
			return 1;
		}

		int GetCurrentPosition()
		{
			return 0;
		}

		void Play(int currentPosition, int totalLength)
		{
			unsigned __int64 ms=DateTime::LocalTime().totalMilliseconds-startTime;
			int frameIndex=(ms/100)%imageRun->image->GetFrameCount();
			imageRun->frameIndex=frameIndex;
			documentViewer->NotifyParagraphUpdated(paragraphIndex);
		}

		void Stop()
		{
		}
	};
}
using namespace document;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

class GuiRichTextWindow : public GuiWindow
{
protected:
	GuiDocumentViewer*				documentViewer;
public:
	GuiRichTextWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		this->SetText(L"Rendering.TextLayout.RichText");
		this->SetClientSize(Size(640, 480));
		this->MoveToScreenCenter();
		this->GetBoundsComposition()->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);
	
		documentViewer=g::NewDocumentViewer();
		documentViewer->GetBoundsComposition()->SetPreferredMinSize(Size(300, 100));
		documentViewer->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
		documentViewer->GetBoundsComposition()->SetAssociatedCursor(GetCurrentController()->ResourceService()->GetSystemCursor(INativeCursor::LargeWaiting));
	
		GetApplication()->InvokeAsync([=]()
		{
			Ptr<DocumentModel> document;
			{
				WString text;
				{
					FileStream fileStream(L"..\\Resources\\document2.xml", FileStream::ReadOnly);
					BomDecoder decoder;
					DecoderStream decoderStream(fileStream, decoder);
					StreamReader writer(decoderStream);
					text=writer.ReadToEnd();
				}
				auto table=XmlLoadTable();
				Ptr<XmlDocument> xml=XmlParseDocument(text, table);
				document=DocumentModel::LoadFromXml(xml, L"..\\Resources\\");
			}
			GetApplication()->InvokeInMainThreadAndWait([=]()
			{
				documentViewer->SetDocument(document);
				documentViewer->GetBoundsComposition()->SetAssociatedCursor(GetCurrentController()->ResourceService()->GetDefaultSystemCursor());

				FOREACH_INDEXER(Ptr<DocumentParagraph>, p, i, document->paragraphs)
				FOREACH(Ptr<DocumentLine>, l, p->lines)
				FOREACH(Ptr<DocumentRun>, r, l->runs)
				{
					Ptr<DocumentImageRun> image=r.Cast<DocumentImageRun>();
					if(image && image->image->GetFrameCount()>1)
					{
						Ptr<GifAnimation> gifAnimation=new GifAnimation(image, i, documentViewer);
						this->GetGraphicsHost()->GetAnimationManager()->AddAnimation(gifAnimation);
					}
				}
			});
		});

		this->GetContainerComposition()->AddChild(documentViewer->GetBoundsComposition());
	}
};

void GuiMain()
{
	GuiRichTextWindow window;
	GetApplication()->Run(&window);
}