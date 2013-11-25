#pragma once

#include <OgrePrerequisites.h>

#include <CsLockable.h>
#include <LinearMath\btIDebugDraw.h>

#include <vector>

class CsDebugDraw :
	public btIDebugDraw, public CsLockable
{
public:
	CsDebugDraw(void);
	virtual ~CsDebugDraw(void);
	inline void SetDebugLines(std::vector<Ogre::Vector3>* debugPoints, std::vector<Ogre::ColourValue>* debugColors) 
	{ mDebugColors = debugColors; mDebugPoints = debugPoints; }

    virtual void	setDebugMode(int mode) {mDebugMode = mode;}
    virtual int		getDebugMode() const { return mDebugMode;}

    virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
    virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,
        btScalar distance,int lifeTime,const btVector3& color)
	{ return;}

	void	draw3dText(const btVector3& location,const char* textString)
	{ return; }

	void	reportErrorWarning(const char* warningString)
	{ 
		// cout << error;
	}

protected:
	std::vector<Ogre::Vector3>* mDebugPoints;
	std::vector<Ogre::ColourValue>* mDebugColors;
    
	int mDebugMode;
};