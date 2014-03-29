//-------------------------------------------------------------------------------------------
// File : Mouse.h
// Desc : Mouse Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <Mouse.h>
#include <GL/glut.h>
#include <iostream>


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------
// Constant Values.
//-------------------------------------------------------------------------------------------
const double PI = 3.14159265358979323846264338327;
const char   AXIS_LABEL      [3] = { 'x', 'y', 'z' };
const float  AXIS_COLOR_RED  [4] = {1.0, 0.0, 0.0, 1.0};
const float  AXIS_COLOR_GREEN[4] = {0.0, 0.0, 1.0, 1.0};
const float  AXIS_COLOR_BLUE [4] = {0.0, 1.0, 0.0, 1.0};
const float  AXIS_COLOR_CYAN [4] = {0.0, 1.0, 1.0, 1.0};
const float  AXIS_COLOR_BLACK[4] = {0.0, 0.0, 0.0, 1.0};


//-------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------
template<class T> static inline T ToDeg(T rad) { return (T)( (rad)*( 180.0 / PI ) ); }
template<class T> static inline T ToRad(T deg) { return (T)( (deg)*( PI / 180.0 ) ); }


//-------------------------------------------------------------------------------------------
//      �~�Ղ�`�悵�܂�.
//-------------------------------------------------------------------------------------------
void DrawDisk()
{
    GLUquadricObj *qobj;
    qobj = gluNewQuadric();
    glPushMatrix();
    glRotated( 180, 1.0, 0.0, 0.0 );
    gluDisk( qobj, 0.0, 0.35, 10, 10 );
    glPopMatrix();
}


} // namespace /* anonymous */


/////////////////////////////////////////////////////////////////////////////////////////////
// Camera class
/////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------
Camera::Camera()
{ Reset( 5.0f ); }

//-------------------------------------------------------------------------------------------
//      �R�s�[�R���X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------
Camera::Camera( const Camera& value )
: m_Right   ( value.m_Right )
, m_Left    ( value.m_Left )
, m_Middle  ( value.m_Middle )
, m_Distance( value.m_Distance )
, m_Angle   ( value.m_Angle )
, m_Position( value.m_Position )
, m_Target  ( value.m_Target )
, m_Upward  ( value.m_Upward )
, m_Move    ( value.m_Move )
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------
Camera::~Camera()
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------
//      �}�E�X�h���b�O���̏����ł�.
//-------------------------------------------------------------------------------------------
void Camera::MouseMotion( int x, int y ) 
{
    //�@���{�^���̏���
    if ( m_Left.state == Push )
    {
        //�@�ړ��ʂ��v�Z
        m_Left.current.x = ( float(x) - m_Left.before.x ) + m_Left.after.x;
        m_Left.current.y = ( float(y) - m_Left.before.y ) + m_Left.after.y;

        if ( m_Left.current.y >= 360.0 ) 
        { m_Left.current.y -= 360.0; }
        else if ( m_Left.current.y < 0.0 )
        { m_Left.current.y += 360.0; }

        m_Angle.x = ToRad( m_Left.current.x );
        m_Angle.y = ToRad( m_Left.current.y );
    }

    //�@�E�{�^���̏���
    if ( m_Right.state == Push )
    {
        m_Right.current.x  = (  float(x) - m_Right.before.x ) + m_Right.after.x;
        m_Right.current.y  = ( -float(y) - m_Right.before.y ) + m_Right.after.y;
    }

    //�@���{�^���̏���
    if ( m_Middle.state == Push )
    {
        //�@�ړ��ʂ��v�Z
        m_Middle.current.x = ( float(x) - m_Middle.before.x ) + m_Middle.after.x;
        m_Middle.current.y = ( float(y) - m_Middle.before.y ) + m_Middle.after.y;
        m_Move.x =  m_Middle.current.x * 0.005f;
        m_Move.y = -m_Middle.current.y * 0.005f;
    }
}

