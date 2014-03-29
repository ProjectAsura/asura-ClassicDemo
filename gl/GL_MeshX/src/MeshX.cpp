//------------------------------------------------------------------------------------------
// File : MeshX.cpp
// Desc : X File Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include <MeshX.h>
#include <cstdio>
#include <GL/freeglut.h>


#ifndef ELOG
#define ELOG( x, ... )  fprintf_s( stderr, "[File:%s, Line:%d] "x"\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#endif//ELOG

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY( x )  { if ( x ) { delete[] (x); (x) = nullptr; } }
#endif//SAFE_DELETE_ARRAY


namespace /* anonymous */ {

////////////////////////////////////////////////////////////////////////////////////////////
// Token class
////////////////////////////////////////////////////////////////////////////////////////////
class Token
{
    //======================================================================================
    // list of friend classes and methods.
    //======================================================================================
    /* NOTHING */

public:
    //======================================================================================
    // public variables.
    //======================================================================================
    /* NOTHING */

    //======================================================================================
    // public methods.
    //======================================================================================

    //--------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //--------------------------------------------------------------------------------------
    Token( char* pHead )
    : m_pPtr ( pHead )
    { memset( m_Token, 0, sizeof(char) * 2048 ); }

    //--------------------------------------------------------------------------------------
    //! @brief      トークンが有効であるかチェックします.
    //--------------------------------------------------------------------------------------
    bool IsValid( const char* token )
    { return ( 0 == strcmp( m_Token, token ) ); }

    //--------------------------------------------------------------------------------------
    //! @brief      次のトークンを取得します.
    //--------------------------------------------------------------------------------------
    void GetNext()
    {
        char* p = m_pPtr;
        char* q = m_Token;

        while( (*p) != '\0' && strchr( " \t\r\n,;\"", (*p) ) )
        { p++; }

        if ( (*p) == '{' || (*p) == '}' )
        { (*q++) = (*p++); }
        else
        {
            while( (*p) != '\0' && !strchr( " \t\r\n,;\"{}", (*p) ) )
            { (*q++) = (*p++); }
        }

        m_pPtr = p;
        (*q) = '\0';
    }

    //-------------------------------------------------------------------------------------
    //! @brief      次のトークンが有効であるかチェックします.
    //-------------------------------------------------------------------------------------
    bool IsNextValid( const char* token )
    {
        GetNext();
        if ( 0 != strcmp( m_Token, token ) )
        {
            printf_s( "Error : Invalid Token.\n" );
            printf_s( "    Expect Token : [%s]\n", token );
            printf_s( "    Readed Token : [%s]\n", m_Token );
            return false;
        }
        return true;
    }

    //-------------------------------------------------------------------------------------
    //! @brief      トークンをfloatとして取得します.
    //-------------------------------------------------------------------------------------
    float GetAsFloat()
    { return (float)atof( m_Token ); }

    //-------------------------------------------------------------------------------------
    //! @brief      トークンをintとして取得します.
    //-------------------------------------------------------------------------------------
    int GetAsInt()
    { return (int)atoi( m_Token ); }

    //-------------------------------------------------------------------------------------
    //! @brief      トークンをstd::stringとして取得します.
    //-------------------------------------------------------------------------------------
    std::string GetAsString()
    {
        char temp[256];
        strcpy_s( temp, m_Token );
        return std::string(temp);
    }

    //-------------------------------------------------------------------------------------
    //! @brief      次のトークンをfloatとして取得します.
    //-------------------------------------------------------------------------------------
    float GetNextAsFloat()
    { 
        GetNext();
        return GetAsFloat();
    }

    //-------------------------------------------------------------------------------------
    //! @brief      次のトークンをintとして取得します.
    //-------------------------------------------------------------------------------------
    int GetNextAsInt()
    {
        GetNext();
        return GetAsInt();
    }

    //-------------------------------------------------------------------------------------
    //! @brief      次のトークンをVec2として取得します.
    //-------------------------------------------------------------------------------------
    Vec2 GetNextAsVec2()
    {
        float x = GetNextAsFloat();
        float y = GetNextAsFloat();
        return Vec2( x, y );
    }

    //-------------------------------------------------------------------------------------
    //! @brief      次のトークンをVec3として取得します.
    //-------------------------------------------------------------------------------------
    Vec3 GetNextAsVec3()
    {
        float x = GetNextAsFloat();
        float y = GetNextAsFloat();
        float z = GetNextAsFloat();
        return Vec3( x, y, z );
    }

