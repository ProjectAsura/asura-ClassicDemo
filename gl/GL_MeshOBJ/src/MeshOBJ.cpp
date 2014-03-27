//-------------------------------------------------------------------------------------------
// File : MeshOBJ.cpp
// Desc : Wavefront Object File Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <MeshOBJ.h>
#include <cstring>
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>


//-------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY( x ) { if ( x ) { delete[] (x); (x) = nullptr; } }
#endif//SAFE_DELETE_ARRAY


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------
//      マテリアルの初期化を行います.
//-------------------------------------------------------------------------------------------
void InitMaterial( Material& material )
{
    memset( &material, 0, sizeof( material ) );
    material.ambient   = Vec3( 0.2f, 0.2f, 0.2f );
    material.diffuse   = Vec3( 0.8f, 0.8f, 0.8f );
    material.specular  = Vec3( 1.0f, 1.0f, 1.0f );
    material.shininess = 0.0f;
    material.alpha     = 1.0f;
}

//-------------------------------------------------------------------------------------------
//      マテリアルを設定します.
//-------------------------------------------------------------------------------------------
void SetMaterial( const Material& material )
{
    glColor4f( 
        material.diffuse.x,
        material.diffuse.y,
        material.diffuse.z,
        material.alpha );
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,    material.ambient );
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,    material.diffuse );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,   material.specular );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, &material.shininess );
}

} // namespace /* anonymous */


/////////////////////////////////////////////////////////////////////////////////////////////
// MeshOBJ class
/////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------
//      コンストラクタです.
//-------------------------------------------------------------------------------------------
MeshOBJ::MeshOBJ()
: m_Vertices    ()
, m_Subsets     ()
, m_Materials   ()
, m_Indices     ()
, m_Box         ()
, m_Sphere      ()
{ /* DO_NOTHING */ }


//-------------------------------------------------------------------------------------------
//      デストラクタです.
//-------------------------------------------------------------------------------------------
MeshOBJ::~MeshOBJ()
{ Release(); }


//-------------------------------------------------------------------------------------------
//      メモリを解放します.
//-------------------------------------------------------------------------------------------
void MeshOBJ::Release()
{
    m_Vertices .clear();
    m_Subsets  .clear();
    m_Materials.clear();
    m_Indices  .clear();
}

