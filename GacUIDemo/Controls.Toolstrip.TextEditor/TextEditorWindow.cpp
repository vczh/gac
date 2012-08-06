#include "TextEditorWindow.h"

using namespace vl::collections;
using namespace vl::stream;

/******************************************************************************
File Operation
******************************************************************************/

void TextEditorWindow::UpdateWindowTitle()
{
	if(fileName==L"")
	{
		this->SetText(L"Controls.Toolstrip.TextEditor[UNTITLED]");
	}
	else
	{
		this->SetText(L"Controls.Toolstrip.TextEditor["+fileName+L"]");
	}
}

bool TextEditorWindow::TryOpen()
{
	if(PromptSaveUnchanged())
	{
		List<WString> selectionFileNames;
		int selectionFilterIndex=0;
		if(GetCurrentController()->DialogService()->ShowFileDialog(
			this->GetNativeWindow(),
			selectionFileNames,
			selectionFilterIndex,
			INativeDialogService::FileDialogOpen,
			L"Open a text document",
			L"",
			L"",
			L".txt",
			L"Text Files(*.txt)|*.txt|All Files(*.*)|*.*",
			(INativeDialogService::FileDialogOptions)
				( INativeDialogService::FileDialogAddToRecent
				| INativeDialogService::FileDialogDereferenceLinks
				| INativeDialogService::FileDialogDirectoryMustExist
				| INativeDialogService::FileDialogFileMustExist
				)
			))
		{
			fileName=selectionFileNames[0];
			FileStream fileStream(fileName, FileStream::ReadOnly);
			BomDecoder decoder;
			DecoderStream decoderStream(fileStream, decoder);
			StreamReader reader(decoderStream);
			textBox->SetText(reader.ReadToEnd());
			textBox->Select(TextPos(0, 0), TextPos(0, 0));
			textBox->ClearUndoRedo();
			textBox->NotifyModificationSaved();
			return true;
		}
	}
	return true;
}

bool TextEditorWindow::TrySave(bool ignoreCurrentFileName)
{
	if(fileName==L"" || ignoreCurrentFileName)
	{
		List<WString> selectionFileNames;
		int selectionFilterIndex=0;
		if(GetCurrentController()->DialogService()->ShowFileDialog(
			this->GetNativeWindow(),
			selectionFileNames,
			selectionFilterIndex,
			INativeDialogService::FileDialogSave,
			L"Save a text document",
			L"",
			L"",
			L".txt",
			L"Text Files(*.txt)|*.txt|All Files(*.*)|*.*",
			(INativeDialogService::FileDialogOptions)
				( INativeDialogService::FileDialogAddToRecent
				| INativeDialogService::FileDialogDereferenceLinks
				| INativeDialogService::FileDialogDirectoryMustExist
				| INativeDialogService::FileDialogPromptOverwriteFile
				)
			))
		{
			fileName=selectionFileNames[0];
		}
		else
		{
			return false;
		}
	}
	{
		FileStream fileStream(fileName, FileStream::WriteOnly);
		BomEncoder encoder(BomEncoder::Utf8);
		EncoderStream encoderStream(fileStream, encoder);
		StreamWriter writer(encoderStream);
		writer.WriteString(textBox->GetText());
		textBox->NotifyModificationSaved();
	}
	return true;
}

bool TextEditorWindow::TryNew()
{
	if(PromptSaveUnchanged())
	{
		fileName=L"";
		textBox->SetText(L"");
		textBox->ClearUndoRedo();
		textBox->NotifyModificationSaved();
		return true;
	}
	else
	{
		return false;
	}
}

bool TextEditorWindow::PromptSaveUnchanged()
{
	if(textBox->GetModified())
	{
		INativeDialogService::MessageBoxButtonsOutput result=GetCurrentController()->DialogService()->ShowMessageBox(
			this->GetNativeWindow(),
			L"The document is modified. Do you want to save it before continue?",
			this->GetText(),
			INativeDialogService::DisplayYesNoCancel,
			INativeDialogService::DefaultFirst,
			INativeDialogService::IconQuestion
			);
		switch(result)
		{
		case INativeDialogService::SelectYes:
			return TrySave(false);
		case INativeDialogService::SelectNo:
			return true;
		default:
			return false;
		}
	}
	else
	{
		return true;
	}
}

void TextEditorWindow::window_WindowClosing(GuiGraphicsComposition* sender, GuiRequestEventArgs& arguments)
{
	if(!PromptSaveUnchanged())
	{
		arguments.cancel=true;
	}
}

/******************************************************************************
Command Updating
******************************************************************************/

