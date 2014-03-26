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
#include <DdsLoader.h>
#include <GL/glew.h>
#include <GL/glut.h>


#ifndef DLOG
#define DLOG( x, ... )  printf_s( x, ##__VA_VARGS__ )
#endif//DLOG

#ifndef ELOG
#define ELOG( x, ... )  fprintf_s( stderr, "[File : %s, Line : %d] "x"\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#endif//ELOG


namespace /* anonymous */ {

//------------------------------------------------------------------------------------------
// Constant Values
//------------------------------------------------------------------------------------------

// dwFlags Value
static const unsigned int DDSD_CAPS         = 0x00000001;   // dwCaps/dwCaps2���L��.
static const unsigned int DDSD_HEIGHT       = 0x00000002;   // dwHeight���L��.
static const unsigned int DDSD_WIDTH        = 0x00000004;   // dwWidth���L��.
static const unsigned int DDSD_PITCH        = 0x00000008;   // dwPitchOrLinearSize��Pitch��\��.
static const unsigned int DDSD_PIXELFORMAT  = 0x00001000;   // dwPfSize/dwPfFlags/dwRGB�`���̒��ڒ�`���L��.
static const unsigned int DDSD_MIPMAPCOUNT  = 0x00020000;   // dwMipMapCount���L��.
static const unsigned int DDSD_LINEARSIZE   = 0x00080000;   // dwPitchOrLinearSize��LinerSize��\��.
static const unsigned int DDSD_DEPTH        = 0x00800000;   // dwDepth���L��.

// dwPfFlags Value
static const unsigned int DDPF_ALPHAPIXELS      = 0x00000001;   // RGB�ȊO��alpha���܂܂�Ă���.
static const unsigned int DDPF_ALPHA            = 0x00000002;   // pixel��Alpha�����̂�.
static const unsigned int DDPF_FOURCC           = 0x00000004;   // dwFourCC���L��.
static const unsigned int DDPF_PALETTE_INDEXED4 = 0x00000008;   // Palette 16 colors.
static const unsigned int DDPF_PALETTE_INDEXED8 = 0x00000020;   // Palette 256 colors.
static const unsigned int DDPF_RGB              = 0x00000040;   // dwRGBBitCount/dwRBitMask/dwGBitMask/dwBBitMask/dwRGBAlphaBitMask�ɂ���ăt�H�[�}�b�g����`����Ă��邱�Ƃ�����.
static const unsigned int DDPF_LUMINANCE        = 0x00020000;   // 1ch�̃f�[�^��RGB�S�ĂɓW�J�����.
static const unsigned int DDPF_BUMPDUDV         = 0x00080000;   // pixel�������t���ł��邱�Ƃ�����.

// dwCaps Value
static const unsigned int DDSCAPS_ALPHA     = 0x00000002;       // Alpha���܂܂�Ă���ꍇ.
static const unsigned int DDSCAPS_COMPLEX   = 0x00000008;       // �����̃f�[�^���܂܂�Ă���ꍇPalette/Mipmap/Cube/Volume��.
static const unsigned int DDSCAPS_TEXTURE   = 0x00001000;       // ���1.
static const unsigned int DDSCAPS_MIPMAP    = 0x00400000;       // MipMap�����݂���ꍇ.

// dwCaps2 Value
static const unsigned int DDSCAPS2_CUBEMAP              = 0x00000200;   // CubeMap�����݂���ꍇ.
static const unsigned int DDSCAPS2_CUBEMAP_POSITIVE_X   = 0x00000400;   // CubeMap X+
static const unsigned int DDSCAPS2_CUBEMAP_NEGATIVE_X   = 0x00000800;   // CubeMap X-
static const unsigned int DDSCAPS2_CUBEMAP_POSITIVE_Y   = 0x00001000;   // CubeMap Y+
static const unsigned int DDSCAPS2_CUBEMAP_NEGATIVE_Y   = 0x00002000;   // CubeMap Y-
static const unsigned int DDSCAPS2_CUBEMAP_POSITIVE_Z   = 0x00004000;   // CubeMap Z+
static const unsigned int DDSCAPS2_CUBEMAP_NEGATIVE_Z   = 0x00008000;   // CubeMap Z-
static const unsigned int DDSCAPS2_VOLUME               = 0x00400000;   // VolumeTexture�̏ꍇ.

// dwFourCC Value
static const unsigned int FOURCC_DXT1           = '1TXD';           // DXT1
static const unsigned int FOURCC_DXT2           = '2TXD';           // DXT2
static const unsigned int FOURCC_DXT3           = '3TXD';           // DXT3
static const unsigned int FOURCC_DXT4           = '4TXD';           // DXT4
static const unsigned int FOURCC_DXT5           = '5TXD';           // DXT5
static const unsigned int FOURCC_ATI1           = '1ITA';           // 3Dc ATI2
static const unsigned int FOURCC_ATI2           = '2ITA';           // 3Dc ATI2
static const unsigned int FOURCC_DX10           = '01XD';           // DX10
static const unsigned int FOURCC_BC4U           = 'U4CB';           // BC4U
static const unsigned int FOURCC_BC4S           = 'S4CB';           // BC4S
static const unsigned int FOURCC_BC5U           = 'U5CB';           // BC5U
static const unsigned int FOURCC_BC5S           = 'S5CB';           // BC5S
static const unsigned int FOURCC_A16B16G16R16   = 0x00000024;
static const unsigned int FOURCC_Q16W16V16U16   = 0x0000006e;
static const unsigned int FOURCC_R16F           = 0x0000006f;
static const unsigned int FOURCC_G16R16F        = 0x00000070;
static const unsigned int FOURCC_A16B16G16R16F  = 0x00000071;
static const unsigned int FOURCC_R32F           = 0x00000072;
static const unsigned int FOURCC_G32R32F        = 0x00000073;
static const unsigned int FOURCC_A32B32G32R32F  = 0x00000074;
static const unsigned int FOURCC_CxV8U8         = 0x00000075;
static const unsigned int FOURCC_Q8W8V8U8       = 0x0000003f;



////////////////////////////////////////////////////////////////////////////////////////////
// DDPixelFormat structure
////////////////////////////////////////////////////////////////////////////////////////////
typedef struct __DDPixelFormat
{
    unsigned int    size;
    unsigned int    flags;
    unsigned int    fourCC;
    unsigned int    bpp;
    unsigned int    maskR;
    unsigned int    maskG;
    unsigned int    maskB;
    unsigned int    maskA;
} DDPixelFormat;


////////////////////////////////////////////////////////////////////////////////////////////
// DDColorKey structure
////////////////////////////////////////////////////////////////////////////////////////////
typedef struct __DDColorKey
{
    unsigned int    low;
    unsigned int    hight;
} DDColorKey;


////////////////////////////////////////////////////////////////////////////////////////////
// DDSurfaceDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
typedef struct __DDSurfaceDesc
{
    unsigned int    size;
    unsigned int    flags;
    unsigned int    height;
    unsigned int    width;
    unsigned int    pitch;
    unsigned int    depth;
    unsigned int    mipMapLevels;
    unsigned int    alphaBitDepth;
    unsigned int    reserved;
    unsigned int    surface;

    DDColorKey      ckDestOverlay;
    DDColorKey      ckDestBit;
    DDColorKey      ckSrcOverlay;
    DDColorKey      ckSrcBit;

    DDPixelFormat   pixelFormat;
    unsigned int    caps;
    unsigned int    caps2;
    unsigned int    reservedCaps[ 2 ];

    unsigned int    textureStage;
} DDSurfaceDesc;



} // namespace /* anonymous */


