#include "TextEditorWindow.h"

void SetImage(GuiToolstripCommand* command, const WString& imagePath)
{
	command->SetImage(new GuiImageData(GetCurrentController()->ImageService()->CreateImageFromFile(imagePath), 0));
}

WString GetResourceFolder()
{
	wchar_t exePath[1024]={0};
	DWORD len=GetModuleFileName(NULL, exePath, 1024);
	while(exePath[--len]!=L'\\');
	exePath[len+1]=0;
	wcscat_s(exePath, L"..\\Resources\\");
	return exePath;
}

void TextEditorWindow::InitializeCommand()
{
	WString resourceFolder=GetResourceFolder();
	{
		commandFileNew=new GuiToolstripCommand;;
		commandFileNew->SetText(L"New");
		SetImage(commandFileNew, resourceFolder+L"_New.png");
		commandFileNew->SetShortcut(shortcutKeyManager->CreateShortcut(true, false, false, L'N'));
		this->AddComponent(commandFileNew);
	}
	{
		commandFileOpen=new GuiToolstripCommand;;
		commandFileOpen->SetText(L"Open...");
		SetImage(commandFileOpen, resourceFolder+L"_Open.png");
		commandFileOpen->SetShortcut(shortcutKeyManager->CreateShortcut(true, false, false, L'O'));
		this->AddComponent(commandFileOpen);
	}
	{
		commandFileSave=new GuiToolstripCommand;;
		commandFileSave->SetText(L"Save");
		SetImage(commandFileSave, resourceFolder+L"_Save.png");
		commandFileSave->SetShortcut(shortcutKeyManager->CreateShortcut(true, false, false, L'S'));
		this->AddComponent(commandFileSave);
	}
	{
		commandFileSaveAs=new GuiToolstripCommand;;
		commandFileSaveAs->SetText(L"Save As...");
		SetImage(commandFileSaveAs, resourceFolder+L"_SaveAs.png");
		this->AddComponent(commandFileSaveAs);
	}
	{
		commandFileExit=new GuiToolstripCommand;;
		commandFileExit->SetText(L"Exit");
		this->AddComponent(commandFileExit);
	}
	{
		commandEditUndo=new GuiToolstripCommand;;
		commandEditUndo->SetText(L"Undo");
		SetImage(commandEditUndo, resourceFolder+L"_Undo.png");
		commandEditUndo->SetShortcut(shortcutKeyManager->CreateShortcut(true, false, false, L'Z'));
		this->AddComponent(commandEditUndo);
	}
	{
		commandEditRedo=new GuiToolstripCommand;;
		commandEditRedo->SetText(L"Redo");
		SetImage(commandEditRedo, resourceFolder+L"_Redo.png");
		commandEditRedo->SetShortcut(shortcutKeyManager->CreateShortcut(true, false, false, L'Y'));
		this->AddComponent(commandEditRedo);
	}
	{
		commandEditCut=new GuiToolstripCommand;;
		commandEditCut->SetText(L"Cut");
		SetImage(commandEditCut, resourceFolder+L"_Cut.png");
		commandEditCut->SetShortcut(shortcutKeyManager->CreateShortcut(true, false, false, L'X'));
		this->AddComponent(commandEditCut);
	}
	{
		commandEditCopy=new GuiToolstripCommand;;
		commandEditCopy->SetText(L"Copy");
		SetImage(commandEditCopy, resourceFolder+L"_Copy.png");
		commandEditCopy->SetShortcut(shortcutKeyManager->CreateShortcut(true, false, false, L'C'));
		this->AddComponent(commandEditCopy);
	}
	{
		commandEditPaste=new GuiToolstripCommand;;
		commandEditPaste->SetText(L"Paste");
		SetImage(commandEditPaste, resourceFolder+L"_Paste.png");
		commandEditPaste->SetShortcut(shortcutKeyManager->CreateShortcut(true, false, false, L'V'));
		this->AddComponent(commandEditPaste);
	}
	{
		commandEditDelete=new GuiToolstripCommand;;
		commandEditDelete->SetText(L"Delete");
		SetImage(commandEditDelete, resourceFolder+L"_Delete.png");
		this->AddComponent(commandEditDelete);
	}
	{
		commandEditSelect=new GuiToolstripCommand;;
		commandEditSelect->SetText(L"Select All");
		this->AddComponent(commandEditSelect);
		commandEditSelect->SetShortcut(shortcutKeyManager->CreateShortcut(true, false, false, L'A'));
	}
	{
		commandFormatFont=new GuiToolstripCommand;;
		commandFormatFont->SetText(L"Font...");
		this->AddComponent(commandFormatFont);
	}
}

