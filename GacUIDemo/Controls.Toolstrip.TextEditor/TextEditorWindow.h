#ifndef GACUIDEMO_CONTROLS_TOOLSTRIP_TEXTEDITOR
#define GACUIDEMO_CONTROLS_TOOLSTRIP_TEXTEDITOR

#include "..\..\Public\Source\GacUIWindows.h"

class TextEditorWindow : public GuiWindow
{
private:
	GuiToolstripMenuBar*			menuBar;
	GuiToolstripToolbar*			toolbar;
	GuiMultilineTextBox*			textBox;
	GuiShortcutKeyManager*			shortcutKeyManager;

	GuiToolstripCommand*			commandFileNew;
	GuiToolstripCommand*			commandFileOpen;
	GuiToolstripCommand*			commandFileSave;
	GuiToolstripCommand*			commandFileSaveAs;
	GuiToolstripCommand*			commandFileExit;
	GuiToolstripCommand*			commandEditUndo;
	GuiToolstripCommand*			commandEditRedo;
	GuiToolstripCommand*			commandEditCut;
	GuiToolstripCommand*			commandEditCopy;
	GuiToolstripCommand*			commandEditPaste;
	GuiToolstripCommand*			commandEditDelete;
	GuiToolstripCommand*			commandEditSelect;
	GuiToolstripCommand*			commandFormatFont;

	void							InitializeCommand();
	void							InitializeMenuBar();
	void							InitializeToolBar();
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

	void							commandFileNew_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandFileOpen_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandFileSave_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandFileSaveAs_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandFileExit_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandEditUndo_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandEditRedo_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandEditCut_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandEditCopy_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandEditPaste_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandEditDelete_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandEditSelect_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandEditGoto_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandFormatFont_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
public:
	TextEditorWindow();
	~TextEditorWindow();
};

#endif