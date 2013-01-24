#include "..\..\Public\Source\GacUI.h"

using namespace vl::stream;
using namespace vl::regex;
using namespace vl::collections;

namespace document
{
	int ConvertHex(wchar_t c)
	{
		if(L'a'<=c && c<=L'f') return c-L'a'+10;
		if(L'A'<=c && c<=L'F') return c-L'A'+10;
		if(L'0'<=c && c<=L'9') return c-L'0';
		return 0;
	}

	Color ConvertColor(const WString& colorString)
	{
		return Color(
			ConvertHex(colorString[1])*16+ConvertHex(colorString[2]),
			ConvertHex(colorString[3])*16+ConvertHex(colorString[4]),
			ConvertHex(colorString[5])*16+ConvertHex(colorString[6])
			);
	}

	struct GifRun
	{
		Ptr<text::DocumentImageRun>		imageRun;
		int								paragraphIndex;
	};
	
	Ptr<text::DocumentModel> BuildDocumentModel(const WString& fileName, List<Ptr<GifRun>>& animations)
	{
		HDC dc=CreateCompatibleDC(NULL);
		int dpi=GetDeviceCaps(dc, LOGPIXELSY);
		DeleteDC(dc);
		Ptr<text::DocumentModel> document=new text::DocumentModel;

		WString rawDocument;
		{
			FileStream fileStream(fileName, FileStream::ReadOnly);
			BomDecoder decoder;
			DecoderStream decoderStream(fileStream, decoder);
			StreamReader reader(decoderStream);
			rawDocument=reader.ReadToEnd();
		}

		WString regexTag_s=L"<(<tag>s)>(<font>[^:]+):(<bold>[^:]+):(<color>[^:]+):(<size>[^:]+):(<text>/.*?)<//s>";
		WString regexTag_i=L"<(<tag>i)>(<cx>[^:]+),(<cy>[^:]+):(<b>[^:]+):(<file>/.*?)<//i>";
		WString regexTag_p=L"<(<tag>p)//>";
		Regex regexTag(regexTag_s+L"|"+regexTag_i+L"|"+regexTag_p);
		Regex regexLine(L"\r\n");
		RegexMatch::List matches;
		regexTag.Search(rawDocument, matches);

		Ptr<text::DocumentParagraph> paragraph=0;
		Ptr<text::DocumentLine> line=0;
		
		for(int i=0;i<matches.Count();i++)
		{
			Ptr<RegexMatch> match=matches[i];
			if(match->Groups()[L"tag"].Get(0).Value()==L"p")
			{
				paragraph=0;
				line=0;
			}
			else if(match->Groups()[L"tag"].Get(0).Value()==L"i")
			{
				int cx=wtoi(match->Groups()[L"cx"].Get(0).Value());
				int cy=wtoi(match->Groups()[L"cy"].Get(0).Value());
				int b=wtoi(match->Groups()[L"b"].Get(0).Value());
				WString file=match->Groups()[L"file"].Get(0).Value();

				if(!paragraph)
				{
					paragraph=new text::DocumentParagraph;
					document->paragraphs.Add(paragraph);
					line=0;
				}
				if(!line)
				{
					line=new text::DocumentLine;
					paragraph->lines.Add(line);
				}

				Ptr<text::DocumentImageRun> run=new text::DocumentImageRun;
				run->size=Size(cx, cy);
				run->baseline=b;
				run->image=GetCurrentController()->ImageService()->CreateImageFromFile(L"..\\Resources\\"+file);
				run->frameIndex=0;
				line->runs.Add(run);

				if(run->image->GetFrameCount()>1)
				{
					Ptr<GifRun> gifRun=new GifRun;
					gifRun->imageRun=run;
					gifRun->paragraphIndex=document->paragraphs.Count()-1;
					animations.Add(gifRun);
				}
			}
			else if(match->Groups()[L"tag"].Get(0).Value()==L"s")
			{
				FontProperties fontStyle;
				Color fontColor;
				RegexMatch::List lines;
				{
					WString font=match->Groups()[L"font"].Get(0).Value();
					WString bold=match->Groups()[L"bold"].Get(0).Value();
					WString color=match->Groups()[L"color"].Get(0).Value();
					WString size=match->Groups()[L"size"].Get(0).Value();
					WString text=match->Groups()[L"text"].Get(0).Value();

					fontStyle.fontFamily=font;
					fontStyle.bold=bold==L"true";
					fontStyle.size=(int)(wtof(size)*dpi/72);
					fontColor=ConvertColor(color);
					regexLine.Split(text, true, lines);
				}

				for(int j=0;j<lines.Count();j++)
				{
					WString lineText=lines[j]->Result().Value();
					if(!paragraph)
					{
						paragraph=new text::DocumentParagraph;
						document->paragraphs.Add(paragraph);
						line=0;
					}
					if(!line || j>0)
					{
						line=new text::DocumentLine;
						paragraph->lines.Add(line);
					}

					Ptr<text::DocumentTextRun> run=new text::DocumentTextRun;
					run->style=fontStyle;
					run->color=fontColor;
					run->text=lineText;
					line->runs.Add(run);
				}
			}
		}

		return document;
	}

