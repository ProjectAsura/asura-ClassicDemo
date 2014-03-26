//-------------------------------------------------------------------------------------------
// File : BmpLoader.cpp
// Desc : Bitmap Texture Loader.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <BmpLoader.h>
#include <GL/glut.h>


namespace /* anonymous */ {

#pragma pack(push, 1 )

/////////////////////////////////////////////////////////////////////////////////////////////
// BmpInfoHeader structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct BmpInfoHeader
{
    unsigned int    biSize;
    long            biWidth;
    long            biHeight;
    unsigned short  biPlanes;
    unsigned short  biBitCount;
    unsigned int    biCompression;
    unsigned int    biSizeImage;
    long            biXPelsPerMeter;
    long            biYPelsPerMeter;
    unsigned int    biClrUsed;
    unsigned int    biClrImportant;
};

/////////////////////////////////////////////////////////////////////////////////////////////
// BmpFileHeader structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct BmpFileHeader
{
    unsigned short  bfType;
    unsigned int    bfSize;
    unsigned short  bfReserved1;
    unsigned short  bfReserved2;
    unsigned int    bfOffBits;
};

#pragma pack(pop)

} // namespace /* anonymous */


/////////////////////////////////////////////////////////////////////////////////////////////
// BmpImage class
/////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------
BmpImage::BmpImage()
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
BmpImage::~BmpImage()
{ Release(); }

//-------------------------------------------------------------------------------------------
//      解放処理を行います.
//-------------------------------------------------------------------------------------------
void BmpImage::Release()
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
//      読み込み処理を行います.
//-------------------------------------------------------------------------------------------
bool BmpImage::Load(const char *filename)
{
    FILE *fp;

    BmpInfoHeader infoHeader;
    BmpFileHeader header;

    // ファイルを開く
    errno_t err = fopen_s( &fp, filename, "rb" );
    if ( err != 0 )
    {
        std::cerr << "Error : File Open Failed.";
        std::cerr << "File Name : " << filename << std::endl;
        return false;
    }

    // ヘッダー情報の読み取り
    fread( &header, sizeof(header), 1, fp );

    // ファイルチェック
    if ( header.bfType != 0x4d42 )
    {
        std::cerr << "Error : Invalid File";
        fclose(fp);
        return false;
    }

    // ヘッダー情報の読み取り
    fread( &infoHeader, sizeof(infoHeader), 1, fp );

    if ( infoHeader.biSizeImage == 0 )
    { infoHeader.biSizeImage = infoHeader.biWidth * infoHeader.biHeight * 3; }

    // 進める.
    fseek( fp, header.bfOffBits, SEEK_SET );

    //　データサイズを決定し，メモリを確保
    m_ImageSize  = infoHeader.biSizeImage;
    m_pImageData = new(std::nothrow) unsigned char [m_ImageSize];
    if ( m_pImageData == nullptr )
    {
        std::cerr << "Error : Memory Allocate Failed." << std::endl;
        fclose( fp );
        return false;
    }

    // データを設定.
    m_Width          = infoHeader.biWidth;
    m_Height         = infoHeader.biHeight;
    m_BytePerPixel   = 3;
    m_Format         = GL_RGB;
    m_InternalFormat = GL_RGB;

    //　ピクセルデータの読み込み
    fread( m_pImageData, 1, infoHeader.biSizeImage, fp );

    //　BGR → RGBに変換
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
bool BmpImage::CreateGLTexture()
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
void BmpImage::DeleteGLTexture()
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
unsigned int BmpImage::GetID() const
{ return m_ID; }