    //-------------------------------------------------------------------------------------
    //! @brief      次のトークンをVec3として取得します.
    //-------------------------------------------------------------------------------------
    Vec4 GetNextAsVec4()
    {
        float x = GetNextAsFloat();
        float y = GetNextAsFloat();
        float z = GetNextAsFloat();
        float w = GetNextAsFloat();
        return Vec4( x, y, z, w );
    }

    //-------------------------------------------------------------------------------------
    //! @brief      次のトークンをstd::stringとして取得します.
    //-------------------------------------------------------------------------------------
    std::string GetNextAsString()
    {
        GetNext();
        return GetAsString();
    }

    //-------------------------------------------------------------------------------------
    //! @brief      ノードをスキップします.
    //-------------------------------------------------------------------------------------
    bool SkipNode()
    {
        while( (*m_pPtr) != '\0' )
        {
            GetNext();
            if ( strchr( m_Token, '{' ) )
            { break; }
        }

        int count = 1;

        while( (*m_pPtr) != '\0' && count > 0 )
        {
            GetNext();
            if ( strchr( m_Token, '{' ) )
            { count++; }
            else if ( strchr( m_Token, '}' ) )
            { count--; }
        }

        if ( count > 0 )
        {
            printf_s( "Error : Block Not Match.\n" );
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------------------
    //! @brief      空であるかどうかチェックします.
    //-------------------------------------------------------------------------------------
    bool IsEmpty()
    {
        if ( m_pPtr == nullptr )
        { return true; }

        if ( (*m_pPtr) == '\0' )
        { return true; }

        return false;
    }

protected:
    //=====================================================================================
    // protected variables.
    //=====================================================================================
    /* NOTHING */

    //=====================================================================================
    // protected methods.
    //=====================================================================================
    /* NOTHING */

private:
    //=====================================================================================
    // private variables.
    //=====================================================================================
    char*   m_pPtr;
    char    m_Token[ 2048 ];

    //=====================================================================================
    // private methods.
    //=====================================================================================
    Token           ( const Token& value );     // アクセス禁止.
    void operator = ( const Token& value );     // アクセス禁止.
};


} // namespace /* anonymous */ 


///////////////////////////////////////////////////////////////////////////////////////////
// ModelX class
///////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------------------
ModelX::ModelX()
: m_Meshes      ()
, m_Materials   ()
, m_Box         ()
, m_Sphere      ()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------------------
//      コピーコンストラクタです.
//-----------------------------------------------------------------------------------------
ModelX::ModelX( const ModelX& value )
: m_Meshes      ( value.m_Meshes )
, m_Materials   ( value.m_Materials )
, m_Box         ( value.m_Box )
, m_Sphere      ( value.m_Sphere )
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------------------
ModelX::~ModelX()
{ Release(); }

//-----------------------------------------------------------------------------------------
//      解放処理を行います.
//-----------------------------------------------------------------------------------------
void ModelX::Release()
{
    m_Meshes   .clear();
    m_Materials.clear();
}

//-----------------------------------------------------------------------------------------
//      ファイルから読み込みします.
//-----------------------------------------------------------------------------------------
bool ModelX::LoadFromFile( const char* filename )
{
    // 引数のチェック.
    if ( filename == nullptr )
    {
        ELOG( "Error : Invalid Argument." );
        return false;
    }

    // ファイルを開く.
    FILE* pFile;
    errno_t err = fopen_s( &pFile, filename, "rb" );
    if ( err != 0 )
    {
        ELOG( "Error : File Open Failed." );
        return false;
    }

    // ファイルサイズを調べる.
    long curr = ftell( pFile );
    fseek( pFile, 0, SEEK_END );
    long end = ftell( pFile );
    fseek( pFile, curr, SEEK_SET );
    size_t size = end - curr;

    // メモリを一気に確保.
    char* pBuffer = new (std::nothrow) char [ size ];
    if ( pBuffer == nullptr )
    {
        ELOG( "Error : Memory Allocate Failed." );
        fclose( pFile );
        return false;
    }

    // 一気に読み込み.
    size_t readSize = fread( pBuffer, sizeof(char), size, pFile );

    // ファイルを閉じる.
    fclose( pFile );

    // 読み込みサイズをチェック.
    if ( readSize != size )
    {
        ELOG( "Error : Read Size Not Matched." );
        SAFE_DELETE_ARRAY( pBuffer );
        return false;
    }

    // トーカナイザーを用意.
    Token token( pBuffer );

    // メッシュのインデックス.
    int meshID = -1;

    // バッファ最後までループ.
    while( !token.IsEmpty() )
    {
        // トークン取得.
        token.GetNext();

        // テンプレートノード.
        if ( token.IsValid( "template") )
        {
            // ノードを読み飛ばす.
            token.SkipNode();
        }
        // メッシュノード.
        else if ( token.IsValid( "Mesh" ) )
        {
            // トークン取得.
            token.GetNext();

            // トークンが"{"出ない場合.
            if ( !token.IsValid( "{" ) )
            {
                /* この実装ではメッシュファイル名は使わない */

                token.IsNextValid( "{" );
            }

            // メッシュを追加.
            {
                MeshX mesh;
                m_Meshes.push_back( mesh );

                // メッシュ番号を更新.
                meshID++;
            }

            // 位置座標数を取得.
            int posCount = token.GetNextAsInt();

            // メモリを確保.
            m_Meshes[ meshID ].positions.resize( posCount );

            // 位置座標データを読み込む.
            for( int i=0; i<posCount; ++i )
            { m_Meshes[ meshID ].positions[i] = token.GetNextAsVec3(); }

            // 面数を取得.
            int faceCount = token.GetNextAsInt();

            // メモリを確保.
            m_Meshes[ meshID ].faces.resize( faceCount );

            // 面データを読み取る..
            for( int i=0; i<faceCount; ++i )
            {
                // 面の構成要素数を取得し，設定する.
                int element = token.GetNextAsInt();
                m_Meshes[ meshID ].faces[i].element = element;

                // インデックスを読み取る.
                for( int j=0; j<element; ++j )
                {
                    int index = token.GetNextAsInt();
                    m_Meshes[ meshID ].faces[ i ].indexP[ j ] = index;
                    m_Meshes[ meshID ].faces[ i ].indexU[ j ] = index;
                }
            }
        }
        // メッシュ法線ベクトルノード.
        else if ( token.IsValid( "MeshNormals" ) )
        {
            token.IsNextValid( "{" );

            // 法線ベクトルを数を取得.
            size_t normalCount = token.GetNextAsInt();

            // メモリを確保.
            m_Meshes[ meshID ].normals.resize( normalCount );

            // 法線ベクトルデータを読み取る.
            for( size_t i=0; i<normalCount; ++i )
            { m_Meshes[ meshID ].normals[ i ] = token.GetNextAsVec3(); }

            // 面数を取得.
            size_t faceCount = token.GetNextAsInt();

            // 面数が一致することを確認.
            if ( faceCount != m_Meshes[ meshID ].faces.size() )
            {
                // エラーログ出力.
                ELOG( "Error : Face Count Not Matched." );

                // メモリを解放.
                SAFE_DELETE_ARRAY( pBuffer );
                Release();

                // 異常終了.
                return false;
            }

            // 面データを読み取る.
            for( size_t i=0; i<faceCount; ++i )
            {
                // 面の構成要素数を取得.
                int element = token.GetNextAsInt();

                // 法線ベクトルインデックスを読み取る.
                for( int j=0; j<element; ++j )
                { m_Meshes[ meshID ].faces[ i ].indexN[ j ] = token.GetNextAsInt(); }
            }
        }
        // メッシュテクスチャ座標ノード.
        else if ( token.IsValid( "MeshTextureCoords" ) )
        {
            token.IsNextValid( "{" );

            // テクスチャ座標数を取得.
            size_t uvCount = token.GetNextAsInt();

            // メモリを確保.
            m_Meshes[ meshID ].texcoords.resize( uvCount );

            // テクスチャ座標データを読み取る.
            for( size_t i=0; i<uvCount; ++i )
            { m_Meshes[ meshID ].texcoords[i] = token.GetNextAsVec2(); }
        }
        // メッシュマテリアルリストノード.
        else if ( token.IsValid( "MeshMaterialList" ) )
        {
            token.IsNextValid( "{" );

            // マテリアル数を取得.
            size_t materialCount = token.GetNextAsInt();

            // メモリを確保.
            m_Materials.resize( materialCount );

            // 面数を取得.
            size_t faceCount = token.GetNextAsInt();

            // 面数が一致することを確認.
            if ( faceCount != m_Meshes[ meshID ].faces.size() )
            {
                // エラーログ出力.
                ELOG( "Error : Face Count Not Matched." );

                // メモリを解放.
                SAFE_DELETE_ARRAY( pBuffer );
                Release();

                // 異常終了.
                return false;
            }

            // マテリアルインデックスデータを読み取る.
            for( size_t i=0; i<faceCount; ++i )
            { m_Meshes[ meshID ].faces[ i ].indexM = token.GetNextAsInt(); }

            // マテリアルデータを読み取る.
            for( size_t i=0; i<materialCount; ++i )
            {
                token.IsNextValid("Material");
                token.GetNext();

                // マテリアル名を取得.
                if ( !token.IsValid( "{" ) )
                {
                    m_Materials[ i ].name = token.GetAsString();
                    token.IsNextValid( "{" );
                }
                else
                {
                    // 取得できない場合は適当な名前を付ける.
                    char temp[ 256 ];
                    sprintf_s( temp, "material_%u", i );
                    m_Materials[ i ].name = std::string(temp);
                }

                m_Materials[ i ].diffuse  = token.GetNextAsVec4();
                m_Materials[ i ].power    = token.GetNextAsFloat();
                m_Materials[ i ].specular = token.GetNextAsVec3();
                m_Materials[ i ].emissive = token.GetNextAsVec3();

                // テクスチャファイルデータのチェック.
                if ( !token.IsNextValid( "}" ) )
                {
                    if ( token.IsValid( "TextureFileName" ) )
                    {
                        token.IsNextValid( "{" );
                        m_Materials[ i ].texture = token.GetNextAsString();
                        token.IsNextValid( "}" ); // テクスチャファイル名の終わり.
                        token.IsNextValid( "}" ); // マテリアルの終わり.
                    }
                }
            }
        }
    }

    // 不要なメモリを解放.
    SAFE_DELETE_ARRAY( pBuffer );

    // 最適化.
    {
        for( size_t i=0; i<m_Meshes.size(); ++i )
        { m_Meshes[i].Optimize(); }

        m_Meshes   .shrink_to_fit();
        m_Materials.shrink_to_fit();
    }

    // バウンディングボックスとバウンディングをスフィアを求める.
    {
        m_Box = BoundingBox( m_Meshes[0].positions[0] );
        for( size_t i=0; i<m_Meshes.size(); ++i )
        {
            for( size_t j=0; j<m_Meshes[i].positions.size(); ++j )
            { m_Box.Merge( m_Meshes[i].positions[j] );  }
        }

        m_Sphere = BoundingSphere( m_Box );
    }

    // 正常終了.
    return true;
}

//-----------------------------------------------------------------------------------------
//      マテリアルを設定します.
//-----------------------------------------------------------------------------------------
void ModelX::SetMaterial( const Material& material )
{
    float ambient[4] = { 0.15f, 0.15f, 0.15f, 1.0f };
    glColor4fv  ( material.diffuse );
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,   ambient );
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   material.diffuse );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  material.specular );
    glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,  material.emissive );
    glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, material.power );
}

