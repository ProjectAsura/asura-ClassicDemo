//-------------------------------------------------------------------------------------------
// File : main.cpp
// Desc : First Polygon
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <iostream>
#include <GL/glut.h>
#include <RawLoader.h>


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------
// Global Variables
//-------------------------------------------------------------------------------------------
int         g_WindowPositionX   = 100;
int         g_WindowPositionY   = 100;
int         g_WindowWidth       = 512;
int         g_WindowHeight      = 512;
double      g_AspectRatio       = g_WindowWidth / g_WindowHeight;
char        g_WindowTitle[]     = "Texture Mapping (1) - Raw File -";

RawImage    g_Texture;

} // namespace /* anonymous */

//-------------------------------------------------------------------------------------------
// Forward Declarations.
//-------------------------------------------------------------------------------------------
bool    OnInit();
void    OnTerm( int code );
void    OnDisplay();
void    OnIdle();
void    OnReshape( int x, int y );
void    OnMouse( int button, int state, int x, int y );
void    OnMotion( int x, int y );
void    OnPassiveMotion( int x, int y );
void    OnKeyboard( unsigned char key, int x, int y );
void    OnSpecial( int key, int x, int y );



//-------------------------------------------------------------------------------------------
//      ���C���G���g���[�|�C���g�ł�.
//-------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    __glutInitWithExit( &argc, argv, OnTerm );
    glutInitWindowPosition( g_WindowPositionX, g_WindowPositionY );
    glutInitWindowSize( g_WindowWidth, g_WindowHeight );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
    __glutCreateWindowWithExit( g_WindowTitle, OnTerm );
    glutDisplayFunc( OnDisplay );
    glutReshapeFunc( OnReshape );
    glutIdleFunc( OnIdle );
    glutMouseFunc( OnMouse );
    glutMotionFunc( OnMotion );
    glutPassiveMotionFunc( OnPassiveMotion );
    glutKeyboardFunc( OnKeyboard );
    glutSpecialFunc( OnSpecial );

    if ( OnInit() )
    { glutMainLoop(); }
    else
    { OnTerm( -1 ); }

    return 0;
}


//-------------------------------------------------------------------------------------------
//      �����������ł�.
//-------------------------------------------------------------------------------------------
bool OnInit()
{
    glClearColor( 0.3f, 0.3f, 1.0f, 1.0f );
    glEnable( GL_DEPTH_TEST );

    char* filename = "../res/sample.raw"; // �v���W�F�N�g�f�B���N�g������̑��΃p�X.

    if ( !g_Texture.Load( filename, 512, 512, false ) ) 
    { return false; }

    if ( !g_Texture.CreateGLTexture() )
    { return false; }

    return true;
}

//-------------------------------------------------------------------------------------------
//      �I�������ł�.
//-------------------------------------------------------------------------------------------
void OnTerm( int code )
{
    g_Texture.Release();
    g_Texture.DeleteGLTexture();
    std::exit( code );
}


//-------------------------------------------------------------------------------------------
//      �A�C�h�����O���̏����ł�.
//-------------------------------------------------------------------------------------------
void OnIdle()
{
    glutPostRedisplay();
}


//-------------------------------------------------------------------------------------------
//      ���T�C�Y���̏����ł�.
//-------------------------------------------------------------------------------------------
void OnReshape( int x, int y )
{
    g_WindowWidth  = ( x >= 1 ) ? x : 1;
    g_WindowHeight = ( y >= 1 ) ? y : 1;
    g_AspectRatio = g_WindowWidth / g_WindowHeight;
}


