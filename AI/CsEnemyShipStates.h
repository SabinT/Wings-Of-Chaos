#pragma once

#include <BulletCommons.h>
#include <CsState.h>
#include <ctime>


class CsIdle :public CsState
{  
public:
	virtual ~CsIdle(void)   {}
	static CsIdle* Instance();

	//the meat of the state executions
	virtual void Enter (CsShipObject* villian);
	virtual void Execute (CsShipObject* villian);
	virtual void Exit (CsShipObject* villian);


private:
  CsIdle()  {}
  
  //copy ctor and assignment should be private
  CsIdle(const CsIdle&);
  CsIdle& operator=(const CsIdle&);
};

class CsDefendBase :public CsState
{  
public:
	virtual ~CsDefendBase(void)   {}
	static CsDefendBase* Instance();

	virtual void Enter (CsShipObject* villian);
	virtual void Execute (CsShipObject* villian);
	virtual void Exit (CsShipObject* villian);


private:

  CsDefendBase()  {}
  
  //copy ctor and assignment should be private
  CsDefendBase(const CsDefendBase&);
  CsDefendBase& operator=(const CsDefendBase&);
};


class CsChase :public CsState
{  
public:
	virtual ~CsChase(void)  {}
	static CsChase* Instance();

	//the meat of the state executions
	virtual void Enter (CsShipObject* villian);
	virtual void Execute (CsShipObject* villian);
	virtual void Exit (CsShipObject* villian);


private:
  
  CsChase()  {}
  
  //copy ctor and assignment should be private
  CsChase(const CsChase&);
  CsChase& operator=(const CsChase&);
};


class CsShoot :public CsState
{  
public:
	virtual ~CsShoot(void)  {}
	static CsShoot* Instance();

	//the meat of the state executions
	virtual void Enter (CsShipObject* villian);
	virtual void Execute (CsShipObject* villian);
	virtual void Exit (CsShipObject* villian);


private:
  
  CsShoot()  {}
  
  //copy ctor and assignment should be private
  CsShoot(const CsShoot&);
  CsShoot& operator=(const CsShoot&);
};

class CsRetreat :public CsState
{  
public:
	virtual ~CsRetreat(void)  {}
	static CsRetreat* Instance();

	//the meat of the state executions
	virtual void Enter (CsShipObject* villian);
	virtual void Execute (CsShipObject* villian);
	virtual void Exit (CsShipObject* villian);


private:
  
  CsRetreat()  {}
  
  //copy ctor and assignment should be private
  CsRetreat(const CsRetreat&);
  CsRetreat& operator=(const CsRetreat&);
};



class CsStunned :public CsState
{  
public:
	virtual ~CsStunned(void)  {}
	static CsStunned* Instance();

	//the meat of the state executions
	virtual void Enter (CsShipObject* villian);
	virtual void Execute (CsShipObject* villian);
	virtual void Exit (CsShipObject* villian);


private:
  
  CsStunned()  {}
  
  //copy ctor and assignment should be private
  CsStunned(const CsStunned&);
  CsStunned& operator=(const CsStunned&);
};



