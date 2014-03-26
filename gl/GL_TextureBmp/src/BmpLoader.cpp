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
//      �R���X�g���N�^�ł�.
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
//      �f�X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------
BmpImage::~BmpImage()
{ Release(); }

//-------------------------------------------------------------------------------------------
//      ����������s���܂�.
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
//      �ǂݍ��ݏ������s���܂�.
//-------------------------------------------------------------------------------------------
bool BmpImage::Load(const char *filename)
{
    FILE *fp;

    BmpInfoHeader infoHeader;
    BmpFileHeader header;

    // �t�@�C�����J��
    errno_t err = fopen_s( &fp, filename, "rb" );
    if ( err != 0 )
    {
        std::cerr << "Error : File Open Failed.";
        std::cerr << "File Name : " << filename << std::endl;
        return false;
    }

    // �w�b�_�[���̓ǂݎ��
    fread( &header, sizeof(header), 1, fp );

    // �t�@�C���`�F�b�N
    if ( header.bfType != 0x4d42 )
    {
        std::cerr << "Error : Invalid File";
        fclose(fp);
        return false;
    }

    // �w�b�_�[���̓ǂݎ��
    fread( &infoHeader, sizeof(infoHeader), 1, fp );

    if ( infoHeader.biSizeImage == 0 )
    { infoHeader.biSizeImage = infoHeader.biWidth * infoHeader.biHeight * 3; }

    // �i�߂�.
    fseek( fp, header.bfOffBits, SEEK_SET );

    //�@�f�[�^�T�C�Y�����肵�C���������m��
    m_ImageSize  = infoHeader.biSizeImage;
    m_pImageData = new(std::nothrow) unsigned char [m_ImageSize];
    if ( m_pImageData == nullptr )
    {
        std::cerr << "Error : Memory Allocate Failed." << std::endl;
        fclose( fp );
        return false;
    }

    // �f�[�^��ݒ�.
    m_Width          = infoHeader.biWidth;
    m_Height         = infoHeader.biHeight;
    m_BytePerPixel   = 3;
    m_Format         = GL_RGB;
    m_InternalFormat = GL_RGB;

    //�@�s�N�Z���f�[�^�̓ǂݍ���
    fread( m_pImageData, 1, infoHeader.biSizeImage, fp );

    //�@BGR �� RGB�ɕϊ�
    for ( unsigned int i=0; i<m_ImageSize; i+=m_BytePerPixel )
    {
        unsigned char    temp = m_pImageData[ i + 0 ];
        m_pImageData[ i + 0 ] = m_pImageData[ i + 2 ];
        m_pImageData[ i + 2 ] = temp;
    }

    //�@�t�@�C�������
    fclose(fp);

    // ����I��.
    return true;
}

//-------------------------------------------------------------------------------------------
//      �e�N�X�`���𐶐����܂�.
//-------------------------------------------------------------------------------------------
bool BmpImage::CreateGLTexture()
{
    if ( m_pImageData == nullptr )
    { return false; }

    //�@�e�N�X�`���𐶐�
    glGenTextures(1, &m_ID);

    //�@�e�N�X�`�����o�C���h����
    glBindTexture(GL_TEXTURE_2D, m_ID);

    if ( m_BytePerPixel == 4 )
    { glPixelStorei(GL_UNPACK_ALIGNMENT, 4); }
    else 
    { glPixelStorei(GL_UNPACK_ALIGNMENT, 1); }

    //�@�e�N�X�`���̊��蓖��
    gluBuild2DMipmaps(
        GL_TEXTURE_2D,
        m_InternalFormat,
        m_Width,
        m_Height,
        m_Format,
        GL_UNSIGNED_BYTE,
        m_pImageData );

    //�@�e�N�X�`�����g��E�k��������@�̎w��
    glTexParameteri(GL_TEXTURE_2D, 	GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, 	GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    //�@�e�N�X�`����
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // �A���o�C���h���Ă���.
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

//-------------------------------------------------------------------------------------------
//      �e�N�X�`����j�����܂�.
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
//      �e�N�X�`��ID���擾���܂�.
//-------------------------------------------------------------------------------------------
unsigned int BmpImage::GetID() const
{ return m_ID; }
