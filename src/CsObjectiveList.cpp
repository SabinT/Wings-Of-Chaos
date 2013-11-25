#include "precompiled.h"
#include <CsObjectiveList.h>
#include <CsDestructibleObject.h>
#include <CsGame.h>
#include <CsCameraManager.h>
#include <CsShipObject.h>

#include <OgreBulletConverter.h>

const float OBJECTIVE_FLASH_TIME = 4.0;

void CsObjective::Update() {
	if (type == DESTROY) 
	{
		if (object->GetHP() < 0) status = SUCCESS;
	} 
	else if (type == PROTECT)	
	{
		if (object->GetHP() < 0) status = FAIL;
	}
	else if (type == TIME_LIMIT)	
	{
		if (CsGame::GetSingletonPtr()->GetGameTime() > timeLimit) status = FAIL;
	}
}


/**********************************************************************************/

CsObjectiveList::CsObjectiveList(void)
{
	mEnemyBase = NULL;
}


CsObjectiveList::~CsObjectiveList(void)
{
}

void CsObjectiveList::Update() {
	std::vector<CsObjective>::iterator it = begin();

	bool allSuccess = true;
	bool failure = false;
	while (it != end()) {
		(*it).Update();
		if ((*it).status != CsObjective::SUCCESS) {
			if ((*it).type != CsObjective::PROTECT) allSuccess = false;
			if ((*it).status == CsObjective::FAIL) failure = true;
		}
		it++;
	}

	if (allSuccess) {
		CsLogicMessage msg;
		CsRenderMessage rmsg;
		rmsg.type = CsLogicMessage::GAME_EVENT;
		msg.type = CsLogicMessage::GAME_EVENT;
		rmsg.info.gameEvent.type = CsGameEvent::MISSION_COMPLETE;
		msg.info.gameEvent.type = CsGameEvent::MISSION_COMPLETE;
		CsGame::GetSingletonPtr()->GetLogicQueue()->push(msg);
		CsGame::GetSingletonPtr()->GetRenderTempQueue()->push(rmsg);
		// send a message
	}
	if (failure) {
		// send a message
		CsLogicMessage msg;
		CsRenderMessage rmsg;
		msg.type = CsLogicMessage::GAME_EVENT;
		rmsg.type = CsLogicMessage::GAME_EVENT;
		msg.info.gameEvent.type = CsGameEvent::MISSION_FAIL;
		rmsg.info.gameEvent.type = CsGameEvent::MISSION_FAIL;
		CsGame::GetSingletonPtr()->GetLogicQueue()->push(msg);
		CsGame::GetSingletonPtr()->GetRenderTempQueue()->push(rmsg);
	}
}

void CsObjectiveList::Add(CsObjective &obj) {
	// return. NOTE: short circuit
	if (obj.type == CsObjective::DESTROY) {
		mEnemyBasePos = obj.object->GetRigidBody()->getWorldTransform().getOrigin();
		mEnemyBase = obj.object;
		push_back(obj);
	}
}

void CsObjectiveList::TourObjectives() {
	static int current = 0; // 0 is nothing, 1 =  1st , n = nth
	static float time = -OBJECTIVE_FLASH_TIME;

	//if (CsGame::GetSingletonPtr()->mGameState != CsGame::PRE_GAME) return;

	if (time + OBJECTIVE_FLASH_TIME < CsGame::GetSingletonPtr()->GetGameTime()) {
		if (current >= size()) {
			// time to stop tour
			CsRenderMessage msg;
			msg.type = CsRenderMessage::INPUT_EVENT;
			msg.info.input.type = Chaos::BUTTON_DOWN;
			msg.info.input.info.buttonInfo.buttonId = Chaos::DEBUG2;
			CsGame::GetSingletonPtr()->GetRenderQueue()->push(msg);
			// start game message

			return;
		}

		CsObjective &objective = this->at(current);
		CsGame::GetSingletonPtr()->GetGameGUI()->ShowMessage(std::string("Objectives"), objective.description);
		CsGame::GetSingletonPtr()->GetCameraManager()->SetTarget(objective.object->GetNode());

		current++;
		time = CsGame::GetSingletonPtr()->GetGameTime();
	}

	//if (time > 10.0)
	//	CsGame::GetSingletonPtr()->mGameState = CsGame::RUNNING;
}

void CsObjectiveList::FlashObjectives() {
	std::string text;
	for (int i = 0; i < size(); i++) {
		text += this->at(i).description;
	}
	
	CsGame::GetSingletonPtr()->GetGameGUI()->ShowMessage(std::string("Objectives"), text);
}