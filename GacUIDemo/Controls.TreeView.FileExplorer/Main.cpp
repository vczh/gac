#include "..\CommonLibrary\FileSystemInformation.h"

using namespace vl::collections;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	return SetupWindowsDirect2DRenderer();
}

/***********************************************************************
SortingAndFilteringWindow
***********************************************************************/

class FileExplorerWindow : public GuiWindow
{
private:
	GuiTreeView*					treeView;

	void OnNodeExpanded(GuiGraphicsComposition* sender, GuiNodeEventArgs& arguments)
	{
		tree::MemoryNodeProvider* parent=treeView->Nodes()->GetMemoryNode(arguments.node);
		if(parent->Children().Count()==1)
		{
			tree::MemoryNodeProvider* child=parent->Children()[0].Obj();
			Ptr<tree::TreeViewItem> childItem=child->GetData().Cast<tree::TreeViewItem>();
			if(childItem->text==L"Loading..." && !childItem->tag)
			{
				GetApplication()->InvokeAsync([=]()
				{
					// get back the full path from the item
					Ptr<tree::TreeViewItem> item=parent->GetData().Cast<tree::TreeViewItem>();
					WString path=item->tag.Cast<ObjectBox<WString>>()->Unbox();
					if(path[path.Length()-1]!=L'\\')
					{
						path+=L"\\";
					}

					// add sub folders and sub files
					List<WString> directories, files;
					SearchDirectoriesAndFiles(path, directories, files);

					GetApplication()->InvokeInMainThreadAndWait([=, &directories, &files]()
					{
						FOREACH(WString, file, directories)
						{
							FileExplorerWindow::AddFolder(parent, path+file);
						}
						FOREACH(WString, file, files)
						{
							FileExplorerWindow::AddFile(parent, path+file);
						}
						// remove the  "Loading..." node
						parent->Children().RemoveAt(0);
					});
				});
			}
		}
	}

	static int AddFile(tree::MemoryNodeProvider* parent, const WString& path)
	{
		Ptr<tree::TreeViewItem> item=new tree::TreeViewItem;
		// set the item text using the display name of the file
		item->text=GetFileDisplayName(path);
		// set the image using the file icon
		item->image=GetFileIcon(path, SHGFI_SMALLICON | SHGFI_ICON);
		// tag the full path to the item
		item->tag=new ObjectBox<WString>(path);

		int index=parent->Children().Add(new tree::MemoryNodeProvider(item));
		return index;
	}

	static void AddFolder(tree::MemoryNodeProvider* parent, const WString& path)
	{
		int index=AddFile(parent, path);

		// Add the "loading" item under a folder
		Ptr<tree::TreeViewItem> loading=new tree::TreeViewItem;
		loading->text=L"Loading...";
		parent->Children()[index]->Children().Add(new tree::MemoryNodeProvider(loading));
	}

	void InitializeFileSystem()
	{
		wchar_t drives[1024];
		GetLogicalDriveStrings(sizeof(drives), drives);

		wchar_t* reading=drives;
		while(true)
		{
			WString drive=reading;
			if(drive==L"")
			{
				break;
			}
			else
			{
				AddFolder(treeView->Nodes().Obj(), drive);
				reading+=drive.Length()+1;
			}
		}
	}
public:
	FileExplorerWindow()
		:GuiWindow(GetCurrentTheme()->CreateWindowStyle())
	{
		this->SetText(L"Controls.TreeView.FileExplorerWindow");
		
		// create tree view control to display the local file system
		treeView=g::NewTreeView();
		treeView->SetHorizontalAlwaysVisible(false);
		treeView->SetVerticalAlwaysVisible(false);
		treeView->GetBoundsComposition()->SetAlignmentToParent(Margin(4, 4, 4, 4));
		// listen to the NodeExpanded event to load the folder content when a folder node is opened
		treeView->NodeExpanded.AttachMethod(this, &FileExplorerWindow::OnNodeExpanded);
		this->AddChild(treeView);

		InitializeFileSystem();

		// set the preferred minimum client size
		this->GetBoundsComposition()->SetPreferredMinSize(Size(640, 480));
		// call this to calculate the size immediately if any indirect content in the table changes
		// so that the window can calcaulte its correct size before calling the MoveToScreenCenter()
		this->ForceCalculateSizeImmediately();
		// move to the screen center
		this->MoveToScreenCenter();
	}

	~FileExplorerWindow()
	{
	}
};

/***********************************************************************
GuiMain
***********************************************************************/

void GuiMain()
{
	GuiWindow* window=new FileExplorerWindow;
	GetApplication()->Run(window);
	delete window;
}