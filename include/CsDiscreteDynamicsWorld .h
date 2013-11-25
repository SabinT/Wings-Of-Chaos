#pragma once

#include <BulletDynamics\Dynamics\btDiscreteDynamicsWorld.h>

//collision detection
#include "BulletCollision/CollisionDispatch/btCollisionDispatcher.h"
#include "BulletCollision/BroadphaseCollision/btSimpleBroadphase.h"
#include "BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletCollision/CollisionDispatch/btSimulationIslandManager.h"
#include "LinearMath/btTransformUtil.h"
#include "LinearMath/btQuickprof.h"

#include <BulletDynamics\Dynamics\btRigidBody.h>

#include <boost\thread\mutex.hpp>

// TODO: create .cpp file

class CsDiscreteDynamicsWorld : public btDiscreteDynamicsWorld {
public:
	CsDiscreteDynamicsWorld (btDispatcher* dispatcher,btBroadphaseInterface* pairCache,btConstraintSolver* constraintSolver,btCollisionConfiguration* collisionConfiguration)
	: btDiscreteDynamicsWorld(dispatcher, pairCache, constraintSolver, collisionConfiguration)
	{ }

	virtual ~CsDiscreteDynamicsWorld() 
	{ }

	void SetSyncMutex (boost::mutex *syncMutex) {
		mSyncMutex = syncMutex;
	}

protected:
	void synchronizeMotionStates()
	{
		boost::mutex::scoped_lock lock(*mSyncMutex);

		BT_PROFILE("synchronizeMotionStates");
		if (m_synchronizeAllMotionStates)
		{
			//iterate  over all collision objects
			for ( int i=0;i<m_collisionObjects.size();i++)
			{
				btCollisionObject* colObj = m_collisionObjects[i];
				btRigidBody* body = btRigidBody::upcast(colObj);
				if (body)
					synchronizeSingleMotionState(body);
			}
		} else
		{
			//iterate over all active rigid bodies
			for ( int i=0;i<m_nonStaticRigidBodies.size();i++)
			{
				btRigidBody* body = m_nonStaticRigidBodies[i];
				if (body->isActive())
					synchronizeSingleMotionState(body);
			}
		}
	}


	// members
	boost::mutex *mSyncMutex;
};