	class GifAnimation : public Object, public IGuiGraphicsAnimation
	{
	protected:
		unsigned __int64				startTime;
		Ptr<text::DocumentImageRun>		imageRun;
		int								paragraphIndex;
		GuiDocumentElement*				documentElement;
	public:
		GifAnimation(Ptr<text::DocumentImageRun> _imageRun, int _paragraphIndex, GuiDocumentElement* _documentElement)
			:imageRun(_imageRun)
			,paragraphIndex(_paragraphIndex)
			,documentElement(_documentElement)
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
			documentElement->NotifyParagraphUpdated(paragraphIndex);
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
	GuiScrollContainer*				scriptDocumentView;
public:
	GuiRichTextWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		this->SetText(L"Rendering.TextLayout.RichText");
		this->SetClientSize(Size(640, 480));
		this->MoveToScreenCenter();
		this->GetBoundsComposition()->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);
	
		GuiScrollContainer* scriptDocumentView=new GuiScrollContainer(GetCurrentTheme()->CreateMultilineTextBoxStyle());
		scriptDocumentView->SetExtendToFullWidth(true);
		scriptDocumentView->SetHorizontalAlwaysVisible(false);
		scriptDocumentView->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
		scriptDocumentView->GetBoundsComposition()->SetAssociatedCursor(GetCurrentController()->ResourceService()->GetSystemCursor(INativeCursor::LargeWaiting));
	
		GetApplication()->InvokeAsync([=]()
		{
			List<Ptr<GifRun>> animations;
			Ptr<text::DocumentModel> document=BuildDocumentModel(L"..\\Resources\\document2.txt", animations);
			GetApplication()->InvokeInMainThreadAndWait([=, &animations]()
			{
				scriptDocumentView->GetBoundsComposition()->SetAssociatedCursor(GetCurrentController()->ResourceService()->GetDefaultSystemCursor());
				GuiDocumentElement* element=GuiDocumentElement::Create();
				element->SetDocument(document);

				GuiBoundsComposition* composition=new GuiBoundsComposition;
				composition->SetOwnedElement(element);
				composition->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElement);
				composition->SetAlignmentToParent(Margin(10, 10, 10, 10));
				scriptDocumentView->GetContainerComposition()->AddChild(composition);

				for(int i=0;i<animations.Count();i++)
				{
					Ptr<GifAnimation> gifAnimation=new GifAnimation(animations[i]->imageRun, animations[i]->paragraphIndex, element);
					this->GetGraphicsHost()->GetAnimationManager()->AddAnimation(gifAnimation);
				}
			});
		});

		this->GetContainerComposition()->AddChild(scriptDocumentView->GetBoundsComposition());
	}
};

void GuiMain()
{
	GuiRichTextWindow window;
	GetApplication()->Run(&window);
}