/////////////////////////////////////////////////////////////////////////////////////////////
// DdsImage class
/////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------
DdsImage::DdsImage()
: m_ImageSize       ( 0 )
, m_Format          ( 0 )
, m_InternalFormat  ( 0 )
, m_Width           ( 0 )
, m_Height          ( 0 )
, m_BytePerPixel    ( 0 )
, m_ID              ( 0 )
, m_pImageData      ( nullptr )
, m_MipmapCount     ( 0 )
{ /* DO_NOTHING */ }


//-------------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------
DdsImage::~DdsImage()
{ Release(); }

//-------------------------------------------------------------------------------------------
//      ����������s���܂�.
//-------------------------------------------------------------------------------------------
void DdsImage::Release()
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
    m_MipmapCount    = 0;
}

//-------------------------------------------------------------------------------------------
//      �ǂݍ��ݏ������s���܂�.
//-------------------------------------------------------------------------------------------
bool DdsImage::Load(const char *filename)
{
    FILE* fp;

    // �t�@�C�����J��.
    errno_t err = fopen_s( &fp, filename, "rb" );
    if ( err != 0 )
    {
        ELOG( "Error : File Open Failed. FileName = %s", filename );
        return false;
    }

    // �t�@�C���}�W�b�N���`�F�b�N.
    char magic[4];
    fread( &magic, sizeof(char), 4, fp );
    if ( ( magic[0] != 'D' )
      || ( magic[1] != 'D' )
      || ( magic[2] != 'S' )
      || ( magic[3] != ' ' ) )
    {
        ELOG( "Error : Invalid File Format." );
        fclose( fp );
        return false;
    }

    // �T�[�t�F�C�X�f�X�N���v�^�[��ǂݍ���.
    DDSurfaceDesc ddsd;
    fread( &ddsd, sizeof(ddsd), 1, fp );

    // �T�C�Y��ݒ�.
    m_Width       = ddsd.width;
    m_Height      = ddsd.height;
    m_MipmapCount = 1;

    if ( ddsd.flags & DDSD_MIPMAPCOUNT )
    { m_MipmapCount = ddsd.mipMapLevels; }

    // �L���[�u�}�b�v�ƃ{�����[���e�N�X�`���̔�����ꉞ�s���ĊY������ꍇ�͒e��.
    if ( ddsd.caps & DDSCAPS_COMPLEX )
    {
        if ( ddsd.caps2 & DDSCAPS2_CUBEMAP )
        {
            ELOG( "Error : Cubemap Not Support." );
            fclose( fp );
            return false;
        }
        else if ( ddsd.caps2 & DDSCAPS2_VOLUME )
        {
            ELOG( "Error : Volume Texture Not Support." );
            fclose( fp );
            return false;
        }
    }

    bool isFind = false;

    // BC���k�̃^�C�v�𒲂ׂ�.
    if ( ddsd.flags & DDSD_PIXELFORMAT )
    {
        if ( ddsd.pixelFormat.flags & DDPF_FOURCC )
        {
            switch( ddsd.pixelFormat.fourCC )
            {
            case FOURCC_DXT1:
                {
                    isFind = true;
                    m_Format         = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                    m_InternalFormat = COMPRESS_TYPE_DXT1;
                    m_Format         = GL_RGBA;
                    m_BytePerPixel   = 8;
                }
                break;

            case FOURCC_DXT2:
                {
                    isFind = true;
                    m_Format         = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                    m_InternalFormat = COMPRESS_TYPE_DXT1;
                    m_Format         = GL_RGBA;
                    m_BytePerPixel   = 16;
                }
                break;

            case FOURCC_DXT3:
                {
                    isFind = true;
                    m_Format         = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                    m_InternalFormat = COMPRESS_TYPE_DXT3;
                    m_Format         = GL_RGB;
                    m_BytePerPixel   = 16;
                }
                break;

            case FOURCC_DXT4:
                {
                    isFind = true;
                    m_Format         = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                    m_InternalFormat = COMPRESS_TYPE_DXT3;
                    m_Format         = GL_RGBA;
                    m_BytePerPixel   = 16;
                }
                break;

            case FOURCC_DXT5:
                {
                    isFind = true;
                    m_Format         = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                    m_InternalFormat = COMPRESS_TYPE_DXT5;
                    m_Format         = GL_RGB;
                    m_BytePerPixel   = 16;
                }
                break;

            default:
                {
                    ELOG( "Error : Unsupported Format. Please use DXT1, DXT3, DXT5 format." );
                    fclose( fp );
                    return false;
                }
            }
        }
    }

    if ( !isFind )
    {
        ELOG( "Error : Invalid Format." );
        fclose( fp );
        return false;
    }

    long curr = 0;
    long end  = 0;

    curr = ftell( fp );
    fseek( fp, 0, SEEK_END );
    end  = ftell( fp );
    fseek( fp, curr, SEEK_SET );

    m_ImageSize = end - curr;
    m_pImageData = new (std::nothrow) unsigned char [ m_ImageSize ];
    if ( m_pImageData == nullptr )
    {
        ELOG( "Error : Memory Allocate Failed." );
        fclose( fp );
        return false;
    }

    // ��C�ɓǂݍ���.
    fread( m_pImageData, sizeof(unsigned char), m_ImageSize, fp );

    // �t�@�C�������.
    fclose( fp );

    // ����I��.
    return true;
}

