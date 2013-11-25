/*
 * OgreMax Sample Viewer and Scene Loader - Ogre3D-based viewer and code for loading and displaying .scene files
 * Copyright 2010 AND Entertainment
 *
 * This code is available under the OgreMax Free License:
 *   -You may use this code for any purpose, commercial or non-commercial.
 *   -If distributing derived works (that use this source code) in binary or source code form, 
 *    you must give the following credit in your work's end-user documentation: 
 *        "Portions of this work provided by OgreMax (www.ogremax.com)"
 *
 * AND Entertainment assumes no responsibility for any harm caused by using this code.
 * 
 * The OgreMax Sample Viewer and Scene Loader were released at www.ogremax.com 
 */


//Includes---------------------------------------------------------------------
#ifndef OGREMAX_VIEWER_NO_LUA

#include "ApplicationLuaTypes.hpp"
#include "InputSystem.hpp"

#include <OgreMath.h>
#include <OgreVector2.h>
#include <OgreVector3.h>
#include <OgrePlane.h>
#include <OgreMatrix3.h>
#include <OgreMatrix4.h>
#include <OgreQuaternion.h>
#include <OgreAxisAlignedBox.h>
#include <OgreSphere.h>
#include <OgreRay.h>
#include <OgreAnimationState.h>
#include <OgreNode.h>
#include <OgreSceneNode.h>
#include <OgreCamera.h>

#include <luabind/luabind.hpp>
#include <luabind/class.hpp>
#include <luabind/function.hpp>
#include <luabind/object.hpp>
#include <luabind/operator.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <luabind/out_value_policy.hpp>

using namespace Ogre;
using namespace OgreMax;
using namespace luabind;


//Local classes----------------------------------------------------------------
class ScriptInputSystem
{
public:
    Keyboard* GetKeyboard() const
    {
        return &InputSystem::GetInstance().Keyboard;
    }

    Mouse* GetMouse() const
    {
        return &InputSystem::GetInstance().Mouse;
    }

    Joystick* GetJoystick() const
    {
        return &InputSystem::GetInstance().GetJoystick(0);
    }

    static ScriptInputSystem& GetInstance()
    {
        static ScriptInputSystem instance;
        return instance;
    }
};


//Local functions--------------------------------------------------------------
/**
 * Template function that returns a constant value
 */
template <class T, const T& value>
T ValueFunction()
{
    return value;
}

static Vector3 GetHSB(const ColourValue& color)
{
    Real h, s, b;
    color.getHSB(&h, &s, &b);
    return Vector3(h, s, b);
}