//-----------------------------------------------------------------------------------------
//      メッシュを描画します.
//-----------------------------------------------------------------------------------------
void ModelX::DrawMesh( unsigned int index )
{
    /* 簡易実装 */

    const MeshX& mesh = m_Meshes[ index ];
    bool hasM = ( !m_Materials.empty() );
    bool hasU = ( !mesh.texcoords.empty() );
    bool hasN = ( !mesh.normals.empty() );

    int prevMat = -1;
    int currMat = 0;

    for( size_t i=0; i<mesh.faces.size(); ++i )
    {
        const Face& face = mesh.faces[i];

        if ( hasM )
        {
            currMat = face.indexM;

            if ( currMat != prevMat )
            {
                SetMaterial( m_Materials[currMat] );
                prevMat = currMat;
            }
        }

        int element = face.element;
        switch( element )
        {
            case 3: { glBegin( GL_TRIANGLES ); }  break;
            case 4: { glBegin( GL_QUADS );     }  break;
        }

        for( int j=0; j<element; ++j )
        {
            if ( hasU ) { glTexCoord2fv( mesh.texcoords[ face.indexU[ j ] ] ); }
            if ( hasN ) { glNormal3fv  ( mesh.normals  [ face.indexN[ j ] ] ); }
            glVertex3fv( mesh.positions[ face.indexP[ j ] ] );
        }

        glEnd();
    }
}

//-----------------------------------------------------------------------------------------
//      モデルを描画します.
//-----------------------------------------------------------------------------------------
void ModelX::Draw()
{
    for( size_t i=0; i<m_Meshes.size(); ++i )
    { DrawMesh( i ); }
}

//-----------------------------------------------------------------------------------------
//      メッシュを取得します.
//-----------------------------------------------------------------------------------------
std::vector<MeshX>& ModelX::GetMeshes()
{ return m_Meshes; }

//-----------------------------------------------------------------------------------------
//      マテリアルを取得します.
//-----------------------------------------------------------------------------------------
std::vector<Material>& ModelX::GetMaterials()
{ return m_Materials; }

//-----------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-----------------------------------------------------------------------------------------
BoundingBox ModelX::GetBox() const
{ return m_Box; }

//-----------------------------------------------------------------------------------------
//      バウンディングスフィアを取得します.
//-----------------------------------------------------------------------------------------
BoundingSphere ModelX::GetSphere() const
{ return m_Sphere; }
