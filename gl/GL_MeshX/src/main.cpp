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
    // メッシュを読み込み.
    if ( !g_Mesh.LoadFromFile( "../res/test2.obj" ) )
    { return false; }

    // バウンディングスフィアを取得.
    BoundingSphere sphere = g_Mesh.GetSphere();

    // カメラを設定.
    g_Camera.Reset( sphere.radius * 2.5f );

    // 背景色を設定.
    glClearColor( 0.3f, 0.3f, 1.0f, 1.0f );

    // 深度テスト有効化.
    glEnable( GL_DEPTH_TEST );

    // ライティングのセットアップ.
    SetLighting();

    // 正常終了.
    return true;
}

//-------------------------------------------------------------------------------------------
//      終了処理です.
//-------------------------------------------------------------------------------------------
void OnTerm()
{
    // メッシュを解放.
    g_Mesh.Release();
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
    {
        // カメラの更新処理.
        g_Camera.Update();

        // メッシュを描画.
        g_Mesh.Draw();
    }
    glPopMatrix();

    glPushMatrix();
    {
        // ギズモを描画.
        g_Camera.DrawGizmo( g_WindowWidth, g_WindowHeight );
    }
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
    g_Camera.MouseMotion( x, y );
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
