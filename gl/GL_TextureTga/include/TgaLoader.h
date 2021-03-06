﻿//-------------------------------------------------------------------------------------------
// File : TgaLoader.h
// Desc : Targa Texture Loader.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

#ifndef _TGA_LOADER_H_
#define _TGA_LOADER_H_


/////////////////////////////////////////////////////////////////////////////////////////////
// TgaImage class
/////////////////////////////////////////////////////////////////////////////////////////////
class TgaImage
{
    //=======================================================================================
    // list of friend classes and methods.
    //=======================================================================================
    /* NOTHING */

public:
    //=======================================================================================
    // public variables.
    //=======================================================================================
    /* NOTHING */

    //=======================================================================================
    // public methods.
    //=======================================================================================

    //---------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------
    TgaImage();

    //---------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------
    virtual ~TgaImage();

    //---------------------------------------------------------------------------------------
    //! @brief      テクスチャを読み込みします.
    //!
    //! @param [in]     filename        ファイル名です.
    //! @retval true    読み込みに成功.
    //! @retval false   読み込みに失敗.
    //---------------------------------------------------------------------------------------
    bool Load( const char* filename );
   
    //---------------------------------------------------------------------------------------
    //! @brief      GLテクスチャを生成します.
    //---------------------------------------------------------------------------------------
    bool CreateGLTexture();

    //---------------------------------------------------------------------------------------
    //! @brief      GLテクスチャを破棄します.
    //---------------------------------------------------------------------------------------
    void DeleteGLTexture();

    //---------------------------------------------------------------------------------------
    //! @brief      解放処理を行います.
    //---------------------------------------------------------------------------------------
    void Release();

    //---------------------------------------------------------------------------------------
    //! @brief      テクスチャIDを取得します.
    //---------------------------------------------------------------------------------------
    unsigned int GetID() const;

protected:
    //=======================================================================================
    // protected variables.
    //=======================================================================================
    unsigned int    m_ImageSize;        //!< ピクセルサイズです.
    unsigned int    m_Format;           //!< フォーマットです.
    unsigned int    m_InternalFormat;   //!< 内部フォーマットです.
    unsigned int    m_Width;            //!< 画像の横幅です.
    unsigned int    m_Height;           //!< 画像の縦幅です.
    unsigned int    m_BytePerPixel;     //!< 1ピクセルあたりのバイト数です.
    unsigned int    m_ID;               //!< テクスチャIDです.
    unsigned char*  m_pImageData;       //!< ピクセルデータです.

    //=======================================================================================
    // protected methods.
    //=======================================================================================
    /* NOTHING */

private:
    //=======================================================================================
    // private variables.
    //=======================================================================================
    /* NOTHING */

    //=======================================================================================
    // private methods.
    //=======================================================================================
    TgaImage        ( const TgaImage& value );     // アクセス禁止.
    void operator = ( const TgaImage& value );     // アクセス禁止.
};


#endif //_TGA_LOADER_H_
