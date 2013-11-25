#include <precompiled.h>

#include <CsGameInterface.h>

#include <CsInputManager.h>

CsGameInterface::~CsGameInterface()
{
    //dtor
}

void CsGameInterface::InjectShutdown() {
	boost::mutex::scoped_lock lock(mMutex);
	mShutdown = true;
}