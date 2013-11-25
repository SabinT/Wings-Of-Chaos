#ifndef CSWORLDOBJECT_H
#define CSWORLDOBJECT_H

#include <OgrePrerequisites.h>

#include <CsObject.h>

/// objects in the visual world

class CsWorldObject : public CsObject
{
public:
    CsWorldObject() {
		CsObject();
		mNode = NULL; 
		mIsVisible = true;
		mNeedsUpdate = true;
	}
    CsWorldObject (Ogre::SceneNode* node);

    void SetNode (Ogre::SceneNode* node);
    inline Ogre::SceneNode* GetNode () { return mNode;}

	inline void SetVisible(bool isVisible) { mIsVisible = isVisible; }
	inline bool GetVisible() { return mIsVisible; }

	inline void SetNeedsUpdate (bool needsUpdate) { mNeedsUpdate = needsUpdate; }
	inline bool GetNeedsUpdate () { return mNeedsUpdate; }

    // this function should be called each frame
	virtual void UpdateSceneNode();

protected:

    // Ogre::Entity* mEntity;

    Ogre::SceneNode *mNode;
	bool mNeedsUpdate;

	bool mIsVisible;
    /// @todo similarly for physics
    // mIsSimulated
    // misVisible, show(), hide(),
};

///@todo class CsPhysicsWorldObject

#endif // CSWORLDOBJECT_H
