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
//      �R���X�g���N�^�ł�.
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
//      �f�X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------
RawImage::~RawImage()
{ Release(); }

//-------------------------------------------------------------------------------------------
//      ����������s���܂�.
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
//      �e�N�X�`����ǂݍ��݂��܂�.
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

    // �t�@�C�����J��.
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

    // �������m��.
    m_pImageData = new(std::nothrow) unsigned char [m_ImageSize];
    if ( m_pImageData == nullptr )
    {
        std::cerr << "Error : Memory Allocate Faied." << std::endl;
        return false;
    }

    // �ꊇ�ǂݍ���.
    file.read((char*)m_pImageData, m_ImageSize);

    // �t�@�C�������.
    file.close();

    // ����I��.
    return true;
}

//-------------------------------------------------------------------------------------------
//      �e�N�X�`���𐶐����܂�.
//-------------------------------------------------------------------------------------------
bool RawImage::CreateGLTexture()
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
void RawImage::DeleteGLTexture()
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
unsigned int RawImage::GetID() const
{ return m_ID; }