//-------------------------------------------------------------------------------------------
//      �}�E�X�{�^���������̏����ł�.
//-------------------------------------------------------------------------------------------
void Camera::MouseInput( int button, int state, int x, int y ) 
{
    switch( button )
    {
        // ���{�^��
        case GLUT_LEFT_BUTTON :
            {
                if( state == GLUT_DOWN )
                {
                    m_Left.before.x = float(x);
                    m_Left.before.y = float(y);
                    m_Left.state    = Push;
                }
                else if( state == GLUT_UP )
                {
                    m_Left.after.x = m_Left.current.x;
                    m_Left.after.y = m_Left.current.y;
                    m_Left.state   = Release;
                }
            }
            break;

        // �E�{�^��
        case GLUT_RIGHT_BUTTON :
            {
                if( state == GLUT_DOWN ) 
                {
                    m_Right.before.x =  float(x);
                    m_Right.before.y = -float(y);
                    m_Right.state    = Push;
                }
                else if( state == GLUT_UP )
                {
                    m_Right.after.x = m_Right.current.x;
                    m_Right.after.y = m_Right.current.y;
                    m_Right.state   = Release;
                }
            }
            break;

        // ���{�^��
        case GLUT_MIDDLE_BUTTON :
                {
                if ( state == GLUT_DOWN )
                {
                    m_Middle.before.x = float(x);
                    m_Middle.before.y = float(y);
                    m_Middle.state    = Push;
                }
                else if ( state == GLUT_UP )
                {
                    m_Middle.after.x = m_Middle.current.x;
                    m_Middle.after.y = m_Middle.current.y;
                    m_Middle.state   = Release;
                }
            }
            break;
    }

}

//-------------------------------------------------------------------------------------------
//      �p�����[�^�����Z�b�g���܂�.
//-------------------------------------------------------------------------------------------
void Camera::Reset( float distance )
{
    m_Distance  = distance;

    m_Left  .Reset();
    m_Right .Reset();
    m_Middle.Reset();

    m_Angle.x = ToRad(  45.0f ); //  45�x.
    m_Angle.y = ToRad( 315.0f ); // -45�x.

    m_Left.after.x =  45.0f; //  45�x.
    m_Left.after.y = 315.0f; // -45�x.

    m_Position  = Vec3( 0.0f, 0.0f, 0.0f );
    m_Target    = Vec3( 0.0f, 0.0f, 0.0f );
    m_Upward    = Vec3( 0.0f, 1.0f, 0.0f );
    m_Move      = Vec3( 0.0f, 0.0f, 0.0f );
}

//-------------------------------------------------------------------------------------------
//      �r���[�s��̍X�V�������s���܂�.
//-------------------------------------------------------------------------------------------
void Camera::Update()
{
    float dist = m_Distance;
    dist += ( m_Right.current.y * 0.5f );

    // �Ԃ���΂Ȃ��悤�ɐ���.
    if ( dist < FLT_EPSILON )
    { dist = FLT_EPSILON; }

    float sinH = sinf( m_Angle.x );
    float cosH = cosf( m_Angle.x );

    float sinV = sinf( m_Angle.y );
    float cosV = cosf( m_Angle.y );

    //�@���_�ʒu���Z�o.
    m_Position.x = m_Move.x + dist * ( -cosV * sinH );
    m_Position.y = m_Move.y + dist * ( -sinV );
    m_Position.z = m_Move.z + dist * ( -cosV * cosH );


    // �����_�ʒu���Z�o.
    m_Target.x = m_Move.x;
    m_Target.y = m_Move.y;
    m_Target.z = m_Move.z;

    // ������x�N�g�����Z�o.
    m_Upward.x = ( -sinV * sinH );
    m_Upward.y = ( cosV );
    m_Upward.z = ( -sinV * cosH );

    //�@���_�ʒu�̐ݒ�
    gluLookAt(
        m_Position.x, m_Position.y, m_Position.z,
        m_Target.x,   m_Target.y,   m_Target.z,
        m_Upward.x,   m_Upward.y,   m_Upward.z );

}

