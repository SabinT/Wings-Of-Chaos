#include <precompiled.h>
#include <CsGameGUI.h>

#include <CEGUI.h>
#include <CsGame.h>

const float MESSAGE_TIMEOUT = 4.0F;

void InterpretCommand (CEGUI::String str);

// ***** Convenience function for dispatching messages **************
void SendMessage (CsGUIEvent::GUIEventType type) {
	CsLogicMessage logicMessage;
	CsRenderMessage renderMessage;

	logicMessage.type = CsLogicMessage::GUI_EVENT;
	renderMessage.type = CsRenderMessage::GUI_EVENT;

	logicMessage.info.guiEvent.type = type;
	renderMessage.info.guiEvent.type = type;

	CsGame::GetSingletonPtr()->GetLogicTempQueue()->push(logicMessage);
	CsGame::GetSingletonPtr()->GetRenderQueue()->push(renderMessage);
}

void SendMessageLogic (CsGUIEvent::GUIEventType type) {
	CsLogicMessage logicMessage;

	logicMessage.type = CsLogicMessage::GUI_EVENT;
	logicMessage.info.guiEvent.type = type;
	CsGame::GetSingletonPtr()->GetLogicTempQueue()->push(logicMessage);
}

void SendMessageRender (CsGUIEvent::GUIEventType type) {
	CsRenderMessage renderMessage;

	renderMessage.type = CsRenderMessage::GUI_EVENT;
	renderMessage.info.guiEvent.type = type;
	CsGame::GetSingletonPtr()->GetRenderQueue()->push(renderMessage);
}

// *********************** Paused Menu ******************************
CsGamePausedMenu::CsGamePausedMenu() {
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	mWindow = wmgr.getWindow("Game/PausedMenu");
	// or maybe load from a sheet and add as a child to the root sheet

	mWindow->hide();

	// subscribe events
	wmgr.getWindow("Game/PausedMenu/Resume")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CsGamePausedMenu::ResumeButtonPressed, this));
	wmgr.getWindow("Game/PausedMenu/Quit")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CsGamePausedMenu::QuitButtonPressed, this));
	mWindow->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&CsGamePausedMenu::KeyDown, this));
}

CsGamePausedMenu::~CsGamePausedMenu() {
	// the GameGUI will destroy all windows, so never bother
}

// the handlers
bool CsGamePausedMenu::KeyDown(const CEGUI::EventArgs &e) {
	CEGUI::KeyEventArgs *key = (CEGUI::KeyEventArgs *)(&e);
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();

	if (key->scancode == CEGUI::Key::Escape) {
		wmgr.getWindow("Game/HUD")->show();
		wmgr.getWindow("Game/PausedMenu")->hide();

		CEGUI::MouseCursor::getSingleton().hide( );

		SendMessage (CsGUIEvent::RESUME_REQUEST);
	}
	return true;
}

bool CsGamePausedMenu::ResumeButtonPressed(const CEGUI::EventArgs &e) {
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();

	wmgr.getWindow("Game/HUD")->show();
	wmgr.getWindow("Game/PausedMenu")->hide();

	CEGUI::MouseCursor::getSingleton().hide( );

	SendMessage (CsGUIEvent::RESUME_REQUEST);

	return true;
}

bool CsGamePausedMenu::QuitButtonPressed(const CEGUI::EventArgs &e) {
	// send quit message
	SendMessageRender (CsGUIEvent::RESUME_REQUEST);
	SendMessageLogic (CsGUIEvent::QUIT_REQUEST);

	return true;
}
// *********************************************************************************

// ****************** Objectives Panel *********************************************
CsGameObjectivesPanel::CsGameObjectivesPanel() {
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	mWindow = wmgr.getWindow("Game/ObjectivesPanel");

	mWindow->hide();
	mWindow->setMousePassThroughEnabled(true);
}

CsGameObjectivesPanel::~CsGameObjectivesPanel() {

}
// *********************************************************************************

