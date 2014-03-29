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
// リリースビルド時はコンソールを非表示化.
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif//defined(_NDEBUG) || defined(NDEBUG)


//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <iostream>
#include <GL/freeglut.h>
#include <Mouse.h>
#include <Spring.h>


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------
// Global Variables
//-------------------------------------------------------------------------------------------
int         g_WindowPositionX   = 100;
int         g_WindowPositionY   = 100;
int         g_WindowWidth       = 512;
int         g_WindowHeight      = 512;
double      g_AspectRatio       = g_WindowWidth / g_WindowHeight;
char        g_WindowTitle[]     = "Spring Simulator";
Camera      g_Camera;
Spring1D    g_Spring;

GLfloat     g_ColorObj [4] = { 0.0, 1.0, 0.0, 1.0 };   // 重りの色
GLfloat     g_ColorLine[4] = { 1.0, 1.0, 1.0, 1.0 };   // 線の色

//-------------------------------------------------------------------------------------------
//      ライティングの設定を行います.
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
//      メインエントリーポイントです.
//-------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
#if defined(DEBUG) || defined(_DEBUG)
    // メモリリーク検出
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
//      初期化処理です.
//-------------------------------------------------------------------------------------------
bool OnInit()
{
    // 背景色を設定.
    glClearColor( 0.3f, 0.3f, 1.0f, 1.0f );

    // 深度テスト有効化.
    glEnable( GL_DEPTH_TEST );

    // ライティングのセットアップ.
    SetLighting();

    // シミュレーションの設定.
    g_Spring.SetMass        ( 1.0 );
    g_Spring.SetGravity     ( 9.8 );
    g_Spring.SetTimeStep    ( 0.01 );
    g_Spring.SetConstantK   ( 100.0 );
    g_Spring.SetLength      ( 0.0 );
    g_Spring.SetInitVelocity( 10.0 );
    g_Spring.SetInitPosition( 10.0 );

    // 正常終了.
    return true;
}

//-------------------------------------------------------------------------------------------
//      終了処理です.
//-------------------------------------------------------------------------------------------
void OnTerm()
{
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

    glPushMatrix();

    //　視点の描画
    gluLookAt(
        0.0, 0.0, -50.0,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0);

    // ベルレ法で更新.
    g_Spring.Update( SIMULATION_TYPE_VERLET );

    //　固定点の描画
    glPushMatrix();
    glColor4fv(g_ColorLine);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  g_ColorLine);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  g_ColorLine);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, g_ColorLine);
    glTranslated(0.0, 10.0, 0.0);
    glutSolidSphere(1.0, 5, 5);
    glPopMatrix();

    //　線の描画
    glLineWidth(5.0);
    glBegin(GL_LINES);
    glColor4fv(g_ColorLine);
    glVertex3d(0.0, 10.0, 0.0);
    glVertex3d(0.0, g_Spring.GetPosition(), 0.0);
    glEnd();

    //　物体の描画
    glPushMatrix();
    glColor4fv(g_ColorObj);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  g_ColorObj);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  g_ColorObj);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, g_ColorObj);
    glTranslated(0.0, g_Spring.GetPosition(), 0.0);
    glutSolidSphere(2.0, 5, 5);
    glPopMatrix();

    glPopMatrix();

    // コマンドを実行して，バッファを交換.
    glutSwapBuffers();

#if defined(DEBUG) || defined(_DEBUG)
    // エラーチェック.
    glutReportErrors();
#endif
}

//-------------------------------------------------------------------------------------------
//      マウスのボタン押下時の処理です.
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
//      マウスドラッグ時の処理です.
//-------------------------------------------------------------------------------------------
void OnMotion( int x, int y )
{
}


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
        { OnTerm( ); }
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
