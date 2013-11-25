#include <precompiled.h>

#include <CsFader.h>
#include <OgreMaterialManager.h>

CsFader::CsFader() {
	mMaterial = NULL;
	mAlpha = 0.5;
	_tex_unit = NULL;
	mOperation = Operation::NONE;
}

void CsFader::SetMaterial(const char* materialName) {
	Ogre::ResourcePtr resptr = Ogre::MaterialManager::getSingleton().getByName(materialName);
    Ogre::Material * mat = dynamic_cast<Ogre::Material*>(resptr.getPointer());
	SetMaterial(mat);
}


void CsFader::SetMaterial(Ogre::Material *mat) {
	mMaterial = mat;
	if (!mat) _tex_unit = NULL;

	Ogre::Technique *tech = mat->getTechnique(0);    // Get the technique
    Ogre::Pass *pass = tech->getPass(0);            // Get the pass
    _tex_unit = pass->getTextureUnitState(0);        // Get the texture_unit state
}

void CsFader::SetAlpha(const char* materialName, float alpha) {
	Ogre::ResourcePtr resptr = Ogre::MaterialManager::getSingleton().getByName(materialName);
    Ogre::Material * mat = dynamic_cast<Ogre::Material*>(resptr.getPointer());

	Ogre::Technique *tech = mat->getTechnique(0);    // Get the technique
    Ogre::Pass *pass = tech->getPass(0);            // Get the pass
	Ogre::TextureUnitState* tex_unit = pass->getTextureUnitState(0);        // Get the texture_unit state
	SetAlpha(tex_unit, alpha);
}

void CsFader::SetAlpha(Ogre::TextureUnitState* tex_state, float alpha) {
	tex_state->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, alpha);    // Change the _alpha operation
}

bool CsFader::Fade(float deltaTime) {
	if (mOperation == Operation::NONE) return false;

	if(!_tex_unit) return false;

    // Set the _alpha value of the _overlay
    if (_alpha > 1.0) SetAlpha(_tex_unit,1.0);
	else if (_alpha < 0.0) SetAlpha(_tex_unit, 0.0);
	else SetAlpha(_tex_unit, _alpha);
 
		 if(mOperation == Operation::FADEIN)
         {
             _alpha += deltaTime / _total_dur;
             if( _alpha > 1.0 + wait )
             {
				 mOperation = Operation::NONE;
			 }
         }
 
         // If fading out, increase the _alpha until it reaches 1.0
		 else if(mOperation == Operation::FADEOUT)
         {
             _alpha -= deltaTime / _total_dur;
             if( _alpha < -0.0 - wait )
             {
				 mOperation = Operation::NONE;
             }
         }

		 return true;
}



void CsFader::SetOperation(CsFader::Operation op, float startAlpha, float duration, float waitt) {
	if( duration < 0 )
         duration = -duration;
     if( duration < 0.000001 )
         duration = 1.0;
 
     _alpha = startAlpha;
     _total_dur = duration;
     _current_dur = duration;
     mOperation = op;
	 wait = waitt;
}