// ****************** HUD Panel ****************************************************
CsGameHUD::CsGameHUD() {
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	mWindow = wmgr.getWindow("Game/HUD");

	mWindow->show();
	mWindow->setMousePassThroughEnabled(true);
}

CsGameHUD::~CsGameHUD() {

}
// *********************************************************************************

// ****************** Console Window ***********************************************
CsGameConsoleWindow::CsGameConsoleWindow() {
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	mWindow = wmgr.getWindow("Game/ConsoleWindow");

	mWindow->hide();
	mWindow->setMousePassThroughEnabled(true);

	CEGUI::Window* tmp;

	// event registration
	tmp = wmgr.getWindow("Game/ConsoleWindow/Input");
	tmp->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&CsGameConsoleWindow::KeyDown, this));
	tmp = wmgr.getWindow("Game/ConsoleWindow");
	tmp->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&CsGameConsoleWindow::KeyDown, this));
	tmp = wmgr.getWindow("Game/ConsoleWindow/History");
	tmp->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&CsGameConsoleWindow::KeyDown, this));
}

CsGameConsoleWindow::~CsGameConsoleWindow() {

}

bool CsGameConsoleWindow::KeyDown(const CEGUI::EventArgs &e) {
	CEGUI::KeyEventArgs *key = (CEGUI::KeyEventArgs *)(&e);

	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();

	CEGUI::Window* temp;

	if (key->scancode == CEGUI::Key::F12 || key->scancode == CEGUI::Key::Escape) {
		temp = wmgr.getWindow("Game/ConsoleWindow");
		temp->hide();

		CEGUI::MouseCursor::getSingleton().hide( );
		wmgr.getWindow("Game")->activate();

		CEGUI::MouseCursor::getSingleton().hide( );
	} 

	if (key->scancode == CEGUI::Key::Return) {
		// just echo the text in history now
		CEGUI::MultiLineEditbox* history = (CEGUI::MultiLineEditbox*) wmgr.getWindow("Game/ConsoleWindow/History");
		CEGUI::String str = wmgr.getWindow("Game/ConsoleWindow/Input")->getText();

		InterpretCommand(str);

		history->appendText(str);
		history->setCaratIndex(history->getText().length());
		history->ensureCaratIsVisible();

		wmgr.getWindow("Game/ConsoleWindow/Input")->setText("");
	}

	if (key->scancode == CEGUI::Key::Backspace) {
		// just echo the text in history now
		CEGUI::String str = wmgr.getWindow("Game/ConsoleWindow/Input")->getText();
		str = str.substr(0, str.length() - 1);
		wmgr.getWindow("Game/ConsoleWindow/Input")->setText(str);
	}

	return true;
}

void InterpretCommand (CEGUI::String str) {
	if (str.find("debug\0") != CEGUI::String::npos) {
		SendMessageLogic(CsGUIEvent::DEBUG_TOGGLE_REQUEST);
	}
	else if (str.find("quit\0") != CEGUI::String::npos) {
		SendMessageLogic(CsGUIEvent::QUIT_REQUEST);
	} else if (str.find("ship\0") != CEGUI::String::npos) {
		SendMessageLogic(CsGUIEvent::CREATE_SHIP);
	} else if (str.find("cam\0") != CEGUI::String::npos) {
		SendMessageRender(CsGUIEvent::SHIP_CAMERA_CHANGE);
	}
}
// *********************************************************************************

