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


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------
// Global Variables
//-------------------------------------------------------------------------------------------
int     g_WindowPositionX   = 100;
int     g_WindowPositionY   = 100;
int     g_WindowWidth       = 512;
int     g_WindowHeight      = 512;
double  g_AspectRatio       = g_WindowWidth / g_WindowHeight;
char    g_WindowTitle[]     = "First Polygon";

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
//      メインエントリーポイントです.
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
//      初期化処理です.
//-------------------------------------------------------------------------------------------
bool OnInit()
{
    glClearColor( 0.3f, 0.3f, 1.0f, 1.0f );
    glEnable( GL_DEPTH_TEST );

    return true;
}

//-------------------------------------------------------------------------------------------
//      終了処理です.
//-------------------------------------------------------------------------------------------
void OnTerm( int code )
{
    std::exit( code );
}


//-------------------------------------------------------------------------------------------
//      アイドリング時の処理です.
//-------------------------------------------------------------------------------------------
void OnIdle()
{
    glutPostRedisplay();
}


//-------------------------------------------------------------------------------------------
//      リサイズ時の処理です.
//-------------------------------------------------------------------------------------------
void OnReshape( int x, int y )
{
    g_WindowWidth  = ( x >= 1 ) ? x : 1;
    g_WindowHeight = ( y >= 1 ) ? y : 1;
    g_AspectRatio = g_WindowWidth / g_WindowHeight;
}


//-------------------------------------------------------------------------------------------
//      描画時の処理です.
//-------------------------------------------------------------------------------------------
void OnDisplay()
{
    // バッファをクリア.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // ビューポートの設定.
    glViewport( 0, 0, g_WindowWidth, g_WindowHeight );

    // 射影行列の設定.
    glMatrixMode( GL_PROJECTION );
    // 単位行列で初期化.
    glLoadIdentity();
    // 射影行列を設定.
    gluPerspective( 45.0, g_AspectRatio, 0.1, 1000.0 );

    // ワールドビュー行列の設定.
    glMatrixMode( GL_MODELVIEW );
    // 単位行列で初期化.
    glLoadIdentity();

    // 視点の設定
    gluLookAt( 
        0.0, 0.0, -5.0,      // カメラ位置.
        0.0, 0.0,  0.0,      // カメラの注視点.
        0.0, 1.0,  0.0 );    // カメラの上向きベクトル.

    // 三角形ポリゴンを描画.
    glBegin(GL_TRIANGLES);
    {
        // 頂点1
        glColor3d ( 1.0, 1.0, 0.0 );
        glVertex3d( 0.0, 0.825, 0.0 );

        // 頂点2
        glColor3d (  0.0,  1.0, 1.0 );
        glVertex3d( -1.0, -0.825, 0.0 );

        // 頂点3
        glColor3d ( 1.0,  0.0, 1.0 );
        glVertex3d( 1.0, -0.825, 0.0 );
    }
    glEnd();

    // コマンドを実行して，バッファを交換.
    glutSwapBuffers();
}

//-------------------------------------------------------------------------------------------
//      マウスのボタン押下時の処理です.
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
//      マウスドラッグ時の処理です.
//-------------------------------------------------------------------------------------------
void OnMotion( int x, int y )
{ /* DO_NOTHING */ }


//-------------------------------------------------------------------------------------------
//      マウス移動時の処理です.
//-------------------------------------------------------------------------------------------
void OnPassiveMotion( int x, int y )
{ /* DO_NOTHING */ }


//-------------------------------------------------------------------------------------------
//      キーボード押下時の処理です.
//-------------------------------------------------------------------------------------------
void OnKeyboard( unsigned char key, int x, int y )
{
    switch( key )
    {
    // ESCキー.
    case '\033':
        { OnTerm( 0 ); }
        break;

    default:
        break;
    }
}


//-------------------------------------------------------------------------------------------
//      特殊キー押下時の処理です.
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
