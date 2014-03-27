//-------------------------------------------------------------------------------------------
// File : Mouse.h
// Desc : Mouse Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

#ifndef _MOUSE_H_
#define _MOUSE_H_

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <TinyMath.h>


/////////////////////////////////////////////////////////////////////////////////////////////
// MouseState enum
/////////////////////////////////////////////////////////////////////////////////////////////
typedef enum MouseState
{ 
    Push,
    Release,
    None
};

/////////////////////////////////////////////////////////////////////////////////////////////
// Cursor class
/////////////////////////////////////////////////////////////////////////////////////////////
struct Cursor
{
    float x;
    float y;

    Cursor()
    : x( 0.0f )
    , y( 0.0f )
    { /* DO_NOTHING */ }

    Cursor( float nx, float ny ) 
    : x( nx )
    , y( ny )
    { /* DO_NOTHING */ }

    ~Cursor()
    { /* DO_NOTHING */ }

    void Reset()
    {
        x = 0.0f;
        y = 0.0f;
    }
};


/////////////////////////////////////////////////////////////////////////////////////////////
//  MouseButton struct
/////////////////////////////////////////////////////////////////////////////////////////////
struct MouseButton
{
    Cursor      before;
    Cursor      current;
    Cursor      after;
    MouseState  state;

    MouseButton()
    : before    ()
    , current   ()
    , after     ()
    , state     ( None )
    { /* DO_NOTHING */ }

    ~MouseButton()
    { /* DO_NOTHING */ }

    void Reset()
    {
        before .Reset();
        current.Reset();
        after  .Reset();
        state = None;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////
// Camara class
/////////////////////////////////////////////////////////////////////////////////////////////
class Camera
{
public:
    Camera(float distance=5.0);
    ~Camera();
    void Reset();
    void MouseInput(int button, int state, int x, int y);
    void MouseMotion(int x, int y);
    void Set();
    void DrawGizmo(int w, int h);

private:
    MouseButton m_Right;
    MouseButton m_Left;
    MouseButton m_Middle;

    float m_Distance;
    Vec2  m_Angle;
    Vec3  m_Position;
    Vec3  m_Target;
    Vec3  m_Upward;
    Vec3  m_Move;
};



#endif		//Å@_MOUSE_WIN_H_INCLUDED_
