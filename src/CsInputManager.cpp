#include <precompiled.h>
#include <CsInputManager.h>

#include <CEGUI.h>

CsInputManager::CsInputManager(void)
{
	for (int i = 0; i < Chaos::NUM_BUTTONS; i++) {
		mButtonState[i] = false;
	}

	mLogicQueue = NULL;
	mRenderQueue = NULL;

	mDisableInputLogic = false;
}


CsInputManager::~CsInputManager(void)
{
}

//------------------------------------------------------------------------------------
void CsInputManager::SetupInput(Ogre::RenderWindow *window,
								CsLockableQueue<CsRenderMessage> *renderQueue,
								CsLockableQueue<CsLogicMessage> *logicQueue) 
{
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    window->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem( pl );

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

	unsigned int width, height, depth;
    int left, top;
    window->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;

	mLogicQueue = logicQueue;
	mRenderQueue = renderQueue;
}

void CsInputManager::SetupInput(Ogre::RenderWindow *window, OIS::MouseListener* mLis, OIS::KeyListener* kLis) {
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    window->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem( pl );

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

    mMouse->setEventCallback(mLis);
    mKeyboard->setEventCallback(kLis);

	unsigned int width, height, depth;
    int left, top;
    window->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;

	mLogicQueue = NULL;
	mRenderQueue = NULL;
}

void CsInputManager::DestroyInput() {
	if( mInputManager )
	{
		//Ogre::LogManager::getSingletonPtr()->logMessage("*** Destroying Input ***");
		mInputManager->destroyInputObject( mMouse );
		mInputManager->destroyInputObject( mKeyboard );

		OIS::InputManager::destroyInputSystem(mInputManager);
		mInputManager = 0;
		//Ogre::LogManager::getSingletonPtr()->logMessage("*** Input Destroyed ***");
	}
}

bool CsInputManager::keyPressed( const OIS::KeyEvent &arg ) {
	static CsLogicMessage logicMessage;
	static CsRenderMessage renderMessage;
	static CsInputEvent evt;

	// feed the GUI first, assuming GUI has already been initialized
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectKeyDown(arg.key);
	sys.injectChar(arg.text);

	evt.type = Chaos::BUTTON_DOWN;
	evt.info.key = arg.key;

	switch (arg.key) {
	case OIS::KC_W:
		evt.info.buttonInfo.buttonId = Chaos::UP;
		break;
	case OIS::KC_S:
		evt.info.buttonInfo.buttonId = Chaos::DOWN;
		break;
	case OIS::KC_A:
		evt.info.buttonInfo.buttonId = Chaos::LEFT;
		break;
	case OIS::KC_D:
		evt.info.buttonInfo.buttonId = Chaos::RIGHT;
		break;
	case OIS::KC_J:
		evt.info.buttonInfo.buttonId = Chaos::FORWARD;
		break;
	case OIS::KC_K:
		evt.info.buttonInfo.buttonId = Chaos::BACKWARD;
		break;
	case OIS::KC_I:
		evt.info.buttonInfo.buttonId = Chaos::FIRE1;
		break;
	case OIS::KC_O:
		evt.info.buttonInfo.buttonId = Chaos::FIRE2;
		break;
	case OIS::KC_SPACE:
		evt.info.buttonInfo.buttonId = Chaos::LOCK_TARGET;
		break;
	case OIS::KC_TAB:
		evt.info.buttonInfo.buttonId = Chaos::SHOW_OBJECTIVES;
		break;
	case OIS::KC_BACKSLASH:
		evt.info.buttonInfo.buttonId = Chaos::DEBUG2;
		break;
	case OIS::KC_Q:
		//evt.info.buttonInfo.buttonId = Chaos::ESCAPE;
		break;
	case OIS::KC_UP:
		evt.info.buttonInfo.buttonId = Chaos::ARROW_UP;
		break;
	case OIS::KC_DOWN:
		evt.info.buttonInfo.buttonId = Chaos::ARROW_DOWN;
		break;
	case OIS::KC_LEFT:
		evt.info.buttonInfo.buttonId = Chaos::ARROW_LEFT;
		break;
	case OIS::KC_RIGHT:
		evt.info.buttonInfo.buttonId = Chaos::ARROW_RIGHT;
		break;

	default: 
		return true;
		break;
	}
	// TODO: try lock in input thread

	// lock Input queue (Logic)
	if (mLogicQueue && !mDisableInputLogic) {
		//mLogicQueue->mMutex.lock();
		logicMessage.type = CsLogicMessage::INPUT_EVENT;
		logicMessage.info.input = evt;
		mLogicQueue->push(logicMessage);
		//mLogicQueue->mMutex.unlock();
	}


	// lock Input queue (Render)
	if (mRenderQueue) {
		//mRenderQueue->mMutex.lock();
		renderMessage.type = CsRenderMessage::INPUT_EVENT;
		renderMessage.info.input = evt;
		mRenderQueue->push(renderMessage);
		//mRenderQueue->mMutex.unlock();
	}

	mButtonState[evt.info.buttonInfo.buttonId] = true;
	return true;
}

