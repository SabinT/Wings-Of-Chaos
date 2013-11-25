#pragma once

class CsShipObject;

class CsState
{

public:

  virtual ~CsState(){}

  //this will execute when the state is entered
  virtual void Enter(CsShipObject*)=0;

  //this is the states normal update function
  virtual void Execute(CsShipObject*)=0;

  //this will execute when the state is exited. 
  virtual void Exit(CsShipObject*)=0;

  
};

