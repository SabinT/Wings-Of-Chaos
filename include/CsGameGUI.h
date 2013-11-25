#pragma once

#include <CEGUIForwardRefs.h>

// ********** sub menu etc  declaration *******************
// Note: each component starts and cleans itself up
class CsGamePausedMenu {
public:
	CEGUI::Window*	mWindow;	
	
	CsGamePausedMenu();
	~CsGamePausedMenu();

	bool KeyDown(const CEGUI::EventArgs &e);

	bool ResumeButtonPressed(const CEGUI::EventArgs &e);
	bool QuitButtonPressed(const CEGUI::EventArgs &e);
};

class CsGameObjectivesPanel {
public:
	CEGUI::Window*	mWindow;

	CsGameObjectivesPanel();
	~CsGameObjectivesPanel();

	// doesnt do anything on its own, is input "transparent"
};

class CsGameConsoleWindow {
public:
	CEGUI::Window*	mWindow;
	
	CsGameConsoleWindow();
	~CsGameConsoleWindow();

	bool KeyDown(const CEGUI::EventArgs &e);
};

class CsGameHUD {
public:
	CEGUI::Window*	mWindow;
	
	CsGameHUD();
	~CsGameHUD();
};

// **************** The class that  puts it all together ***************
class CsGameGUI {
public:
	// load GUI sheets, give all the interfaces their windows, set initial visibility etc.
	CsGameGUI();
	// remove the loaded windows
	~CsGameGUI();

	// the root sheet
	CEGUI::Window*	mWindow;

	// messageboard thingies
	CEGUI::Window* mMessageBoard;
	CEGUI::Window* mMessageTitle;
	CEGUI::Window* mMessageText;

	// in case this one gets focus
	bool KeyDown(const CEGUI::EventArgs &e);
	
	void ClearHistory();
	void WriteToConsole(const char *str);
	
	float mLastUsedTime;

	// messageboard timeouts etc.
	void Update();
	void ShowMessage(std::string &title, std::string &text);

	// the interface, initialized using their constructors
	CsGamePausedMenu *mPausedMenu;
	CsGameHUD *mHUD;
	CsGameObjectivesPanel *mObjectivesPanel;
	CsGameConsoleWindow *mConsoleWindow;
};
// *********************************************************************

