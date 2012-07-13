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
			UpdateWindowTitle();
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
		UpdateWindowTitle();
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
		UpdateWindowTitle();
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
		menuEditPaste->SetEnabled(GetCurrentController()->ClipboardService()->ContainsText());
	}
	if(commands & SelectionCommands)
	{
		menuEditCut->SetEnabled(textBox->CanCut());
		menuEditCopy->SetEnabled(textBox->CanCopy());
		menuEditDelete->SetEnabled(textBox->GetCaretBegin()!=textBox->GetCaretEnd());
	}
	if(commands & UndoRedoCommands)
	{
		menuEditUndo->SetEnabled(textBox->CanUndo());
		menuEditRedo->SetEnabled(textBox->CanRedo());
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

void TextEditorWindow::menuFileNew_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	TryNew();
}

void TextEditorWindow::menuFileOpen_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	TryOpen();
}

void TextEditorWindow::menuFileSave_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	TrySave(false);
}

void TextEditorWindow::menuFileSaveAs_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	TrySave(true);
}

void TextEditorWindow::menuFileExit_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	this->Close();
}

void TextEditorWindow::menuEditUndo_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	textBox->Undo();
}

void TextEditorWindow::menuEditRedo_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	textBox->Redo();
}

void TextEditorWindow::menuEditCut_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	textBox->Cut();
}

void TextEditorWindow::menuEditCopy_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	textBox->Copy();
}

void TextEditorWindow::menuEditPaste_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	textBox->Paste();
}

void TextEditorWindow::menuEditDelete_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	textBox->SetSelectionText(L"");
}

void TextEditorWindow::menuEditSelect_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
{
	textBox->SelectAll();
}

void TextEditorWindow::menuFormatFont_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments)
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

	menuFileNew->Clicked.AttachMethod(this, &TextEditorWindow::menuFileNew_Clicked);
	menuFileOpen->Clicked.AttachMethod(this, &TextEditorWindow::menuFileOpen_Clicked);
	menuFileSave->Clicked.AttachMethod(this, &TextEditorWindow::menuFileSave_Clicked);
	menuFileSaveAs->Clicked.AttachMethod(this, &TextEditorWindow::menuFileSaveAs_Clicked);
	menuFileExit->Clicked.AttachMethod(this, &TextEditorWindow::menuFileExit_Clicked);
	menuEditUndo->Clicked.AttachMethod(this, &TextEditorWindow::menuEditUndo_Clicked);
	menuEditRedo->Clicked.AttachMethod(this, &TextEditorWindow::menuEditRedo_Clicked);
	menuEditCut->Clicked.AttachMethod(this, &TextEditorWindow::menuEditCut_Clicked);
	menuEditCopy->Clicked.AttachMethod(this, &TextEditorWindow::menuEditCopy_Clicked);
	menuEditPaste->Clicked.AttachMethod(this, &TextEditorWindow::menuEditPaste_Clicked);
	menuEditDelete->Clicked.AttachMethod(this, &TextEditorWindow::menuEditDelete_Clicked);
	menuEditSelect->Clicked.AttachMethod(this, &TextEditorWindow::menuEditSelect_Clicked);
	menuFormatFont->Clicked.AttachMethod(this, &TextEditorWindow::menuFormatFont_Clicked);
}

TextEditorWindow::~TextEditorWindow()
{
}