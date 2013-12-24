#ifndef GACUIDEMO_CONTROLS_TOOLSTRIP_TEXTEDITOR
#define GACUIDEMO_CONTROLS_TOOLSTRIP_TEXTEDITOR

#include "..\..\Public\Source\GacUI.h"

class DocumentEditorWindow : public GuiWindow
{
private:
	GuiToolstripMenuBar*			menuBar;
	GuiToolstripToolBar*			ToolBar;
	GuiDocumentViewer*				textBox;
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
	GuiToolstripCommand*			commandStyleBold;
	GuiToolstripCommand*			commandStyleItalic;
	GuiToolstripCommand*			commandStyleUnderline;
	GuiToolstripCommand*			commandStyleStrikeline;
	GuiToolstripCommand*			commandStyleFont;
	GuiToolstripCommand*			commandStyleForeColor;
	GuiToolstripCommand*			commandStyleBackColor;
	GuiToolstripCommand*			commandStyleAlignLeft;
	GuiToolstripCommand*			commandStyleAlignCenter;
	GuiToolstripCommand*			commandStyleAlignRight;

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
	void							commandStyleBold_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandStyleItalic_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandStyleUnderline_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandStyleStrikeline_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandStyleFont_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandStyleForeColor_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandStyleBackColor_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandStyleAlignLeft_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandStyleAlignCenter_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void							commandStyleAlignRight_Executed(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
public:
	DocumentEditorWindow();
	~DocumentEditorWindow();
};

#endif