//-------------------------------------------------------------------------------------------
//      �e�N�X�`���𐶐����܂�.
//-------------------------------------------------------------------------------------------
bool DdsImage::CreateGLTexture()
{
    if ( m_pImageData == nullptr )
    { return false; }

    if ( glewInit() != GLEW_OK )
    {
        ELOG( "Error : GLEW Initialize Failed." );
        return false;
    }

    //�@�e�N�X�`���𐶐�
    glGenTextures(1, &m_ID);

    //�@�e�N�X�`�����o�C���h����
    glBindTexture(GL_TEXTURE_2D, m_ID);

    //�@�g��E�k��������@�̎w��
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    //�@�e�N�X�`�����̐ݒ�
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    //�@�𓀍��
    DecompressBC();

    // �A���o�C���h���Ă���.
    glBindTexture(GL_TEXTURE_2D, 0);

    // ����I��.
    return true;
}

//-------------------------------------------------------------------------------------------
//      �u���b�N���k���𓀂��܂�.
//-------------------------------------------------------------------------------------------
void DdsImage::DecompressBC()
{
    int blockSize = 0;
    int offset    = 0;
    int size      = 0;

    int w = m_Width;
    int h = m_Height;

    //�@DXT1
    if ( m_Format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )
    { blockSize = 8; }
    //�@DXT3, DXT5
    else
    { blockSize = 16; }

    //�@��
    for ( unsigned int i=0; i<m_MipmapCount; i++ )
    {
        size = ( (w+3)/4 ) * ( (h+3)/4 ) * blockSize;
        glCompressedTexImage2D( GL_TEXTURE_2D, int(i), m_Format, w, h, 0, size, m_pImageData + offset );

        w = ( w >> 1 );
        h = ( h >> 1 );
        if ( w < 1 ) { w = 1; }
        if ( h < 1 ) { h = 1; }

        offset += size;
    }
}


//-------------------------------------------------------------------------------------------
//      �e�N�X�`����j�����܂�.
//-------------------------------------------------------------------------------------------
void DdsImage::DeleteGLTexture()
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
unsigned int DdsImage::GetID() const
{ return m_ID; }
