#include "DocumentEditorWindow.h"

void SetImage(GuiToolstripCommand* command, const WString& imagePath)
{
	command->SetImage(new GuiImageData(GetCurrentController()->ImageService()->CreateImageFromFile(imagePath), 0));
}

WString GetResourceFolder()
{
	WString exePath=GetFolderPath(GetApplication()->GetExecutablePath());
	return exePath+L"..\\Resources\\";
}

void DocumentEditorWindow::InitializeCommand()
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
		commandStyleBold=new GuiToolstripCommand;;
		commandStyleBold->SetText(L"Bold");
		SetImage(commandStyleBold, resourceFolder+L"s_Bold.png");
		this->AddComponent(commandStyleBold);
	}
	{
		commandStyleItalic=new GuiToolstripCommand;;
		commandStyleItalic->SetText(L"Italic");
		SetImage(commandStyleItalic, resourceFolder+L"s_Italic.png");
		this->AddComponent(commandStyleItalic);
	}
	{
		commandStyleUnderline=new GuiToolstripCommand;;
		commandStyleUnderline->SetText(L"Underline");
		SetImage(commandStyleUnderline, resourceFolder+L"s_Underline.png");
		this->AddComponent(commandStyleUnderline);
	}
	{
		commandStyleStrikeline=new GuiToolstripCommand;;
		commandStyleStrikeline->SetText(L"Strikeline");
		SetImage(commandStyleStrikeline, resourceFolder+L"s_Strike.png");
		this->AddComponent(commandStyleStrikeline);
	}
	{
		commandStyleFont=new GuiToolstripCommand;;
		commandStyleFont->SetText(L"Font...");
		SetImage(commandStyleFont, resourceFolder+L"s_Font.png");
		this->AddComponent(commandStyleFont);
	}
	{
		commandStyleForeColor=new GuiToolstripCommand;;
		commandStyleForeColor->SetText(L"Text Color...");
		SetImage(commandStyleForeColor, resourceFolder+L"s_Color.png");
		this->AddComponent(commandStyleForeColor);
	}
	{
		commandStyleBackColor=new GuiToolstripCommand;;
		commandStyleBackColor->SetText(L"Background Color...");
		SetImage(commandStyleBackColor, resourceFolder+L"s_Background.png");
		this->AddComponent(commandStyleBackColor);
	}
	{
		commandStyleAlignLeft=new GuiToolstripCommand;;
		commandStyleAlignLeft->SetText(L"Left Align");
		SetImage(commandStyleAlignLeft, resourceFolder+L"s_AlignLeft.png");
		this->AddComponent(commandStyleAlignLeft);
	}
	{
		commandStyleAlignCenter=new GuiToolstripCommand;;
		commandStyleAlignCenter->SetText(L"Center Align");
		SetImage(commandStyleAlignCenter, resourceFolder+L"s_AlignCenter.png");
		this->AddComponent(commandStyleAlignCenter);
	}
	{
		commandStyleAlignRight=new GuiToolstripCommand;;
		commandStyleAlignRight->SetText(L"Right Align");
		SetImage(commandStyleAlignRight, resourceFolder+L"s_AlignRight.png");
		this->AddComponent(commandStyleAlignRight);
	}
}

void DocumentEditorWindow::InitializeMenuBar()
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
		->Button(0, L"Style")
			->BeginSubMenu()
			->Button(commandStyleBold)
			->Button(commandStyleItalic)
			->Button(commandStyleUnderline)
			->Button(commandStyleStrikeline)
			->Splitter()
			->Button(commandStyleFont)
			->Button(commandStyleForeColor)
			->Button(commandStyleBackColor)
			->Splitter()
			->Button(commandStyleAlignLeft)
			->Button(commandStyleAlignCenter)
			->Button(commandStyleAlignRight)
			->EndSubMenu()
		;
}

void DocumentEditorWindow::InitializeToolBar()
{
	ToolBar=g::NewToolBar();
	ToolBar->GetBoundsComposition()->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);
	ToolBar->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));

	ToolBar->GetBuilder()
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
		->Splitter()
		->Button(commandStyleBold)
		->Button(commandStyleItalic)
		->Button(commandStyleUnderline)
		->Button(commandStyleStrikeline)
		->Splitter()
		->Button(commandStyleFont)
		->Button(commandStyleForeColor)
		->Button(commandStyleBackColor)
		->Splitter()
		->Button(commandStyleAlignLeft)
		->Button(commandStyleAlignCenter)
		->Button(commandStyleAlignRight)
		;
}

void DocumentEditorWindow::InitializeComponents()
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
		cell->AddChild(ToolBar->GetBoundsComposition());
	}

	// create the text box
	{
		GuiCellComposition* cell=new GuiCellComposition;
		table->AddChild(cell);
		cell->SetSite(2, 0, 1, 1);
		cell->SetInternalMargin(Margin(1, 1, 1, 1));

		// create the menu bar
		textBox=g::NewDocumentViewer();
		textBox->SetEditMode(GuiDocumentViewer::Editable);
		textBox->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
		cell->AddChild(textBox->GetBoundsComposition());

		Ptr<DocumentModel> model=DocumentModel::LoadFromXml(GetResourceFolder()+L"XmlDocDemo.xml");
		textBox->SetDocument(model);
	}

	// set the preferred minimum client size
	this->GetBoundsComposition()->SetPreferredMinSize(Size(640, 480));
	// call this to calculate the size immediately if any indirect content in the table changes
	// so that the window can calcaulte its correct size before calling the MoveToScreenCenter()
	this->ForceCalculateSizeImmediately();
	// move to the screen center
	this->MoveToScreenCenter();
}