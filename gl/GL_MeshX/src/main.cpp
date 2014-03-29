//-------------------------------------------------------------------------------------------
// File : main.cpp
// Desc : First Polygon
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------


#if defined(DEBUG) || defined(_DEBUG)
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif//defined(DEBUG) || defined(_DEBUG)

#if defined(_NDEBUG) || defined(NDEBUG)
    // �����[�X�r���h���̓R���\�[�����\����.
    #pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif//defined(_NDEBUG) || defined(NDEBUG)


//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <iostream>
#include <GL/freeglut.h>
#include <MeshOBJ.h>
#include <Mouse.h>


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------
// Global Variables
//-------------------------------------------------------------------------------------------
int     g_WindowPositionX   = 100;
int     g_WindowPositionY   = 100;
int     g_WindowWidth       = 512;
int     g_WindowHeight      = 512;
double  g_AspectRatio       = g_WindowWidth / g_WindowHeight;
char    g_WindowTitle[]     = "Mesh Loader (1) - OBJ -";
Camera  g_Camera;
MeshOBJ g_Mesh;


//-------------------------------------------------------------------------------------------
//      ���C�e�B���O�̐ݒ���s���܂�.
//-------------------------------------------------------------------------------------------
void SetLighting()
{
    GLfloat lightAmbientColor [4] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightDiffuseColor [4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightSpecularColor[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat lightPosition     [4] = { 0.0f, 100.0f, 100.0f, 0.0f };

    glEnable( GL_LIGHTING);
    glEnable( GL_LIGHT0);
    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );
    glLightfv( GL_LIGHT0, GL_AMBIENT,  lightAmbientColor );
    glLightfv( GL_LIGHT0, GL_DIFFUSE,  lightDiffuseColor );
    glLightfv( GL_LIGHT0, GL_SPECULAR, lightSpecularColor );
}


} // namespace /* anonymous */


//-------------------------------------------------------------------------------------------
// Forward Declarations.
//-------------------------------------------------------------------------------------------
bool    OnInit();
void    OnTerm();
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
#if defined(DEBUG) || defined(_DEBUG)
    // ���������[�N���o
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif//defined(DEBUG) || defined(_DEBUG)
    {
        glutInit( &argc, argv );
        glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );
        glutInitWindowPosition( g_WindowPositionX, g_WindowPositionY );
        glutInitWindowSize( g_WindowWidth, g_WindowHeight );
        glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
        glutCreateWindow( g_WindowTitle );
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

        OnTerm();
    }

    return 0;
}


//-------------------------------------------------------------------------------------------
//      �����������ł�.
//-------------------------------------------------------------------------------------------
bool OnInit()
{
    // ���b�V����ǂݍ���.
    if ( !g_Mesh.LoadFromFile( "../res/test2.obj" ) )
    { return false; }

    // �o�E���f�B���O�X�t�B�A���擾.
    BoundingSphere sphere = g_Mesh.GetSphere();

    // �J������ݒ�.
    g_Camera.Reset( sphere.radius * 2.5f );

    // �w�i�F��ݒ�.
    glClearColor( 0.3f, 0.3f, 1.0f, 1.0f );

    // �[�x�e�X�g�L����.
    glEnable( GL_DEPTH_TEST );

    // ���C�e�B���O�̃Z�b�g�A�b�v.
    SetLighting();

    // ����I��.
    return true;
}

//-------------------------------------------------------------------------------------------
//      �I�������ł�.
//-------------------------------------------------------------------------------------------
void OnTerm()
{
    // ���b�V�������.
    g_Mesh.Release();
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

    // �r���[�|�[�g�̐ݒ�.
    glViewport( 0, 0, g_WindowWidth, g_WindowHeight );

    // �ˉe�s��̐ݒ�.
    glMatrixMode( GL_PROJECTION );
    // �P�ʍs��ŏ�����.
    glLoadIdentity();
    // �ˉe�s���ݒ�.
    gluPerspective( 45.0, g_AspectRatio, 0.1, 1000.0 );

    // ���[���h�r���[�s��̐ݒ�.
    glMatrixMode( GL_MODELVIEW );
    // �P�ʍs��ŏ�����.
    glLoadIdentity();

    glPushMatrix();
    {
        // �J�����̍X�V����.
        g_Camera.Update();

        // ���b�V����`��.
        g_Mesh.Draw();
    }
    glPopMatrix();

    glPushMatrix();
    {
        // �M�Y����`��.
        g_Camera.DrawGizmo( g_WindowWidth, g_WindowHeight );
    }
    glPopMatrix();

    // �R�}���h�����s���āC�o�b�t�@������.
    glutSwapBuffers();

#if defined(DEBUG) || defined(_DEBUG)
    // �G���[�`�F�b�N.
    glutReportErrors();
#endif
}

//-------------------------------------------------------------------------------------------
//      �}�E�X�̃{�^���������̏����ł�.
//-------------------------------------------------------------------------------------------
void OnMouse( int button, int state, int x, int y )
{
    g_Camera.MouseInput( button, state, x, y );

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
{
    g_Camera.MouseMotion( x, y );
}


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
        { OnTerm( ); }
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
