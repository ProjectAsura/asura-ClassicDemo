//-------------------------------------------------------------------------------------------
// File : RawLoader.h
// Desc : Raw Texture Loader.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <RawLoader.h>
#include <GL/glut.h>


/////////////////////////////////////////////////////////////////////////////////////////////
// RawImage class
/////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------
RawImage::RawImage()
: m_ImageSize       ( 0 )
, m_Format          ( 0 )
, m_InternalFormat  ( 0 )
, m_Width           ( 0 )
, m_Height          ( 0 )
, m_BytePerPixel    ( 0 )
, m_ID              ( 0 )
, m_pImageData      ( nullptr )
{ /* DO_NOTHING */ }


//-------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------
RawImage::~RawImage()
{ Release(); }

//-------------------------------------------------------------------------------------------
//      解放処理を行います.
//-------------------------------------------------------------------------------------------
void RawImage::Release()
{
    if ( m_pImageData )
    {
        delete[] m_pImageData;
        m_pImageData = nullptr;
    }

    m_ImageSize      = 0;
    m_Format         = 0;
    m_InternalFormat = 0;
    m_Width          = 0;
    m_Height         = 0;
    m_BytePerPixel   = 0;
}

//-------------------------------------------------------------------------------------------
//      テクスチャを読み込みします.
//-------------------------------------------------------------------------------------------
bool RawImage::Load
(
    const char*         filename,
    const unsigned int  width,
    const unsigned int  height,
    bool                alphaFlag
)
{
    std::ifstream file;

    // ファイルを開く.
    file.open(filename, std::ios::in | std::ios::binary);
    if ( !file.is_open() )
    {
        std::cerr << "Error : File Open Failed.";
        std::cerr << "File Name : " << filename << std::endl;
        return false;
    }

    m_Width  = width;
    m_Height = height;

    if ( alphaFlag )
    {
        m_BytePerPixel   = 4;
        m_ImageSize      = m_Width * m_Height * m_BytePerPixel;
        m_Format         = GL_RGBA;
        m_InternalFormat = GL_RGBA;
    }
    else
    {
        m_BytePerPixel   = 3;
        m_ImageSize      = m_Width * m_Height * m_BytePerPixel;
        m_Format         = GL_RGB;
        m_InternalFormat = GL_RGB;
    }

    // メモリ確保.
    m_pImageData = new(std::nothrow) unsigned char [m_ImageSize];
    if ( m_pImageData == nullptr )
    {
        std::cerr << "Error : Memory Allocate Faied." << std::endl;
        return false;
    }

    // 一括読み込み.
    file.read((char*)m_pImageData, m_ImageSize);

    // ファイルを閉じる.
    file.close();

    // 正常終了.
    return true;
}

//-------------------------------------------------------------------------------------------
//      テクスチャを生成します.
//-------------------------------------------------------------------------------------------
bool RawImage::CreateGLTexture()
{
    if ( m_pImageData == nullptr )
    { return false; }

    //　テクスチャを生成
    glGenTextures(1, &m_ID);

    //　テクスチャをバインドする
    glBindTexture(GL_TEXTURE_2D, m_ID);

    if ( m_BytePerPixel == 4 )
    { glPixelStorei(GL_UNPACK_ALIGNMENT, 4); }
    else 
    { glPixelStorei(GL_UNPACK_ALIGNMENT, 1); }

    //　テクスチャの割り当て
    gluBuild2DMipmaps(
        GL_TEXTURE_2D,
        m_InternalFormat,
        m_Width,
        m_Height,
        m_Format,
        GL_UNSIGNED_BYTE,
        m_pImageData );

    //　テクスチャを拡大・縮小する方法の指定
    glTexParameteri(GL_TEXTURE_2D, 	GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, 	GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    //　テクスチャ環境
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // アンバインドしておく.
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

//-------------------------------------------------------------------------------------------
//      テクスチャを破棄します.
//-------------------------------------------------------------------------------------------
void RawImage::DeleteGLTexture()
{
    if ( m_ID )
    {
        glDeleteTextures( 1, &m_ID );
        m_ID = 0;
    }
}

//-------------------------------------------------------------------------------------------
//      テクスチャIDを取得します.
//-------------------------------------------------------------------------------------------
unsigned int RawImage::GetID() const
{ return m_ID; }