// ********************* The root GUI sheet ******************************************************************
CsGameGUI::CsGameGUI() {
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();

	mWindow = wmgr.loadWindowLayout("gameGUI.layout"); 
	CEGUI::System::getSingleton().setGUISheet(mWindow);

	mWindow->activate();
	// recieves input, but to prevent accidental activation during menu
	mWindow->setMousePassThroughEnabled(true);

	CEGUI::MouseCursor::getSingleton().hide( );

	// events for the root sheet
	mWindow->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&CsGameGUI::KeyDown, this));

	// message board
	mMessageBoard = wmgr.getWindow("Game/MessageBoard");
	mMessageTitle = wmgr.getWindow("Game/MessageBoard/Title");
	mMessageText = wmgr.getWindow("Game/MessageBoard/Text");

	mMessageBoard->setMousePassThroughEnabled(true);
	mMessageBoard->deactivate();
	mMessageBoard->hide();

	// create child windows
	mPausedMenu = new CsGamePausedMenu;
	mObjectivesPanel = new CsGameObjectivesPanel;
	mHUD = new CsGameHUD;
	mConsoleWindow = new CsGameConsoleWindow;

	// a few ogre overlays
	Ogre::OverlayManager::getSingleton().getByName("MenuOverlay")->hide();

	//Ogre::OverlayManager::getSingleton().getByName("TargetOverlay")->show();
	Ogre::OverlayManager::getSingleton().getByName("InfoOverlay")->show();
	// TODO: re-enable MiniMapOverlay
	// Ogre::OverlayManager::getSingleton().getByName("MiniMapOverlay")->show();
	//Ogre::OverlayManager::getSingleton().getByName("InfoOverlay")->getChild("Info/Panel")->getChild("Info/Text")->setCaption("This is War baby!");
}

CsGameGUI::~CsGameGUI() {
	Ogre::OverlayManager::getSingleton().getByName("TargetLockedOverlay")->hide();
	Ogre::OverlayManager::getSingleton().getByName("TargetOverlay")->hide();
	//Ogre::OverlayManager::getSingleton().destroyAll();
	//Ogre::OverlayManager::getSingleton().destroyAllOverlayElements();

	delete mPausedMenu;
	delete mObjectivesPanel;
	delete mHUD;
	delete mConsoleWindow;

	CEGUI::WindowManager::getSingleton().destroyAllWindows();
}

bool CsGameGUI::KeyDown(const CEGUI::EventArgs &e) {
	CEGUI::KeyEventArgs *key = (CEGUI::KeyEventArgs *)(&e);

	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();

	CEGUI::Window* temp;

	if (key->scancode == CEGUI::Key::Escape) {
		wmgr.getWindow("Game/HUD")->hide();
		wmgr.getWindow("Game/PausedMenu")->show();
		wmgr.getWindow("Game/PausedMenu")->activate();

		CEGUI::MouseCursor::getSingleton().show( );
		
		SendMessage (CsGUIEvent::PAUSE_REQUEST);
	}

	else if (key->scancode == CEGUI::Key::F12) {
		temp = wmgr.getWindow("Game/ConsoleWindow");

		if (temp->isVisible()) {
			temp->hide();
			wmgr.getWindow("Game")->activate();
		}
		else {
			temp->show();
			wmgr.getWindow("Game/ConsoleWindow/Input")->activate();
		}
	}

	return true;
}

void CsGameGUI::WriteToConsole(const char *str) {
	CEGUI::MultiLineEditbox* history = (CEGUI::MultiLineEditbox*) CEGUI::WindowManager::getSingleton().getWindow("Game/ConsoleWindow/History");

	history->appendText(CEGUI::String(str));
	history->setCaratIndex(history->getText().length());
	history->ensureCaratIsVisible();
}

void CsGameGUI::ClearHistory() {
	CEGUI::MultiLineEditbox* history = (CEGUI::MultiLineEditbox*) CEGUI::WindowManager::getSingleton().getWindow("Game/ConsoleWindow/History");
	history->setText(CEGUI::String());
}

void CsGameGUI::ShowMessage(std::string &title, std::string &text) {
	mMessageTitle->setText(title);
	mMessageText->setText(text);
	mMessageBoard->show();

	mLastUsedTime = CsGame::GetSingletonPtr()->GetGameTime();
}

void CsGameGUI::Update() {
	if (CsGame::GetSingletonPtr()->mGameState != CsGame::POST_GAME) {
		if (mLastUsedTime + MESSAGE_TIMEOUT < CsGame::GetSingletonPtr()->GetGameTime()) mMessageBoard->hide();
	}
}