#pragma once

#include <CsCommons.h>
#include <vector>

struct CsObjective {
	enum ObjectiveType {
		DESTROY, PROTECT, TIME_LIMIT
	};
	enum Status {
		FAIL, SUCCESS, RUNNING
	};

	ObjectiveType type;
	Status status;
	std::string description;
	// use a union maybe
	union {
		CsDestructibleObject* object;
		float timeLimit;
	};
	CsObjective() : object(0), status(RUNNING) { }
	CsObjective(ObjectiveType _type, CsDestructibleObject* _object) {
		type = _type;
		object = _object;
		status = RUNNING;
	}

	void Update();
};

class CsObjectiveList : public std::vector<CsObjective>
{
public:
	CsObjectiveList(void);
	virtual ~CsObjectiveList(void);

	void Add(CsObjective &obj);
	// this function called from logic part
	void Update();

	// this class also handles the showing/flashing etc of objectives
	void FlashObjectives();
	void TourObjectives();

	btVector3 mEnemyBasePos;
	CsDestructibleObject *mEnemyBase;
};