//-------------------------------------------------------------------------------------------
//      OBJファイルから読み込み処理を行います.
//-------------------------------------------------------------------------------------------
bool MeshOBJ::LoadOBJFile( const char *filename )
{
    std::ifstream file;

    std::string materialFile;

    const int BUFFER_LENGTH = 2048;
    char buf[BUFFER_LENGTH] = { 0 };
    std::vector<Vec3>   positions;
    std::vector<Vec3>   normals;
    std::vector<Vec2>   texcoords;
    bool initBox  = false;
    int  prevSize = 0;

    unsigned int total     = 0;
    unsigned int faceIndex = 0;
    unsigned int faceCount = 0;
    std::string  curMaterial;

    //　ファイルを開く
    file.open( filename, std::ios::in );

    //　チェック
    if ( !file.is_open() )
    {
        std::cerr << "Error : ファイルオープンに失敗\n";
        std::cerr << "File Name : " << filename << std::endl;
        return false;
    }

    //　ループ
    for( ;; )
    {
        file >> buf;
        if ( !file )
            break;

        //　コメント
        if ( 0 == strcmp( buf, "#" ) )
        { continue; }

        //　頂点座標
        else if ( 0 == strcmp( buf, "v" ) )
        {
            float x, y, z;
            file >> x >> y >> z;
            Vec3 v( x, y, z );
            positions.push_back( v );

            //　バウンディングボックスの初期化
            if ( !initBox )
            {
                m_Box = BoundingBox( v );
                initBox = true;
            }

            //　バウンディングボックスの算出
            m_Box.Merge( v );
        }

        //　テクスチャ座標
        else if ( 0 == strcmp( buf, "vt" ) )
        {
            float u, v;
            file >> u >> v;
            texcoords.push_back( Vec2( u, v ) );
        }

        //　法線ベクトル
        else if ( 0 == strcmp( buf, "vn" ) )
        {
            float x, y, z;
            file >> x >> y >> z;
            normals.push_back( Vec3( x, y, z) );
        }

        //　面
        else if ( 0 == strcmp( buf, "f" ) )
        {
            unsigned int iPosition = 0;
            unsigned int iTexCoord = 0;
            unsigned int iNormal   = 0;
            unsigned int p[4]      = {-1};
            unsigned int t[4]      = {-1};
            unsigned int n[4]      = {-1};
            int          count     = 0;
            unsigned int index     = 0;
            Vertex       vertex;

            faceIndex++;
            faceCount++;

            //　三角形・四角形のみ対応
            for ( int iFace = 0; iFace < 4; iFace++ )
            {
                count++;    //　頂点数を数える
                memset( &vertex, 0, sizeof( vertex ) );

                file >> iPosition;
                vertex.position = positions[ iPosition - 1 ];
                p[iFace] = iPosition -1;

                if ( '/' == file.peek() )
                {
                    file.ignore();

                    //　テクスチャ座標インデックス
                    if ( '/' != file.peek() )
                    {
                        file >> iTexCoord;
                        vertex.texcoord = texcoords[ iTexCoord - 1 ];
                        t[iFace] = iTexCoord -1;
                    }

                    //　法線ベクトルインデックス
                    if ( '/' == file.peek() )
                    {
                        file.ignore();

                        file >> iNormal;
                        vertex.normal = normals[ iNormal - 1 ];
                        n[iFace] = iNormal -1;
                    }
                }

                //　カウントが3未満
                if ( iFace < 3 )
                {
                    index = m_Vertices.size();
                    m_Vertices.push_back( vertex );
                    m_Indices .push_back( index );
                }

                //　次が改行だったら終了
                if ( '\n' == file.peek() )
                {
                    break;
                }

            }

            //　四角形ポリゴンの場合，三角形を追加する
            if ( count > 3 )
            {
                //　カウント
                faceIndex++;
                faceCount++;

                //　頂点とインデックスを追加
                for ( int idx = 1; idx < 4; idx++ )
                {
                    int j = ( idx + 1 ) % 4;
                    memset( &vertex, 0, sizeof( vertex ) );

                    if ( p[j] != -1 ) vertex.position = positions[ p[j] ];
                    if ( t[j] != -1 ) vertex.texcoord = texcoords[ t[j] ];
                    if ( n[j] != -1 ) vertex.normal   = normals  [ n[j] ];

                    index = m_Vertices.size();
                    m_Vertices.push_back( vertex );
                    m_Indices .push_back( index );
                }

            }
        }

        //　マテリアルファイル
        else if ( 0 == strcmp( buf, "mtllib" ) )
        {
            char fileName[ 256 ];
            file >> fileName;

            materialFile =  filename;
            //　マテリアルファイルの読み込み
            if ( !materialFile.empty() )
            {
                if ( !LoadMTLFile( materialFile.c_str() ) )
                {
                    std::cerr << "Error : マテリアルのロードに失敗\n";
                    return false;
                }
            }
        }

        //　マテリアル
        else if ( 0 == strcmp( buf, "usemtl" ) )
        {
            std::string name;
            file >> name;
            Subset subset;
            subset.offset = 1;

            MaterialDictionaryItr itr = m_Materials.find( name );
            if ( itr != m_Materials.end() )
            {
                curMaterial = name;
            }

            subset.materialName = curMaterial;
            subset.offset       = faceIndex * 3;
            prevSize            = m_Subsets.size();
            m_Subsets.push_back( subset );
            if ( m_Subsets.size() > 1 )
            {
                m_Subsets[ prevSize - 1 ].count = faceCount * 3;
                faceCount = 0;
            }
        }

        file.ignore( BUFFER_LENGTH, '\n' );
    }

    //　サブセット
    if ( m_Subsets.size() > 0 )
    {
        int maxSize = m_Subsets.size();
        m_Subsets[maxSize-1].count = faceCount * 3;
    }

    //　ファイルを閉じる
    file.close();

    // メモリを解放.
    positions.clear();
    normals  .clear();
    texcoords.clear();

    // 最適化
    {
        size_t size = 0;
        size = m_Vertices.size();
        m_Vertices.resize( size );

        size = m_Subsets.size();
        m_Subsets.resize( size );

        size = m_Indices.size();
        m_Indices.resize( size );
    }

    //　バウンディングスフィアの作成
    m_Sphere = BoundingSphere( m_Box );

    //　正常終了
    return true;
}

