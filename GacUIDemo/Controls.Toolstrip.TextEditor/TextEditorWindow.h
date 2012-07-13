#ifndef GACUIDEMO_CONTROLS_TOOLSTRIP_TEXTEDITOR
#define GACUIDEMO_CONTROLS_TOOLSTRIP_TEXTEDITOR

#include "..\..\Public\Source\GacUI.h"

class TextEditorWindow : public GuiWindow
{
private:
	GuiMenuBar*						menuBar;
	GuiMultilineTextBox*			textBox;
	GuiMenuButton*					menuFile;
	GuiMenuButton*					menuFileNew;
	GuiMenuButton*					menuFileOpen;
	GuiMenuButton*					menuFileSave;
	GuiMenuButton*					menuFileSaveAs;
	GuiMenuButton*					menuFileExit;
	GuiMenuButton*					menuEdit;
	GuiMenuButton*					menuEditUndo;
	GuiMenuButton*					menuEditRedo;
	GuiMenuButton*					menuEditCut;
	GuiMenuButton*					menuEditCopy;
	GuiMenuButton*					menuEditPaste;
	GuiMenuButton*					menuEditDelete;
	GuiMenuButton*					menuEditSelect;
	GuiMenuButton*					menuFormat;
	GuiMenuButton*					menuFormatFont;

	void							InitializeMenuBar();
	void							InitializeComponents();
private:

/******************************************************************************
File Operation
******************************************************************************/

	WString							fileName;

	void							UpdateWindowTitle();
	bool							TryOpen();
	bool							TrySave(bool ignoreCurrentFileName);
	bool							TryNew();
	bool							PromptSaveUnchanged();
	void							window_WindowClosing(GuiGraphicsComposition* sender, GuiRequestEventArgs& arguments);

/******************************************************************************
Command Updating
******************************************************************************/

	static const int				ClipboardCommands=1;
	static const int				SelectionCommands=2;
	static const int				UndoRedoCommands=4;
	static const int				AllCommands=7;

	void							UpdateMenuItems(int commands);
	void							window_ClipboardUpdated(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							textBox_SelectionChanged(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							textBox_TextChanged(GuiGraphicsComposition* sender, GuiEventArgs& arguments);

/******************************************************************************
Menu Handlers
******************************************************************************/

	void							menuFileNew_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							menuFileOpen_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							menuFileSave_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							menuFileSaveAs_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							menuFileExit_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							menuEditUndo_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							menuEditRedo_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							menuEditCut_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							menuEditCopy_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							menuEditPaste_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							menuEditDelete_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							menuEditSelect_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							menuEditGoto_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							menuFormatFont_Clicked(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
public:
	TextEditorWindow();
	~TextEditorWindow();
};

#endif