//-------------------------------------------------------------------------------------------
//      �`�掞�̏����ł�.
//-------------------------------------------------------------------------------------------
void OnDisplay()
{
    // �o�b�t�@���N���A.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //�@�e�N�X�`���}�b�s���O�L����
    glEnable(GL_TEXTURE_2D);
    //�@�e�N�X�`�����o�C���h
    glBindTexture(GL_TEXTURE_2D, g_Texture.GetID());
    //�@�F�̎w��
    glColor4f(1.0, 1.0, 1.0, 1.0);

    //�@�l�p�`���e�N�X�`�����W���ŕ`��
    glBegin(GL_QUADS);
    {
        double size = 0.5;
        glTexCoord2d( -1.0, 1.0 );     glVertex3d( -size, -size, 0.0 );
        glTexCoord2d( -1.0, 0.0 );     glVertex3d( -size,  size, 0.0 );
        glTexCoord2d(  0.0, 0.0 );     glVertex3d(  size,  size, 0.0 );
        glTexCoord2d(  0.0, 1.0 );     glVertex3d(  size, -size, 0.0 );
    }
    glEnd();

    // �e�N�X�`�����A���o�C���h.
    glBindTexture(GL_TEXTURE_2D, 0);
    //�@�e�N�X�`���}�b�s���O������
    glDisable(GL_TEXTURE_2D);

    // �R�}���h�����s���āC�o�b�t�@������.
    glutSwapBuffers();
}

//-------------------------------------------------------------------------------------------
//      �}�E�X�̃{�^���������̏����ł�.
//-------------------------------------------------------------------------------------------
void OnMouse( int button, int state, int x, int y )
{
    switch( button )
    {
    case GLUT_LEFT_BUTTON:
        {
            if ( state == GLUT_DOWN )
            {
            }
            else if ( state == GLUT_UP )
            {
            }
        }
        break;

    case GLUT_MIDDLE_BUTTON:
        {
            if ( state == GLUT_DOWN )
            {
            }
            else if ( state == GLUT_UP )
            {
            }
        }
        break;

    case GLUT_RIGHT_BUTTON:
        {
            if ( state == GLUT_DOWN )
            {
            }
            else if ( state == GLUT_UP )
            {
            }
        }
        break;
    }
}


//-------------------------------------------------------------------------------------------
//      �}�E�X�h���b�O���̏����ł�.
//-------------------------------------------------------------------------------------------
void OnMotion( int x, int y )
{ /* DO_NOTHING */ }


//-------------------------------------------------------------------------------------------
//      �}�E�X�ړ����̏����ł�.
//-------------------------------------------------------------------------------------------
void OnPassiveMotion( int x, int y )
{ /* DO_NOTHING */ }


//-------------------------------------------------------------------------------------------
//      �L�[�{�[�h�������̏����ł�.
//-------------------------------------------------------------------------------------------
void OnKeyboard( unsigned char key, int x, int y )
{
    switch( key )
    {
    // ESC�L�[.
    case '\033':
        { OnTerm( 0 ); }
        break;

    default:
        break;
    }
}


//-------------------------------------------------------------------------------------------
//      ����L�[�������̏����ł�.
//-------------------------------------------------------------------------------------------
void OnSpecial( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_F1:
        break;

    case GLUT_KEY_F2:
        break;

    case GLUT_KEY_F3:
        break;

    case GLUT_KEY_F4:
        break;

    case GLUT_KEY_F5:
        break;

    case GLUT_KEY_F6:
        break;

    case GLUT_KEY_F7:
        break;

    case GLUT_KEY_F8:
        break;

    case GLUT_KEY_F9:
        break;

    case GLUT_KEY_F10:
        break;

    case GLUT_KEY_F11:
        break;

    case GLUT_KEY_F12:
        break;

    case GLUT_KEY_LEFT:
        break;

    case GLUT_KEY_RIGHT:
        break;

    case GLUT_KEY_UP:
        break;

    case GLUT_KEY_DOWN:
        break;

    case GLUT_KEY_PAGE_UP:
        break;

    case GLUT_KEY_PAGE_DOWN:
        break;

    case GLUT_KEY_HOME:
        break;

    case GLUT_KEY_END:
        break;

    case GLUT_KEY_INSERT:
        break;
    }
}