//-------------------------------------------------------------------------------------------
//      MTLファイルから読み込み処理を行います.
//-------------------------------------------------------------------------------------------
bool MeshOBJ::LoadMTLFile( const char* filename )
{
    const int BUFFER_LENGTH = 2048;

    char buf[BUFFER_LENGTH] = { 0 };
    int  count = -1;

    std::ifstream file;
    std::string   name;

    //　ファイルを開く
    file.open( filename, std::ios::in );

    //　チェック
    if ( !file.is_open() )
    {
        std::cerr << "Error : File Open Failed\n";
        std::cerr << "File Name : " << filename << std::endl;
        return false;
    }

    //　ループ
    for( ;; )
    {
        file >> buf;
        if ( !file )
            break;

        if ( 0 == strcmp( buf, "#" ) )
        { continue; }
        // New Material
        else if ( 0 == strcmp( buf, "newmtl" ) )
        {
            count++;
            Material material;
            material.id = count;
            file >> name;

            m_Materials.insert( std::pair<std::string, Material>( name, material ) );
        }
        // Ambient Color
        else if ( 0 == strcmp( buf, "Ka" ) )
        {
            float r, g, b;
            file >> r >> g >> b;
            m_Materials[ name ].ambient = Vec3( r, g, b );
        }
        // Diffuse Color
        else if ( 0 == strcmp( buf, "Kd" ) )
        {
            float r, g, b;
            file >> r >> g >> b;
            m_Materials[ name ].diffuse = Vec3( r, g, b );
        }
        // Specular Color
        else if ( 0 == strcmp( buf, "Ks" ) )
        {
            float r, g, b;
            file >> r >> g >> b;
            m_Materials[ name ].specular = Vec3( r, g, b );
        }
        // Alpha
        else if ( ( 0 == strcmp( buf, "d" )  )
               || ( 0 == strcmp( buf, "Tr" ) ) )
        {
            file >> m_Materials[ name ].alpha;
        }
        // Shininess
        else if ( 0 == strcmp( buf, "Ns" ) )
        {
            file >> m_Materials[ name ].shininess;
        }
        // Ambient Map
        else if ( 0 == strcmp( buf, "map_Ka" ) )
        {
            file >> m_Materials[ name ].ambientMap;
        }
        // Diffuse Map
        else if ( 0 == strcmp( buf, "map_Kd" ) )
        {
            file >> m_Materials[ name ].diffuseMap;
        }
        // Specular Map
        else if ( 0 == strcmp( buf, "map_Ks" ) )
        {
            file >> m_Materials[ name ].specularMap;
        }
        // Bump Map
        else if ( 0 == strcmp( buf, "map_Bump" ) )
        {
            file >> m_Materials[ name ].bumpMap;
        }

        file.ignore( BUFFER_LENGTH, '\n' );
    }

    //　ファイルを閉じる
    file.close();

    //　正常終了
    return true;
}

//-------------------------------------------------------------------------------------------
//      ファイルから読み込み処理を行います.
//-------------------------------------------------------------------------------------------
bool MeshOBJ::LoadFromFile( const char* filename )
{
    //　OBJ, MTLファイルを読み込み
    if ( !LoadOBJFile( filename ) )
    {
        std::cerr << "Error : Load File Failed.\n";
        return false;
    }

    //　正常終了
    return true;
}


//-------------------------------------------------------------------------------------------
//      描画処理を行います.
//-------------------------------------------------------------------------------------------
void MeshOBJ::Draw()
{
    for ( size_t i = 0; i<m_Subsets.size(); i++ )
    {
        //　マテリアル
        Material& material = m_Materials[m_Subsets[i].materialName];
        SetMaterial( material );

        //　三角形描画
        glInterleavedArrays( GL_T2F_N3F_V3F, 0, &m_Vertices[0] );
        glDrawElements( GL_TRIANGLES, m_Subsets[i].count, GL_UNSIGNED_INT, &m_Indices[m_Subsets[i].offset] );
    }
}

//-------------------------------------------------------------------------------------------
//      頂点バッファを取得します.
//-------------------------------------------------------------------------------------------
MeshOBJ::VertexList& MeshOBJ::GetVertices()
{ return m_Vertices; }

//-------------------------------------------------------------------------------------------
//      サブセットを取得します.
//-------------------------------------------------------------------------------------------
MeshOBJ::SubsetList& MeshOBJ::GetSubsets()
{ return m_Subsets; }

//-------------------------------------------------------------------------------------------
//      マテリアルを取得します.
//-------------------------------------------------------------------------------------------
MeshOBJ::MaterialDictionary& MeshOBJ::GetMaterials()
{ return m_Materials; }

//-------------------------------------------------------------------------------------------
//      頂点インデックスを取得します.
//-------------------------------------------------------------------------------------------
MeshOBJ::IndexList& MeshOBJ::GetIndices()
{ return m_Indices; }

//-------------------------------------------------------------------------------------------
//      頂点バッファを取得します.
//-------------------------------------------------------------------------------------------
const MeshOBJ::VertexList& MeshOBJ::GetVertices() const
{ return m_Vertices; }

//-------------------------------------------------------------------------------------------
//      サブセットを取得します.
//-------------------------------------------------------------------------------------------
const MeshOBJ::SubsetList& MeshOBJ::GetSubsets() const
{ return m_Subsets; }

//-------------------------------------------------------------------------------------------
//      マテリアルを取得します.
//-------------------------------------------------------------------------------------------
const MeshOBJ::MaterialDictionary& MeshOBJ::GetMaterials() const
{ return m_Materials; }

//-------------------------------------------------------------------------------------------
//      頂点インデックスを取得します.
//-------------------------------------------------------------------------------------------
const MeshOBJ::IndexList& MeshOBJ::GetIndices() const
{ return m_Indices; }

//-------------------------------------------------------------------------------------------
//      バウンディングボックスを取得します.
//-------------------------------------------------------------------------------------------
BoundingBox MeshOBJ::GetBox() const
{ return m_Box; }

//-------------------------------------------------------------------------------------------
//      バウンディングスフィアを取得します.
//-------------------------------------------------------------------------------------------
BoundingSphere MeshOBJ::GetSphere() const
{ return m_Sphere; }