void TextEditorWindow::InitializeMenuBar()
{
	menuBar=g::NewMenuBar();
	menuBar->GetBoundsComposition()->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);
	menuBar->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));

	menuBar->GetBuilder()
		->Button(0, L"File")
			->BeginSubMenu()
			->Button(commandFileNew)
			->Button(commandFileOpen)
			->Button(commandFileSave)
			->Button(commandFileSaveAs)
			->Splitter()
			->Button(commandFileExit)
			->EndSubMenu()
		->Button(0, L"Edit")
			->BeginSubMenu()
			->Button(commandEditUndo)
			->Button(commandEditRedo)
			->Splitter()
			->Button(commandEditCut)
			->Button(commandEditCopy)
			->Button(commandEditPaste)
			->Splitter()
			->Button(commandEditDelete)
			->Button(commandEditSelect)
			->EndSubMenu()
		->Button(0, L"Format")
			->BeginSubMenu()
			->Button(commandFormatFont)
			->EndSubMenu()
		;
}

void TextEditorWindow::InitializeToolBar()
{
	toolbar=g::NewToolbar();
	toolbar->GetBoundsComposition()->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);
	toolbar->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));

	toolbar->GetBuilder()
		->Button(commandFileNew)
		->Button(commandFileOpen)
		->Button(commandFileSave)
		->Button(commandFileSaveAs)
		->Splitter()
		->Button(commandEditUndo)
		->Button(commandEditRedo)
		->Splitter()
		->Button(commandEditCut)
		->Button(commandEditCopy)
		->Button(commandEditPaste)
		->Button(commandEditDelete)
		;
}

void TextEditorWindow::InitializeComponents()
{
	shortcutKeyManager=new GuiShortcutKeyManager();
	this->SetShortcutKeyManager(shortcutKeyManager);

	InitializeCommand();
	// create a table to place a menu bar and a text box
	GuiTableComposition* table=new GuiTableComposition;
	table->SetCellPadding(0);
	table->SetAlignmentToParent(Margin(0, 0, 0, 0));
	table->SetRowsAndColumns(3, 1);
	table->SetRowOption(0, GuiCellOption::MinSizeOption());
	table->SetRowOption(1, GuiCellOption::MinSizeOption());
	table->SetRowOption(2, GuiCellOption::PercentageOption(1.0));
	table->SetColumnOption(0, GuiCellOption::PercentageOption(1.0));
	this->GetContainerComposition()->AddChild(table);

	// create the menu bar
	{
		GuiCellComposition* cell=new GuiCellComposition;
		table->AddChild(cell);
		cell->SetSite(0, 0, 1, 1);
		
		InitializeMenuBar();
		cell->AddChild(menuBar->GetBoundsComposition());
	}

	// create the tool bar
	{
		GuiCellComposition* cell=new GuiCellComposition;
		table->AddChild(cell);
		cell->SetSite(1, 0, 1, 1);
		cell->SetInternalMargin(Margin(1, 0, 1, 0));
		
		InitializeToolBar();
		cell->AddChild(toolbar->GetBoundsComposition());
	}

	// create the text box
	{
		GuiCellComposition* cell=new GuiCellComposition;
		table->AddChild(cell);
		cell->SetSite(2, 0, 1, 1);
		cell->SetInternalMargin(Margin(1, 1, 1, 1));

		// create the menu bar
		textBox=g::NewMultilineTextBox();
		textBox->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
		cell->AddChild(textBox->GetBoundsComposition());
	}

	// set the preferred minimum client size
	this->GetBoundsComposition()->SetPreferredMinSize(Size(640, 480));
	// call this to calculate the size immediately if any indirect content in the table changes
	// so that the window can calcaulte its correct size before calling the MoveToScreenCenter()
	this->ForceCalculateSizeImmediately();
	// move to the screen center
	this->MoveToScreenCenter();
}