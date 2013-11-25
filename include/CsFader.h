#ifndef CsFader_H
#define CsFader_H

#include <OgrePrerequisites.h>
#include <CsCommons.h>

class CsFader {
public:
	enum Operation {FADEIN, FADEOUT, NONE};

	CsFader();
	
	void SetMaterial(Ogre::Material* mat);
	void SetMaterial(const char* materialName);
	void SetAlpha(const char* materialName, float alpha);
	void SetAlpha(Ogre::TextureUnitState * tex_state, float alpha);
	void SetOperation(Operation op, float startAlpha = 0.5, float duration = 1.0, float wait = 1.0);
	
	// step fade, returns false if operation complete
	bool Fade(float deltaTime);
private:
	Operation mOperation;

	Ogre::Material *mMaterial;
	double mAlpha;

	double _alpha;
     double _current_dur;
     double _total_dur;
	 float wait;

     Ogre::TextureUnitState *_tex_unit;
};

#endif