//Implementation---------------------------------------------------------------
void ApplicationLuaTypes::Register(LuaScript& script, const Ogre::String& moduleName)
{
    class ScriptKeys
    {
    };

    class ScriptTransformSpace
    {
    };

    class ScriptProjectionType
    {
    };

    //Initialize script types
    module(script, moduleName.c_str())
	[
        class_<Keyboard>("Keyboard")
            .def("PressedKey", &Keyboard::PressedKey)
            .def("PressingKey", &Keyboard::PressingKey)
            .def("ReleasedKey", &Keyboard::ReleasedKey),

        class_<ScriptKeys>("Keys")
            .enum_("constants")
            [
                value("ESCAPE", OIS::KC_ESCAPE),
		        value("_1", OIS::KC_1),
		        value("_2", OIS::KC_2),
		        value("_3", OIS::KC_3),
		        value("_4", OIS::KC_4),
		        value("_5", OIS::KC_5),
		        value("_6", OIS::KC_6),
		        value("_7", OIS::KC_7),
		        value("_8", OIS::KC_8),
		        value("_9", OIS::KC_9),
		        value("_0", OIS::KC_0),
		        value("MINUS", OIS::KC_MINUS),
		        value("EQUALS", OIS::KC_EQUALS),
		        value("BACK", OIS::KC_BACK),
		        value("TAB", OIS::KC_TAB),
		        value("Q", OIS::KC_Q),
		        value("W", OIS::KC_W),
		        value("E", OIS::KC_E),
		        value("R", OIS::KC_R),
		        value("T", OIS::KC_T),
		        value("Y", OIS::KC_Y),
		        value("U", OIS::KC_U),
		        value("I", OIS::KC_I),
		        value("O", OIS::KC_O),
		        value("P", OIS::KC_P),
		        value("LBRACKET", OIS::KC_LBRACKET),
		        value("RBRACKET", OIS::KC_RBRACKET),
		        value("RETURN", OIS::KC_RETURN),
		        value("LCONTROL", OIS::KC_LCONTROL),
		        value("A", OIS::KC_A),
		        value("S", OIS::KC_S),
		        value("D", OIS::KC_D),
		        value("F", OIS::KC_F),
		        value("G", OIS::KC_G),
		        value("H", OIS::KC_H),
		        value("J", OIS::KC_J),
		        value("K", OIS::KC_K),
		        value("L", OIS::KC_L),
		        value("SEMICOLON", OIS::KC_SEMICOLON),
		        value("APOSTROPHE", OIS::KC_APOSTROPHE),
		        value("GRAVE", OIS::KC_GRAVE),
		        value("LSHIFT", OIS::KC_LSHIFT),
		        value("BACKSLASH", OIS::KC_BACKSLASH),
		        value("Z", OIS::KC_Z),
		        value("X", OIS::KC_X),
		        value("C", OIS::KC_C),
		        value("V", OIS::KC_V),
		        value("B", OIS::KC_B),
		        value("N", OIS::KC_N),
		        value("M", OIS::KC_M),
		        value("COMMA", OIS::KC_COMMA),
		        value("PERIOD", OIS::KC_PERIOD),
		        value("SLASH", OIS::KC_SLASH),
		        value("RSHIFT", OIS::KC_RSHIFT),
		        value("MULTIPLY", OIS::KC_MULTIPLY),
		        value("LMENU", OIS::KC_LMENU),
		        value("SPACE", OIS::KC_SPACE),
		        value("CAPITAL", OIS::KC_CAPITAL),
		        value("F1", OIS::KC_F1),
		        value("F2", OIS::KC_F2),
		        value("F3", OIS::KC_F3),
		        value("F4", OIS::KC_F4),
		        value("F5", OIS::KC_F5),
		        value("F6", OIS::KC_F6),
		        value("F7", OIS::KC_F7),
		        value("F8", OIS::KC_F8),
		        value("F9", OIS::KC_F9),
		        value("F10", OIS::KC_F10),
		        value("NUMLOCK", OIS::KC_NUMLOCK),
		        value("SCROLL", OIS::KC_SCROLL),
		        value("NUMPAD7", OIS::KC_NUMPAD7),
		        value("NUMPAD8", OIS::KC_NUMPAD8),
		        value("NUMPAD9", OIS::KC_NUMPAD9),
		        value("SUBTRACT", OIS::KC_SUBTRACT),
		        value("NUMPAD4", OIS::KC_NUMPAD4),
		        value("NUMPAD5", OIS::KC_NUMPAD5),
		        value("NUMPAD6", OIS::KC_NUMPAD6),
		        value("ADD", OIS::KC_ADD),
		        value("NUMPAD1", OIS::KC_NUMPAD1),
		        value("NUMPAD2", OIS::KC_NUMPAD2),
		        value("NUMPAD3", OIS::KC_NUMPAD3),
		        value("NUMPAD0", OIS::KC_NUMPAD0),
		        value("DECIMAL", OIS::KC_DECIMAL),
		        value("OEM_102", OIS::KC_OEM_102),
		        value("F11", OIS::KC_F11),
		        value("F12", OIS::KC_F12),
		        value("F13", OIS::KC_F13),
		        value("F14", OIS::KC_F14),
		        value("F15", OIS::KC_F15),
		        value("KANA", OIS::KC_KANA),
		        value("ABNT_C1", OIS::KC_ABNT_C1),
		        value("CONVERT", OIS::KC_CONVERT),
		        value("NOCONVERT", OIS::KC_NOCONVERT),
		        value("YEN", OIS::KC_YEN),
		        value("ABNT_C2", OIS::KC_ABNT_C2),
		        value("NUMPADEQUALS", OIS::KC_NUMPADEQUALS),
		        value("PREVTRACK", OIS::KC_PREVTRACK),
		        value("AT", OIS::KC_AT),
		        value("COLON", OIS::KC_COLON),
		        value("UNDERLINE", OIS::KC_UNDERLINE),
		        value("KANJI", OIS::KC_KANJI),
		        value("STOP", OIS::KC_STOP),
		        value("AX", OIS::KC_AX),
		        value("UNLABELED", OIS::KC_UNLABELED),
		        value("NEXTTRACK", OIS::KC_NEXTTRACK),
		        value("NUMPADENTER", OIS::KC_NUMPADENTER),
		        value("RCONTROL", OIS::KC_RCONTROL),
		        value("MUTE", OIS::KC_MUTE),
		        value("CALCULATOR", OIS::KC_CALCULATOR),
		        value("PLAYPAUSE", OIS::KC_PLAYPAUSE),
		        value("MEDIASTOP", OIS::KC_MEDIASTOP),
		        value("VOLUMEDOWN", OIS::KC_VOLUMEDOWN),
		        value("VOLUMEUP", OIS::KC_VOLUMEUP),
		        value("WEBHOME", OIS::KC_WEBHOME),
		        value("NUMPADCOMMA", OIS::KC_NUMPADCOMMA),
		        value("DIVIDE", OIS::KC_DIVIDE),
		        value("SYSRQ", OIS::KC_SYSRQ),
		        value("RMENU", OIS::KC_RMENU),
		        value("PAUSE", OIS::KC_PAUSE),
		        value("HOME", OIS::KC_HOME),
		        value("UP", OIS::KC_UP),
		        value("PGUP", OIS::KC_PGUP),
		        value("LEFT", OIS::KC_LEFT),
		        value("RIGHT", OIS::KC_RIGHT),
		        value("END", OIS::KC_END),
		        value("DOWN", OIS::KC_DOWN),
		        value("PGDOWN", OIS::KC_PGDOWN),
		        value("INSERT", OIS::KC_INSERT),
		        value("DELETE", OIS::KC_DELETE),
		        value("LWIN", OIS::KC_LWIN),
		        value("RWIN", OIS::KC_RWIN),
		        value("APPS", OIS::KC_APPS),
		        value("POWER", OIS::KC_POWER),
		        value("SLEEP", OIS::KC_SLEEP),
		        value("WAKE", OIS::KC_WAKE),
		        value("WEBSEARCH", OIS::KC_WEBSEARCH),
		        value("WEBFAVORITES", OIS::KC_WEBFAVORITES),
		        value("WEBREFRESH", OIS::KC_WEBREFRESH),
		        value("WEBSTOP", OIS::KC_WEBSTOP),
		        value("WEBFORWARD", OIS::KC_WEBFORWARD),
		        value("WEBBACK", OIS::KC_WEBBACK),
		        value("MYCOMPUTER", OIS::KC_MYCOMPUTER),
		        value("MAIL", OIS::KC_MAIL),
		        value("MEDIASELECT", OIS::KC_MEDIASELECT)
            ],

        class_<Mouse>("Mouse")
            .def("PressedButton", &Mouse::PressedButton)
            .def("PressingButton", &Mouse::PressingButton)
            .def("ReleasedButton", &Mouse::ReleasedButton),

        class_<Joystick>("Joystick")
            .def("PressedButton", &Joystick::PressedButton)
            .def("PressingButton", &Joystick::PressingButton)
            .def("ReleasedButton", &Joystick::ReleasedButton),

        class_<ScriptInputSystem>("InputSystem")
            .property("Keyboard", &ScriptInputSystem::GetKeyboard)
            .property("Mouse", &ScriptInputSystem::GetMouse)
            .property("Joystick", &ScriptInputSystem::GetJoystick),

        class_<Radian>("Radian")
            .def(constructor<Real>())
            .def(constructor<const Radian&>())
            .def("GetDegrees", &Radian::valueDegrees)
            .def("GetRadians", &Radian::valueRadians),

        class_<Vector2>("Vector2")
            .def(constructor<>())
            .def(constructor<float, float>())
            .def(constructor<const Vector2&>())
            .def_readwrite("X", &Vector2::x)
            .def_readwrite("Y", &Vector2::y)
            .def(const_self + other<const Vector2&>())
            .def(const_self - other<const Vector2&>())
            .def(const_self / other<const Vector2&>())
            .def(const_self * other<const Vector2&>())
            .def(const_self * float())
            .def(const_self / float())
            .def(float() * const_self)
            .def(float() / const_self)
            .def(const_self + float())
            .def(const_self - float())
            .def(float() + const_self)
            .def(float() - const_self)
            .def(- const_self)
            .def("Length", &Vector2::length)
            .def("SquaredLength", &Vector2::squaredLength)
            .def("Dot", &Vector2::dotProduct)
            .def("Normalize", &Vector2::normalise)
            .def("Cross", &Vector2::crossProduct)
            .def("MidPoint", &Vector2::midPoint)
            .def("Perpendicular", &Vector2::perpendicular)
            .scope
            [
                def("Zero", ValueFunction<Vector2, Vector2::ZERO>),
                def("One", ValueFunction<Vector2, Vector2::UNIT_SCALE>),
                def("UnitX", ValueFunction<Vector2, Vector2::UNIT_X>),
                def("UnitY", ValueFunction<Vector2, Vector2::UNIT_Y>),
                def("NegativeUnitX", ValueFunction<Vector2, Vector2::NEGATIVE_UNIT_X>),
                def("NegativeUnitY", ValueFunction<Vector2, Vector2::NEGATIVE_UNIT_Y>)
            ],

		class_<Vector3>("Vector3")
            .def(constructor<>())
            .def(constructor<float, float, float>())
            .def(constructor<const Vector3&>())
            .def_readwrite("X", &Vector3::x)
            .def_readwrite("Y", &Vector3::y)
            .def_readwrite("Z", &Vector3::z)
            .def(const_self + other<const Vector3&>())
            .def(const_self - other<const Vector3&>())
            .def(const_self / other<const Vector3&>())
            .def(const_self * other<const Vector3&>())
            .def(const_self * float())
            .def(const_self / float())
            .def(float() * const_self)
            .def(float() / const_self)
            .def(const_self + float())
            .def(const_self - float())
            .def(float() + const_self)
            .def(float() - const_self)
            .def(- const_self)
            .def("Length", &Vector3::length)
            .def("SquaredLength", &Vector3::squaredLength)
            .def("Distance", &Vector3::distance)
            .def("SquaredDistance", &Vector3::squaredDistance)
            .def("Dot", &Vector3::dotProduct)
            .def("AbsDot", &Vector3::absDotProduct)
            .def("Normalize", &Vector3::normalise)
            .def("Cross", &Vector3::crossProduct)
            .def("MidPoint", &Vector3::midPoint)
            .def("Perpendicular", &Vector3::perpendicular)
            .scope
            [
                def("Zero", ValueFunction<Vector3, Vector3::ZERO>),
                def("One", ValueFunction<Vector3, Vector3::UNIT_SCALE>),
                def("UnitX", ValueFunction<Vector3, Vector3::UNIT_X>),
                def("UnitY", ValueFunction<Vector3, Vector3::UNIT_Y>),
                def("UnitZ", ValueFunction<Vector3, Vector3::UNIT_Z>),
                def("NegativeUnitX", ValueFunction<Vector3, Vector3::NEGATIVE_UNIT_X>),
                def("NegativeUnitY", ValueFunction<Vector3, Vector3::NEGATIVE_UNIT_Y>),
                def("NegativeUnitZ", ValueFunction<Vector3, Vector3::NEGATIVE_UNIT_Z>)
            ],

        class_<Plane>("Plane")
            .def(constructor<>())
            .def(constructor<const Plane&>())
            .def(constructor<const Vector3&, Real>())
            .def(constructor<const Vector3&, const Vector3&>())
            .def(constructor<const Vector3&, const Vector3&, const Vector3&>())
            .def_readwrite("Normal", &Plane::normal)
            .def_readwrite("D", &Plane::d)
            .def("GetSide", (Plane::Side (Plane::*)(const Vector3&) const)&Plane::getSide)
            .def("GetSide", (Plane::Side (Plane::*)(const Vector3&, const Vector3&) const)&Plane::getSide)
            .def("GetDistance", &Plane::getDistance)
            .def("Redefine", (void (Plane::*)(const Vector3&, const Vector3&))&Plane::redefine)
            .def("Redefine", (void (Plane::*)(const Vector3&, const Vector3&, const Vector3&))&Plane::redefine)
            .def("ProjectVector", &Plane::projectVector)
            .enum_("constants")
            [
                value("NO_SIDE", Plane::NO_SIDE),
                value("POSITIVE_SIDE", Plane::POSITIVE_SIDE),
                value("NEGATIVE_SIDE", Plane::NEGATIVE_SIDE),
                value("BOTH_SIDES", Plane::BOTH_SIDE)
            ],

        class_<Matrix3>("Matrix3")
            .def(constructor<>())
            .def(constructor<const Matrix3&>())
            .def(const_self * other<const Vector3&>())
            .def("Transposed", &Matrix3::Transpose)
            .def("Inverse", (Matrix3 (Matrix3::*)(Real) const)&Matrix3::Inverse)
            .def("GetColumn", &Matrix3::GetColumn)
            .scope
            [
                def("Identity", ValueFunction<Matrix3, Matrix3::IDENTITY>)
            ],

        class_<Matrix4>("Matrix4")
            .def(constructor<>())
            .def(constructor<const Matrix4&>())
            .def(constructor<const Quaternion&>())
            .def(const_self * other<const Vector3&>())
            .def("SetScale", &Matrix4::setScale)
            .def("GetRotationQuaternion", &Matrix4::extractQuaternion)
            .def("GetTranslation", (Vector3 (Matrix4::*)() const)&Matrix4::getTrans)
            .def("SetTranslation", &Matrix4::setTrans)
            .def("ExtractQuaternion", &Matrix4::extractQuaternion)
            .scope
            [
                def("Identity", ValueFunction<Matrix4, Matrix4::IDENTITY>)
            ],

        class_<Quaternion>("Quaternion")
            .def(constructor<>())
            .def(constructor<const Quaternion&>())
            .def(constructor<Radian, const Vector3&>())
            .def(constructor<const Vector3&, const Vector3&, const Vector3&>())
            .def_readwrite("X", &Quaternion::x)
            .def_readwrite("Y", &Quaternion::y)
            .def_readwrite("Z", &Quaternion::z)
            .def_readwrite("W", &Quaternion::w)
            .def(const_self + other<const Quaternion&>())
            .def(const_self - other<const Quaternion&>())
            .def(const_self * other<const Quaternion&>())
            .def("FromRotationMatrix", &Quaternion::FromRotationMatrix)
            .def("FromAxes", (void (Quaternion::*)(const Vector3&, const Vector3&, const Vector3&))&Quaternion::FromAxes)
            .def("Normalize", &Quaternion::normalise)
            .def("Inverse", &Quaternion::Inverse)
            .def("UnitInverse", &Quaternion::UnitInverse)
            .def("Exp", &Quaternion::Exp)
            .def("Log", &Quaternion::Log)
            .scope
            [
                def("Identity", ValueFunction<Quaternion, Quaternion::IDENTITY>),
                def("Slerp", &Quaternion::Slerp),
                def("Nlerp", &Quaternion::nlerp)
            ],

         class_<ColourValue>("ColourValue")
            .def(constructor<>())
            .def(constructor<Real, Real, Real, Real>())
            .def(constructor<Real, Real, Real>())
            .def_readwrite("R", &ColourValue::r)
            .def_readwrite("G", &ColourValue::g)
            .def_readwrite("B", &ColourValue::b)
            .def_readwrite("A", &ColourValue::a)
            .def(const_self + other<const ColourValue&>())
            .def(const_self - other<const ColourValue&>())
            .def(const_self * other<const ColourValue&>())
            .def(const_self * Real())
            .def(const_self / Real())
            .def("Saturate", &ColourValue::saturate)
            .def("GetHSB", GetHSB)
            .def("SetHSB", &ColourValue::setHSB)
            .scope
            [
                def("Zero", ValueFunction<ColourValue, ColourValue::ZERO>),
                def("Black", ValueFunction<ColourValue, ColourValue::Black>),
                def("White", ValueFunction<ColourValue, ColourValue::White>),
                def("Red", ValueFunction<ColourValue, ColourValue::Red>),
                def("Green", ValueFunction<ColourValue, ColourValue::Green>),
                def("Blue", ValueFunction<ColourValue, ColourValue::Blue>)
            ],

        class_<std::pair<bool, Real> >("IntersectionResult")
            .def_readwrite("Intersects", &std::pair<bool, Real>::first)
            .def_readwrite("Distance", &std::pair<bool, Real>::second),

        class_<AxisAlignedBox>("AxisAlignedBox")
            .def(constructor<>())
            .def(constructor<const AxisAlignedBox&>())
            .def(constructor<const Vector3&, const Vector3&>())
            .def("Set", (void (AxisAlignedBox::*)(const Vector3&, const Vector3&))&AxisAlignedBox::setExtents)
            .property("Min", (const Vector3& (AxisAlignedBox::*)() const)&AxisAlignedBox::getMinimum, (void (AxisAlignedBox::*)(const Vector3&))&AxisAlignedBox::setMinimum)
            .property("Max", (const Vector3& (AxisAlignedBox::*)() const)&AxisAlignedBox::getMaximum, (void (AxisAlignedBox::*)(const Vector3&))&AxisAlignedBox::setMaximum)
            .def("Merge", (void (AxisAlignedBox::*)(const AxisAlignedBox&))&AxisAlignedBox::merge)
            .def("Merge", (void (AxisAlignedBox::*)(const Vector3&))&AxisAlignedBox::merge)
            .def("Transform", &AxisAlignedBox::transform)
            .def("IsEmpty", &AxisAlignedBox::isNull)
            .def("SetEmpty", &AxisAlignedBox::setNull),

        class_<Sphere>("Sphere")
            .def(constructor<>())
            .def(constructor<const Sphere&>())
            .def(constructor<const Vector3&, Real>())
            .property("Center", &Sphere::getCenter, &Sphere::setCenter)
            .property("Radius", &Sphere::getRadius, &Sphere::setRadius)
            .def("Intersects", (bool (Sphere::*)(const Sphere&) const)&Sphere::intersects)
            .def("Intersects", (bool (Sphere::*)(const Vector3&) const)&Sphere::intersects),

        class_<Ray>("Ray")
            .def(constructor<>())
            .def(constructor<const Ray&>())
            .def(constructor<const Vector3&, const Vector3&>())
            .property("Origin", &Ray::getOrigin, &Ray::setOrigin)
            .property("Direction", &Ray::getDirection, &Ray::setDirection)
            .def("GetPoint", &Ray::getPoint),

        class_<Math>("Math")
            .scope
            [
                def("Sqrt", (Real (*)(Real))Math::Sqrt),
                def("InvSqrt", Math::InvSqrt),
                def("UnitRandom", Math::UnitRandom),
                def("RangeRandom", Math::UnitRandom),
                def("SymmetricRandom", Math::SymmetricRandom),
                def("DegreesToRadians", Math::DegreesToRadians),
                def("RadiansToDegrees", Math::RadiansToDegrees),
                def("IsPointInTriangle2D", Math::pointInTri2D),
                def("IsPointInTriangle3D", Math::pointInTri3D),
                def("Intersects", (std::pair<bool, Real> (*)(const Ray&, const Plane&))Math::intersects),
                def("Intersects", (std::pair<bool, Real> (*)(const Ray&, const Sphere&, bool))Math::intersects),
                def("Intersects", (std::pair<bool, Real> (*)(const Ray&, const AxisAlignedBox&))Math::intersects),
                def("Intersects", (std::pair<bool, Real> (*)(const Ray&, const Vector3&, const Vector3&, const Vector3&, bool, bool))Math::intersects),
                def("Intersects", (bool (*)(const Sphere&, const AxisAlignedBox&))Math::intersects),
                def("Intersects", (bool (*)(const Plane&, const AxisAlignedBox&))Math::intersects),
                def("Intersects", (bool (*)(const Sphere&, const Plane&))Math::intersects),
                def("GetFaceNormal", Math::calculateBasicFaceNormal),
                def("GaussianDistribution", Math::gaussianDistribution)
            ],

        class_<AnimationState>("AnimationState")
            .property("Name", &AnimationState::getAnimationName)
            .property("Enabled", &AnimationState::getEnabled, &AnimationState::setEnabled)
            .property("Looped", &AnimationState::getLoop, &AnimationState::setLoop)
            .property("Ended", &AnimationState::hasEnded)
            .property("TimePosition", &AnimationState::getTimePosition, &AnimationState::setTimePosition)
            .property("Length", &AnimationState::getLength, &AnimationState::setLength)
            .property("Weight", &AnimationState::getWeight, &AnimationState::setWeight)
            .def("AddTime", &AnimationState::addTime),

        class_<ScriptTransformSpace>("TransformSpace")
            .enum_("constants")
            [
                value("LOCAL", Node::TS_LOCAL),
                value("PARENT", Node::TS_PARENT),
                value("WORLD", Node::TS_WORLD)
            ],

        class_<ScriptProjectionType>("ProjectionType")
            .enum_("constants")
            [
                value("ORTHOGRAPHIC", PT_ORTHOGRAPHIC),
                value("PERSPECTIVE", PT_PERSPECTIVE)
            ],

        class_<Camera>("Camera")
            .property("Name", &Camera::getName)
            .property("Position", &Camera::getPosition, (void (Camera::*)(const Vector3&))&Camera::setPosition)
            .property("Orientation", &Camera::getOrientation, (void (Camera::*)(const Quaternion&))&Camera::setOrientation)
            .property("NearClipDistance", &Camera::getNearClipDistance, &Camera::setNearClipDistance)
            .property("FarClipDistance", &Camera::getFarClipDistance, &Camera::setFarClipDistance)
            .property("FovY", &Camera::getFOVy, (void (Camera::*)(const Radian&))&Camera::setFOVy)
            .property("ProjectionType", &Camera::getProjectionType, &Camera::setProjectionType),

        class_<SceneNode>("SceneNode")
            .property("Name", &SceneNode::getName)
            .property("Position", &SceneNode::getPosition, (void (SceneNode::*)(const Vector3&))&SceneNode::setPosition)
            .property("Scale", &SceneNode::getScale, (void (SceneNode::*)(const Vector3&))&SceneNode::setScale)
            .property("Orientation", &SceneNode::getOrientation, (void (SceneNode::*)(const Quaternion&))&SceneNode::setOrientation)
            .property("DerivedPosition", &SceneNode::_getDerivedPosition)
            .property("DerivedScale", &SceneNode::_getDerivedScale)
            .property("DerivedOrientation", &SceneNode::_getDerivedOrientation)
            .def("ResetOrientation", &SceneNode::resetOrientation)
            .def("FlipVisibility", &SceneNode::flipVisibility)
            .def("SetVisible", &SceneNode::setVisible)
            .def("ShowBoundingBox", &SceneNode::showBoundingBox)
    ];

    //Set the input system object as a global
    luabind::object globalTable = luabind::globals(script);
    globalTable[moduleName]["InputSystem"] = &ScriptInputSystem::GetInstance();
}

#endif