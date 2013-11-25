#pragma once

#include <boost\thread\mutex.hpp>

class CsLockable {
public:
	boost::mutex mMutex;
	CsLockable () {}
	virtual ~CsLockable() {}
};