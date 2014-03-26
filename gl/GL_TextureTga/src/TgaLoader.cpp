//-------------------------------------------------------------------------------------------
// File : TgaLoader.cpp
// Desc : Targa Texture Loader.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <TgaLoader.h>
#include <GL/glut.h>


namespace /* anonymous */ {

#pragma pack(push, 1)   // パディングでサイズがずれるのを防ぐ.

/////////////////////////////////////////////////////////////////////////////////////////////
// TgaHeader structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct TgaHeader
{
    unsigned char   IDLength;           //!< IDの長さ.
    unsigned char   ColorMapType;       //!< カラーマップタイプ.
    unsigned char   ImageType;          //!< イメージタイプ.
    unsigned short  ColorMapOrigin;     //!< 最初のカラーマップエントリーのインデックス.
    unsigned short  ColorMapLength;     //!< カラーマップエントリーの数.
    unsigned char   ColorMapDepth;      //!< 各カラーマップエントリーのビット数.
    unsigned short  ImageOriginX;       //!< 画像の原点(X方向).
    unsigned short  ImageOriginY;       //!< 画像の原点(Y方向).
    unsigned short  ImageWidth;         //!< 画像の横幅.
    unsigned short  ImageHeight;        //!< 画像の縦幅.
    unsigned char   BitPerPixel;        //!< 1ピクセル当たりのビット数.
    unsigned char   Descriptor;         //!< 記述子.
};

#pragma pack(pop)

} // namespace /* anonymous */


/////////////////////////////////////////////////////////////////////////////////////////////
// TgaImage class
/////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------
TgaImage::TgaImage()
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
TgaImage::~TgaImage()
{ Release(); }

//-------------------------------------------------------------------------------------------
//      解放処理を行います.
//-------------------------------------------------------------------------------------------
void TgaImage::Release()
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

//-----------------------------------------------------------------------------------------------------
//　　ReadTGA
//　　Desc : TGAファイルの読み込み
//-----------------------------------------------------------------------------------------------------
bool TgaImage::Load(const char *filename)
{
    FILE *fp;
    TgaHeader header;

    //　ファイルを開く
    errno_t err = fopen_s( &fp, filename, "rb");
    if ( err != 0 )
    {
        std::cerr << "Error : File Open Failed" << std::endl;
        std::cerr << "File Name : " << filename << std::endl;
        return false;
    }

    //　ヘッダー情報の読み込み
    fread(&header, sizeof(header), 1, fp);

    //　幅と高さを決める
    m_Width  = header.ImageWidth;
    m_Height = header.ImageHeight;

    //　24 bit
    if ( header.BitPerPixel == 24 )
    {
        m_Format = GL_RGB;
        m_InternalFormat = GL_RGB;
    }
    //　32 bit
    else if ( header.BitPerPixel == 32 )
    {
        m_Format = GL_RGBA;
        m_InternalFormat = GL_RGBA;
    }
    else
    {
        std::cerr << "Error : Unexpected Data." << std::endl;
        return false;
    }

    // 1ピクセル当たりのバイト数.
    m_BytePerPixel = header.BitPerPixel / 8;

    //　データサイズの決定
    m_ImageSize = m_Width * m_Height * m_BytePerPixel;

    //　メモリを確保
    m_pImageData = new(std::nothrow) unsigned char[ m_ImageSize ];
    if ( m_pImageData == nullptr )
    {
        std::cerr << "Error : Memory Allocacte Failed." << std::endl;
        fclose( fp );
        return false;
    }

    //　テクセルデータを一気に読み取り
    fread( m_pImageData, sizeof(unsigned char), m_ImageSize, fp );

    //　BGR(A)をRGB(A)にコンバート
    for ( unsigned int i=0; i<m_ImageSize; i+=m_BytePerPixel )
    {
        unsigned char    temp = m_pImageData[ i + 0 ];
        m_pImageData[ i + 0 ] = m_pImageData[ i + 2 ];
        m_pImageData[ i + 2 ] = temp;
    }

    //　ファイルを閉じる
    fclose(fp);

    // 正常終了.
    return true;
}

//-------------------------------------------------------------------------------------------
//      テクスチャを生成します.
//-------------------------------------------------------------------------------------------
bool TgaImage::CreateGLTexture()
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
void TgaImage::DeleteGLTexture()
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
unsigned int TgaImage::GetID() const
{ return m_ID; }
