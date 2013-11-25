#pragma once

#include <cassert>
#include <string>

#include <CsState.h>




class CsStateMachine
{
private:

  //a pointer to the agent that owns this instance
  CsShipObject*          mShipOwner;

  CsState*   mCurrentState;
  
  //a record of the last state the agent was in
  CsState*   mPreviousState;

  //this is called every time the FSM is updated
  CsState*   mGlobalState;
  

public:

  CsStateMachine(CsShipObject* owner):mShipOwner(owner),
                                   mCurrentState(NULL),
                                   mPreviousState(NULL),
                                   mGlobalState(NULL)
  {}

  virtual ~CsStateMachine(){}

  //use these methods to initialize the FSM
  void SetCurrentState(CsState* s){mCurrentState = s;}
  void SetGlobalState(CsState* s) {mGlobalState = s;}
  void SetPreviousState(CsState* s){mPreviousState = s;}
  
  //call this to update the FSM
  void  Update()const
  {
    //if a global state exists, call its execute method, else do nothing
    if(mGlobalState)   mGlobalState->Execute(mShipOwner);

    //same for the current state
    if (mCurrentState) mCurrentState->Execute(mShipOwner);
  }


  //change to a new state
  void  ChangeState(CsState* pNewState)
  {
    assert(pNewState && "<StateMachine::ChangeState>:trying to assign null state to current");

    //keep a record of the previous state
    mPreviousState = mCurrentState;

    //call the exit method of the existing state
    mCurrentState->Exit(mShipOwner);

    //change state to the new state
    mCurrentState = pNewState;

    //call the entry method of the new state
    mCurrentState->Enter(mShipOwner);
  }

  //change state back to the previous state
  void  RevertToPreviousState()
  {
    ChangeState(mPreviousState);
  }

 
};