bool CsInputManager::keyReleased( const OIS::KeyEvent &arg ) {
	static CsLogicMessage logicMessage;
	static CsRenderMessage renderMessage;
	static CsInputEvent evt;

	// feed the GUI
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectKeyUp(arg.key);

	evt.type = Chaos::BUTTON_UP;
	evt.info.key = arg.key;

	switch (arg.key) {
	case OIS::KC_W:
		evt.info.buttonInfo.buttonId = Chaos::UP;
		break;
	case OIS::KC_S:
		evt.info.buttonInfo.buttonId = Chaos::DOWN;
		break;
	case OIS::KC_A:
		evt.info.buttonInfo.buttonId = Chaos::LEFT;
		break;
	case OIS::KC_D:
		evt.info.buttonInfo.buttonId = Chaos::RIGHT;
		break;
	case OIS::KC_J:
		evt.info.buttonInfo.buttonId = Chaos::FORWARD;
		break;
	case OIS::KC_K:
		evt.info.buttonInfo.buttonId = Chaos::BACKWARD;
		break;
	case OIS::KC_I:
		evt.info.buttonInfo.buttonId = Chaos::FIRE1;
		break;
	case OIS::KC_O:
		evt.info.buttonInfo.buttonId = Chaos::FIRE2;
		break;
	case OIS::KC_SPACE:
		evt.info.buttonInfo.buttonId = Chaos::LOCK_TARGET;
		break;
	case OIS::KC_TAB:
		evt.info.buttonInfo.buttonId = Chaos::SHOW_OBJECTIVES;
		break;
	case OIS::KC_BACKSLASH:
		evt.info.buttonInfo.buttonId = Chaos::DEBUG2;
		break;
	//case OIS::KC_Q:
		evt.info.buttonInfo.buttonId = Chaos::ESCAPE;
		break;
	case OIS::KC_UP:
		evt.info.buttonInfo.buttonId = Chaos::ARROW_UP;
		break;
	case OIS::KC_DOWN:
		evt.info.buttonInfo.buttonId = Chaos::ARROW_DOWN;
		break;
	case OIS::KC_LEFT:
		evt.info.buttonInfo.buttonId = Chaos::ARROW_LEFT;
		break;
	case OIS::KC_RIGHT:
		evt.info.buttonInfo.buttonId = Chaos::ARROW_RIGHT;
		break;
	default: 
		return true;
		break;
	}

	// NOTE: locking not required here in new implementation
	if (mLogicQueue && !mDisableInputLogic) {
		logicMessage.type = CsLogicMessage::INPUT_EVENT;
		logicMessage.info.input = evt;
		mLogicQueue->push(logicMessage);
	}

	if (mRenderQueue) {
		renderMessage.type = CsRenderMessage::INPUT_EVENT;
		renderMessage.info.input = evt;
		mRenderQueue->push(renderMessage);
	}

	mButtonState[evt.info.buttonInfo.buttonId] = false;
	return true;
}

// to convert mouse converts from OIS to CEGUI
CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
    switch (buttonID)
    {
    case OIS::MB_Left:
        return CEGUI::LeftButton;
 
    case OIS::MB_Right:
        return CEGUI::RightButton;
 
    case OIS::MB_Middle:
        return CEGUI::MiddleButton;
 
    default:
        return CEGUI::LeftButton;
    }
}

