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

#pragma pack(push, 1)   // �p�f�B���O�ŃT�C�Y�������̂�h��.

/////////////////////////////////////////////////////////////////////////////////////////////
// TgaHeader structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct TgaHeader
{
    unsigned char   IDLength;           //!< ID�̒���.
    unsigned char   ColorMapType;       //!< �J���[�}�b�v�^�C�v.
    unsigned char   ImageType;          //!< �C���[�W�^�C�v.
    unsigned short  ColorMapOrigin;     //!< �ŏ��̃J���[�}�b�v�G���g���[�̃C���f�b�N�X.
    unsigned short  ColorMapLength;     //!< �J���[�}�b�v�G���g���[�̐�.
    unsigned char   ColorMapDepth;      //!< �e�J���[�}�b�v�G���g���[�̃r�b�g��.
    unsigned short  ImageOriginX;       //!< �摜�̌��_(X����).
    unsigned short  ImageOriginY;       //!< �摜�̌��_(Y����).
    unsigned short  ImageWidth;         //!< �摜�̉���.
    unsigned short  ImageHeight;        //!< �摜�̏c��.
    unsigned char   BitPerPixel;        //!< 1�s�N�Z��������̃r�b�g��.
    unsigned char   Descriptor;         //!< �L�q�q.
};

#pragma pack(pop)

} // namespace /* anonymous */


/////////////////////////////////////////////////////////////////////////////////////////////
// TgaImage class
/////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
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
//      �f�X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------
TgaImage::~TgaImage()
{ Release(); }

//-------------------------------------------------------------------------------------------
//      ����������s���܂�.
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
//�@�@ReadTGA
//�@�@Desc : TGA�t�@�C���̓ǂݍ���
//-----------------------------------------------------------------------------------------------------
bool TgaImage::Load(const char *filename)
{
    FILE *fp;
    TgaHeader header;

    //�@�t�@�C�����J��
    errno_t err = fopen_s( &fp, filename, "rb");
    if ( err != 0 )
    {
        std::cerr << "Error : File Open Failed" << std::endl;
        std::cerr << "File Name : " << filename << std::endl;
        return false;
    }

    //�@�w�b�_�[���̓ǂݍ���
    fread(&header, sizeof(header), 1, fp);

    //�@���ƍ��������߂�
    m_Width  = header.ImageWidth;
    m_Height = header.ImageHeight;

    //�@24 bit
    if ( header.BitPerPixel == 24 )
    {
        m_Format = GL_RGB;
        m_InternalFormat = GL_RGB;
    }
    //�@32 bit
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

    // 1�s�N�Z��������̃o�C�g��.
    m_BytePerPixel = header.BitPerPixel / 8;

    //�@�f�[�^�T�C�Y�̌���
    m_ImageSize = m_Width * m_Height * m_BytePerPixel;

    //�@���������m��
    m_pImageData = new(std::nothrow) unsigned char[ m_ImageSize ];
    if ( m_pImageData == nullptr )
    {
        std::cerr << "Error : Memory Allocacte Failed." << std::endl;
        fclose( fp );
        return false;
    }

    //�@�e�N�Z���f�[�^����C�ɓǂݎ��
    fread( m_pImageData, sizeof(unsigned char), m_ImageSize, fp );

    //�@BGR(A)��RGB(A)�ɃR���o�[�g
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
bool TgaImage::CreateGLTexture()
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
void TgaImage::DeleteGLTexture()
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
unsigned int TgaImage::GetID() const
{ return m_ID; }