//-------------------------------------------------------------------------------------------
//      �M�Y����`�悵�܂�.
//-------------------------------------------------------------------------------------------
void Camera::DrawGizmo(int w, int h)
{
    const float dist = 15.0f;
    Vec3 eye;

    //�@�E�B���h�E�S�̂��r���[�|�[�g�ɂ���
    glViewport( w - 100, h - 100, 100, 100 );

    //�@�����ϊ��s��̐ݒ�
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, 1, 1, 100000.0);

    //�@���f���r���[�ϊ��̐ݒ�
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float sinH = sinf( m_Angle.x );
    float cosH = cosf( m_Angle.x );

    float sinV = sinf( m_Angle.y );
    float cosV = cosf( m_Angle.y );

    //�@���_�ʒu�̐ݒ�
    eye.x = dist * ( -cosV * sinH );
    eye.y = dist * ( -sinV );
    eye.z = dist * ( -cosV * cosH );

    // �r���[�s����v�Z.
    gluLookAt( 
        eye.x,      eye.y,      eye.z,
        0.0,        0.0,        0.0,
        m_Upward.x, m_Upward.y, m_Upward.z );

    //�@���C�e�B���O����
    GLboolean isLighting;
    glGetBooleanv( GL_LIGHTING, &isLighting );
    glDisable( GL_LIGHTING );

    //�@���̕���
    glPushMatrix();
    {
        glColor4fv( AXIS_COLOR_BLACK ); 
        glRasterPos3d( 2.25, 0.0, 0.0 );
        glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_10, (int)AXIS_LABEL[0] );
        glRasterPos3d( 0.0, 2.25, 0.0 );
        glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_10, (int)AXIS_LABEL[1] );
        glRasterPos3d( 0.0, 0.0, 2.25 );
        glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_10, (int)AXIS_LABEL[2] );
    }
    glPopMatrix();

    //�@���C�e�B���O�L��
    if ( isLighting )
    { glEnable(GL_LIGHTING); }

    //�@x����
    glPushMatrix();
    {
        glColor4fv( AXIS_COLOR_RED );
        glMaterialfv( GL_FRONT, GL_AMBIENT,  AXIS_COLOR_RED );
        glMaterialfv( GL_FRONT, GL_DIFFUSE,  AXIS_COLOR_RED );
        glMaterialfv( GL_FRONT, GL_SPECULAR, AXIS_COLOR_RED );
        glTranslated( 1.75, 0.0, 0.0);
        glRotated( -90.0, 0.0, 1.0, 0.0);
        glutSolidCone( 0.35, 1.0, 10, 10);
        DrawDisk();
    }
    glPopMatrix();

    //�@y����
    glPushMatrix();
    {
        glColor4fv( AXIS_COLOR_GREEN );
        glMaterialfv( GL_FRONT, GL_AMBIENT,  AXIS_COLOR_GREEN );
        glMaterialfv( GL_FRONT, GL_DIFFUSE,  AXIS_COLOR_GREEN );
        glMaterialfv( GL_FRONT, GL_SPECULAR, AXIS_COLOR_GREEN );
        glTranslated( 0.0, 1.75, 0.0 );
        glRotated( 90.0, 1.0, 0.0, 0.0 );
        glutSolidCone( 0.35, 1.0, 10, 10 );
        DrawDisk();
    }
    glPopMatrix();

    //�@z����
    glPushMatrix();
    {
        glColor4fv( AXIS_COLOR_BLUE );
        glMaterialfv( GL_FRONT, GL_AMBIENT,  AXIS_COLOR_BLUE );
        glMaterialfv( GL_FRONT, GL_DIFFUSE,  AXIS_COLOR_BLUE );
        glMaterialfv( GL_FRONT, GL_SPECULAR, AXIS_COLOR_BLUE );
        glTranslated( 0.0, 0.0, 1.75 );
        glRotated( 180.0, 1.0, 0.0, 0.0 );
        glutSolidCone( 0.35, 1.0, 10, 10 );
        DrawDisk();
    }
    glPopMatrix();

    //�@�L���[�u
    glPushMatrix();
    {
        glColor4fv( AXIS_COLOR_CYAN );
        glMaterialfv( GL_FRONT, GL_AMBIENT,  AXIS_COLOR_CYAN );
        glMaterialfv( GL_FRONT, GL_DIFFUSE,  AXIS_COLOR_CYAN );
        glMaterialfv( GL_FRONT, GL_SPECULAR, AXIS_COLOR_CYAN );
        glutSolidCube( 1.0 );
    }
    glPopMatrix();

    //�@x����
    glPushMatrix();
    {
        glTranslated( -1.75, 0.0, 0.0 );
        glRotated( 90.0, 0.0, 1.0, 0.0 );
        glutSolidCone( 0.35, 1.0, 10, 10 );
        DrawDisk();
    }
    glPopMatrix();

    //�@y����
    glPushMatrix();
    {
        glTranslated( 0.0, -1.75, 0.0 );
        glRotated( -90.0, 1.0, 0.0, 0.0 );
        glutSolidCone( 0.35, 1.0, 10, 10 );
        DrawDisk();
    }
    glPopMatrix();

    //�@z����
    glPushMatrix();
    {
        glTranslated( 0.0, 0.0, -1.75 );
        glutSolidCone( 0.35, 1.0, 10, 10 );
        DrawDisk();
    }
    glPopMatrix();

}

//-------------------------------------------------------------------------------------------
//      ������Z�q�ł�.
//-------------------------------------------------------------------------------------------
Camera& Camera::operator = ( const Camera& value )
{
    m_Right     = value.m_Right;
    m_Left      = value.m_Left;
    m_Middle    = value.m_Middle;
    m_Distance  = value.m_Distance;
    m_Angle     = value.m_Angle;
    m_Position  = value.m_Position;
    m_Target    = value.m_Target;
    m_Upward    = value.m_Upward;
    m_Move      = value.m_Move;

    return (*this);
}