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
		vint							paragraphIndex;
		GuiDocumentViewer*				documentViewer;
	public:
		GifAnimation(Ptr<DocumentImageRun> _imageRun, vint _paragraphIndex, GuiDocumentViewer* _documentViewer)
			:imageRun(_imageRun)
			,paragraphIndex(_paragraphIndex)
			,documentViewer(_documentViewer)
			,startTime(DateTime::LocalTime().totalMilliseconds)
		{
		}

		vint GetTotalLength()
		{
			return 1;
		}

		vint GetCurrentPosition()
		{
			return 0;
		}

		void Play(vint currentPosition, vint totalLength)
		{
			unsigned __int64 ms=DateTime::LocalTime().totalMilliseconds-startTime;
			vint frameIndex=(ms/100)%imageRun->image->GetFrameCount();
			imageRun->frameIndex=frameIndex;
			documentViewer->NotifyParagraphUpdated(paragraphIndex, 1, 1, false);
		}

		void Stop()
		{
		}
	};
	
	class GifAnimationVisitor : public Object, public DocumentRun::IVisitor
	{
	public:
		GuiControlHost*					controlHost;
		vint							paragraphIndex;
		GuiDocumentViewer*				documentViewer;

		GifAnimationVisitor(GuiControlHost* _controlHost, vint _paragraphIndex, GuiDocumentViewer* _documentViewer)
			:controlHost(_controlHost)
			,paragraphIndex(_paragraphIndex)
			,documentViewer(_documentViewer)
		{
		}

		void VisitContainer(DocumentContainerRun* run)
		{
			FOREACH(Ptr<DocumentRun>, subRun, run->runs)
			{
				subRun->Accept(this);
			}
		}

		void Visit(DocumentTextRun* run)override
		{
		}

		void Visit(DocumentStylePropertiesRun* run)override
		{
			VisitContainer(run);
		}

		void Visit(DocumentStyleApplicationRun* run)override
		{
			VisitContainer(run);
		}

		void Visit(DocumentHyperlinkRun* run)override
		{
			VisitContainer(run);
		}

		void Visit(DocumentImageRun* run)override
		{
			if(run->image->GetFrameCount()>1)
			{
				Ptr<GifAnimation> gifAnimation=new GifAnimation(run, paragraphIndex, documentViewer);
				controlHost->GetGraphicsHost()->GetAnimationManager()->AddAnimation(gifAnimation);
			}
		}

		void Visit(DocumentParagraphRun* run)override
		{
			VisitContainer(run);
		}

		static void CreateGifAnimation(DocumentParagraphRun* run, GuiControlHost* controlHost, vint paragraphIndex, GuiDocumentViewer* documentViewer)
		{
			GifAnimationVisitor visitor(controlHost, paragraphIndex, documentViewer);
			run->Accept(&visitor);
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
				List<WString> errors;
				document = DocumentModel::LoadFromXml(xml, L"..\\Resources\\", errors);
			}
			GetApplication()->InvokeInMainThreadAndWait([=]()
			{
				documentViewer->SetDocument(document);
				documentViewer->GetBoundsComposition()->SetAssociatedCursor(GetCurrentController()->ResourceService()->GetDefaultSystemCursor());

				FOREACH_INDEXER(Ptr<DocumentParagraphRun>, p, i, document->paragraphs)
				{
					GifAnimationVisitor::CreateGifAnimation(p.Obj(), this, i, documentViewer);
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