void TextEditorWindow::UpdateMenuItems(int commands)
{
	if(commands & ClipboardCommands)
	{
		commandEditPaste->SetEnabled(GetCurrentController()->ClipboardService()->ContainsText());
	}
	if(commands & SelectionCommands)
	{
		commandEditCut->SetEnabled(textBox->CanCut());
		commandEditCopy->SetEnabled(textBox->CanCopy());
		commandEditDelete->SetEnabled(textBox->GetCaretBegin()!=textBox->GetCaretEnd());
	}
	if(commands & UndoRedoCommands)
	{
		commandEditUndo->SetEnabled(textBox->CanUndo());
		commandEditRedo->SetEnabled(textBox->CanRedo());
	}
}

void TextEditorWindow::window_ClipboardUpdated(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	UpdateMenuItems(ClipboardCommands);
}

void TextEditorWindow::textBox_SelectionChanged(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	UpdateMenuItems(SelectionCommands);
}

void TextEditorWindow::textBox_TextChanged(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	UpdateMenuItems(UndoRedoCommands);
}

/******************************************************************************
Menu Handlers
******************************************************************************/

void TextEditorWindow::commandFileNew_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	TryNew();
	UpdateMenuItems(AllCommands);
	UpdateWindowTitle();
}

void TextEditorWindow::commandFileOpen_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	TryOpen();
	UpdateMenuItems(AllCommands);
	UpdateWindowTitle();
}

void TextEditorWindow::commandFileSave_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	TrySave(false);
	UpdateMenuItems(AllCommands);
	UpdateWindowTitle();
}

void TextEditorWindow::commandFileSaveAs_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	TrySave(true);
	UpdateMenuItems(AllCommands);
	UpdateWindowTitle();
}

void TextEditorWindow::commandFileExit_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	this->Close();
}

void TextEditorWindow::commandEditUndo_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	textBox->Undo();
}

void TextEditorWindow::commandEditRedo_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	textBox->Redo();
}

void TextEditorWindow::commandEditCut_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	textBox->Cut();
}

void TextEditorWindow::commandEditCopy_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	textBox->Copy();
}

void TextEditorWindow::commandEditPaste_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	textBox->Paste();
}

void TextEditorWindow::commandEditDelete_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	textBox->SetSelectionText(L"");
}

void TextEditorWindow::commandEditSelect_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	textBox->SelectAll();
}

void TextEditorWindow::commandFormatFont_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	FontProperties font=textBox->GetFont();
	Color color;
	if(GetCurrentController()->DialogService()->ShowFontDialog(
		this->GetNativeWindow(),
		font,
		color,
		true,
		false,
		true))
	{
		textBox->SetFont(font);
	}
}

TextEditorWindow::TextEditorWindow()
	:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
{
	InitializeComponents();
	UpdateWindowTitle();
	UpdateMenuItems(AllCommands);

	this->WindowClosing.AttachMethod(this, &TextEditorWindow::window_WindowClosing);
	this->ClipboardUpdated.AttachMethod(this, &TextEditorWindow::window_ClipboardUpdated);

	textBox->TextChanged.AttachMethod(this, &TextEditorWindow::textBox_TextChanged);
	textBox->SelectionChanged.AttachMethod(this, &TextEditorWindow::textBox_SelectionChanged);

	commandFileNew->Executed.AttachMethod(this, &TextEditorWindow::commandFileNew_Executed);
	commandFileOpen->Executed.AttachMethod(this, &TextEditorWindow::commandFileOpen_Executed);
	commandFileSave->Executed.AttachMethod(this, &TextEditorWindow::commandFileSave_Executed);
	commandFileSaveAs->Executed.AttachMethod(this, &TextEditorWindow::commandFileSaveAs_Executed);
	commandFileExit->Executed.AttachMethod(this, &TextEditorWindow::commandFileExit_Executed);
	commandEditUndo->Executed.AttachMethod(this, &TextEditorWindow::commandEditUndo_Executed);
	commandEditRedo->Executed.AttachMethod(this, &TextEditorWindow::commandEditRedo_Executed);
	commandEditCut->Executed.AttachMethod(this, &TextEditorWindow::commandEditCut_Executed);
	commandEditCopy->Executed.AttachMethod(this, &TextEditorWindow::commandEditCopy_Executed);
	commandEditPaste->Executed.AttachMethod(this, &TextEditorWindow::commandEditPaste_Executed);
	commandEditDelete->Executed.AttachMethod(this, &TextEditorWindow::commandEditDelete_Executed);
	commandEditSelect->Executed.AttachMethod(this, &TextEditorWindow::commandEditSelect_Executed);
	commandFormatFont->Executed.AttachMethod(this, &TextEditorWindow::commandFormatFont_Executed);
}

TextEditorWindow::~TextEditorWindow()
{
}