// OIS::MouseListener
bool CsInputManager::mouseMoved( const OIS::MouseEvent &arg ) {
	static CsLogicMessage logicMessage;
	static CsRenderMessage renderMessage;

	static CsInputEvent evtx;
	static CsInputEvent evty;
	static CsInputEvent evtz;

	evtx.type = Chaos::AXIS_MOVED;
	evtx.info.axisInfo.axisId = Chaos::AXIS_X;
	evtx.info.axisInfo.axisValue = arg.state.X.rel;

	evty.type = Chaos::AXIS_MOVED;
	evty.info.axisInfo.axisId = Chaos::AXIS_Y;
	evty.info.axisInfo.axisValue = arg.state.Y.rel;

	evtz.type = Chaos::AXIS_MOVED;
	evtz.info.axisInfo.axisId = Chaos::AXIS_Z;
	evtz.info.axisInfo.axisValue = arg.state.Z.rel;

	if (mLogicQueue && !mDisableInputLogic) {
		if (evtx.info.axisInfo.axisValue) {
			logicMessage.info.input = evtx;
			mLogicQueue->push(logicMessage);
		}
		if (evty.info.axisInfo.axisValue) {
			logicMessage.info.input = evty;
			mLogicQueue->push(logicMessage);
		}
		if (evtz.info.axisInfo.axisValue) {
			logicMessage.info.input = evtz;
			mLogicQueue->push(logicMessage);
		}
	}
	
	if (mRenderQueue) {
		if (evtx.info.axisInfo.axisValue) {
			renderMessage.info.input = evtx;
			mRenderQueue->push(renderMessage);
		}
		if (evty.info.axisInfo.axisValue) {
			renderMessage.info.input = evty;
			mRenderQueue->push(renderMessage);
		}
		if (evtz.info.axisInfo.axisValue) {
			renderMessage.info.input = evtz;
			mRenderQueue->push(renderMessage);
		}
	}
	
	// feed the GUI
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
	// Scroll wheel.
	if (arg.state.Z.rel)
		sys.injectMouseWheelChange(arg.state.Z.rel / 120.0f);

	return true;
}
bool CsInputManager::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
	static CsLogicMessage logicMessage;
	static CsRenderMessage renderMessage;
	static CsInputEvent evt;

	evt.type = Chaos::BUTTON_DOWN;

	if (id == OIS::MB_Left) evt.info.buttonInfo.buttonId = Chaos::FIRE1;
	else if (id == OIS::MB_Right) evt.info.buttonInfo.buttonId = Chaos::FIRE2;
	else return true;

	if (mLogicQueue && !mDisableInputLogic) {
		logicMessage.type = CsLogicMessage::INPUT_EVENT;
		logicMessage.info.input = evt;
		mLogicQueue->push(logicMessage);
	}

	if (mRenderQueue) {
		renderMessage.type = CsRenderMessage::INPUT_EVENT;
		renderMessage.info.input = evt;
		mRenderQueue->push(renderMessage);
	}

	CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));

	mButtonState[evt.info.buttonInfo.buttonId] = true;
	return true;
}

bool CsInputManager::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
	static CsLogicMessage logicMessage;
	static CsRenderMessage renderMessage;
	static CsInputEvent evt;

	evt.type = Chaos::BUTTON_UP;

	if (id == OIS::MB_Left) evt.info.buttonInfo.buttonId = Chaos::FIRE1;
	else if (id == OIS::MB_Right) evt.info.buttonInfo.buttonId = Chaos::FIRE2;
	else return true;

	if (mLogicQueue && !mDisableInputLogic) {
		logicMessage.type = CsLogicMessage::INPUT_EVENT;
		logicMessage.info.input = evt;
		mLogicQueue->push(logicMessage);
	}

	if (mRenderQueue) {
		renderMessage.type = CsRenderMessage::INPUT_EVENT;
		renderMessage.info.input = evt;
		mRenderQueue->push(renderMessage);
	}

	CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));

	mButtonState[evt.info.buttonInfo.buttonId] = false;
	return true;
}