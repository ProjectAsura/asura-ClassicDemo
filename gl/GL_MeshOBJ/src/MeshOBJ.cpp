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

static const unsigned int OBJ_BUFFER_LENGTH = 2048;

//-------------------------------------------------------------------------------------------
//      マテリアルの初期化を行います.
//-------------------------------------------------------------------------------------------
void InitMaterial( Material* pMaterial )
{
    memset( pMaterial, 0, sizeof( Material ) );
    pMaterial->ambient   = Vec3( 0.2f, 0.2f, 0.2f );
    pMaterial->diffuse   = Vec3( 0.8f, 0.8f, 0.8f );
    pMaterial->specular  = Vec3( 1.0f, 1.0f, 1.0f );
    pMaterial->shininess = 0.0f;
    pMaterial->alpha     = 1.0f;
}

//-------------------------------------------------------------------------------------------
//      マテリアルを設定します.
//-------------------------------------------------------------------------------------------
void SetMaterial( Material* material )
{
    if ( material == nullptr )
    { return; }

    glColor4f( 
        material->diffuse.x,
        material->diffuse.y,
        material->diffuse.z,
        material->alpha );
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,    material->ambient );
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,    material->diffuse );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,   material->specular );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, &material->shininess );
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
    SAFE_DELETE_ARRAY( m_Vertices );
    SAFE_DELETE_ARRAY( m_Materials );
    SAFE_DELETE_ARRAY( m_Subsets );
    SAFE_DELETE_ARRAY( m_Indices );

    m_NumVertices  = 0;
    m_NumMaterials = 0;
    m_NumSubsets   = 0;
    m_NumIndices   = 0;
}

//-------------------------------------------------------------------------------------------
//      OBJファイルから読み込み処理を行います.
//-------------------------------------------------------------------------------------------
bool MeshOBJ::LoadOBJFile( const char *filename )
{
    std::ifstream file;

    char buf[OBJ_BUFFER_LENGTH] = {0};
    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<Vec2> texcoords;
    std::vector<Vertex> t_vertices;
    std::vector<Subset> t_subsets;
    std::vector<unsigned int> t_indices;
    bool initBox = false;
    int prevSize = 0;

    unsigned long total = 0;

    Material material;
    unsigned int dwFaceIndex = 0;
    unsigned int dwFaceCount = 0;
    unsigned int dwCurSubset = 0;

    // ディレクトリ名を取り出す.
    {
        std::string directoryPath;
        std::string tfile( filename );
        size_t idx = tfile.find_last_of( '\\' );
        if ( idx == std::string::npos )
        { idx = tfile.find_last_of( '/' ); }
        if ( idx != std::string::npos )
        { m_DirectoryPath = tfile.substr( 0, idx + 1 ); }
    }

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
        if ( !file || file.eof() )
        { break; }

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
            unsigned int iPosition, iTexCoord, iNormal;
            unsigned int p[4] = {-1}, t[4] = {-1}, n[4] = {-1};	
            Vertex vertex;
            dwFaceIndex++;
            dwFaceCount++;
            int count = 0;
            unsigned int index = 0;

            //　三角形・四角形のみ対応
            for ( int iFace = 0; iFace < 4; iFace++ )
            {
                count++;	//　頂点数を数える
                ZeroMemory( &vertex, sizeof( Vertex ) );

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
                    t_vertices.push_back( vertex );
                    index = t_vertices.size()-1;
                    t_indices.push_back( index );
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
                dwFaceIndex++;
                dwFaceCount++;

                //　頂点とインデックスを追加
                for ( int iFace = 1; iFace < 4; iFace++ )
                {
                    int j = (iFace+1)%4;
                    memset( &vertex, 0, sizeof( Vertex ) );

                    if ( p[j] != -1 ) vertex.position = positions[ p[j] ];
                    if ( t[j] != -1 ) vertex.texcoord = texcoords[ t[j] ];
                    if ( n[j] != -1 ) vertex.normal = normals[ n[j] ];

                    t_vertices.push_back( vertex );
                    index = t_vertices.size() - 1;
                    t_indices.push_back( index );
                }

            }
        }

        //　マテリアルファイル
        else if ( 0 == strcmp( buf, "mtllib" ) )
        {
            std::string materialFile;
            file >> materialFile;
            //　マテリアルファイルの読み込み
            if ( !materialFile.empty() )
            {
                if ( !LoadMTLFile( ( m_DirectoryPath + materialFile ).c_str() ) )
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
            subset.count = 1;

            for ( unsigned int i = 0; i < m_NumMaterials; i++ )
            {
                if ( 0 == strcmp( m_Materials[i].name, name.c_str() ) )
                {
                    dwCurSubset = i;
                    break;
                }
            }

            subset.materialID = dwCurSubset;
            subset.offset = dwFaceIndex*3;
            prevSize = t_subsets.size();
            t_subsets.push_back( subset );
            if ( t_subsets.size() > 1 )
            {
                t_subsets[prevSize-1].count = dwFaceCount*3;
                dwFaceCount = 0;
            }
        }

        file.ignore( OBJ_BUFFER_LENGTH, '\n' );
    }

    //　サブセット
    if ( t_subsets.size() > 0 )
    {
        int maxSize = t_subsets.size();
        t_subsets[maxSize-1].count = dwFaceCount*3;
    }

    //　ファイルを閉じる
    file.close();

    //　メモリ破棄
    positions.clear();
    normals.clear();
    texcoords.clear();

    //　頂点データをコピー
    m_NumVertices = t_vertices.size();
    m_Vertices = new Vertex[ m_NumVertices ];
    for ( unsigned int i = 0; i<m_NumVertices; i++ )
        m_Vertices[i] = t_vertices[i];

    t_vertices.clear();

    //　サブセットデータをコピー
    m_NumSubsets = t_subsets.size();
    m_Subsets = new Subset[ m_NumSubsets ];
    for ( unsigned int i =0; i<m_NumSubsets; i++ )
        m_Subsets[i] = t_subsets[i];

    t_subsets.clear();


    //　インデックスデータをコピー
    m_NumIndices = t_indices.size();
    m_Indices = new unsigned int [ m_NumIndices ];
    for ( unsigned int i = 0; i<m_NumIndices; i++ )
        m_Indices[i] = t_indices[i];

    t_indices.clear();

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
    char buf[OBJ_BUFFER_LENGTH] = {0};
    int iMtlCount = -1;
    std::ifstream file;
    std::vector<Material> t_materials;
    Material material;
    InitMaterial( &material );

    //　ファイルを開く
    file.open( filename, std::ios::in );

    //　チェック
    if ( !file.is_open() )
    {
        std::cerr << "Error : ファイルオープンに失敗しました\n";
        std::cerr << "File Name : " << filename << std::endl;
        return false;
    }

    //　ループ
    for( ;; )
    {
        file >> buf;
        if ( !file )
            break;

        // New Material
        if ( 0 == strcmp( buf, "newmtl" ) )
        {
            iMtlCount++;
            t_materials.push_back( material );
            std::string name;
            file >> name;
            strcpy_s( t_materials[iMtlCount].name, name.c_str() );
        }
        // Ambient Color
        else if ( 0 == strcmp( buf, "Ka" ) )
        {
            float r, g, b;
            file >> r >> g >> b;
            t_materials[iMtlCount].ambient = Vec3( r, g, b );
        }
        // Diffuse Color
        else if ( 0 == strcmp( buf, "Kd" ) )
        {
            float r, g, b;
            file >> r >> g >> b;
            t_materials[iMtlCount].diffuse = Vec3( r, g, b );
        }
        // Specular Color
        else if ( 0 == strcmp( buf, "Ks" ) )
        {
            float r, g, b;
            file >> r >> g >> b;
            t_materials[iMtlCount].specular = Vec3( r, g, b );
        }
        // Alpha
        else if ( 0 == strcmp( buf, "d" ) ||
            0 == strcmp( buf, "Tr" ) )
        {
            file >> t_materials[iMtlCount].alpha;
        }
        // Shininess
        else if ( 0 == strcmp( buf, "Ns" ) )
        {
            file >> t_materials[iMtlCount].shininess;
        }
        // Ambient Map
        else if ( 0 == strcmp( buf, "map_Ka" ) )
        {
            std::string name;
            file >> name;
            strcpy_s( t_materials[iMtlCount].ambientMap, ( m_DirectoryPath + name ).c_str() );
        }
        // Diffuse Map
        else if ( 0 == strcmp( buf, "map_Kd" ) )
        {
            std::string name;
            file >> name;
            strcpy_s( t_materials[iMtlCount].diffuseMap, ( m_DirectoryPath + name ).c_str() );
        }
        // Specular Map
        else if ( 0 == strcmp( buf, "map_Ks" ) )
        {
            std::string name;
            file >> name;
            strcpy_s( t_materials[iMtlCount].specularMap, ( m_DirectoryPath + name ).c_str() );
        }
        // Bump Map
        else if ( 0 == strcmp( buf, "map_Bump" ) )
        {
            std::string name;
            file >> name;
            strcpy_s( t_materials[iMtlCount].bumpMap, ( m_DirectoryPath + name ).c_str() );
        }

        file.ignore( OBJ_BUFFER_LENGTH, '\n' );
    }

    //　ファイルを閉じる
    file.close();

    //　マテリアルデータをコピー
    m_NumMaterials = t_materials.size();
    m_Materials = new(std::nothrow) Material[ m_NumMaterials ];
    if ( m_Materials == nullptr )
    {
        t_materials.clear();
        return false;
    }

    for ( unsigned int i = 0; i<m_NumMaterials; i++ )
        m_Materials[i] = t_materials[i];

    t_materials.clear();

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
    for ( unsigned int i = 0; i<m_NumSubsets; i++ )
    {
        //　マテリアル
        Material* pMat = &m_Materials[m_Subsets[i].materialID];
        SetMaterial( pMat );	

        //　三角形描画
        glInterleavedArrays( GL_T2F_N3F_V3F, 0, m_Vertices );
        glDrawElements( GL_TRIANGLES, m_Subsets[i].count, GL_UNSIGNED_INT, &m_Indices[m_Subsets[i].offset] );
    }
}

//------------------------------------------------------------------------
// Name : GetNumVertices()
// Desc : 頂点数を取得
//------------------------------------------------------------------------
unsigned int MeshOBJ::GetNumVertices()
{ return m_NumVertices; }

//-----------------------------------------------------------------------
// Name : GetNumSubsets()
// Desc : サブセット数を取得
//-----------------------------------------------------------------------
unsigned int MeshOBJ::GetNumSubsets()
{ return m_NumSubsets; }

//-----------------------------------------------------------------------
// Name : GetNumMaterials()
// Desc : マテリアル数を取得
//-----------------------------------------------------------------------
unsigned int MeshOBJ::GetNumMaterials()
{ return m_NumMaterials; }

//-----------------------------------------------------------------------
// Name : GetNumIndices()
// Desc : インデックス数を取得
//-----------------------------------------------------------------------
unsigned int MeshOBJ::GetNumIndices()
{ return m_NumIndices; }

//-----------------------------------------------------------------------
// Name : GetIndexData()
// Desc : 指定されたインデックスデータを取得
//-----------------------------------------------------------------------
unsigned int MeshOBJ::GetIndexData( unsigned int index )
{ return m_Indices[index]; }

//-----------------------------------------------------------------------
// Name : GetVertex()
// Desc : 指定された頂点データを取得
//-----------------------------------------------------------------------
Vertex MeshOBJ::GetVertex( unsigned int index )
{ return m_Vertices[index]; }

//-----------------------------------------------------------------------
// Name : GetSubsets()
// Desc : 指定されたサブセットデータを取得
//-----------------------------------------------------------------------
Subset MeshOBJ::GetSubset( unsigned int index )
{ return m_Subsets[index]; }

//-----------------------------------------------------------------------
// Name : GetMaterial()
// Desc : 指定されたマテリアルデータを取得
//-----------------------------------------------------------------------
Material MeshOBJ::GetMaterial( unsigned int index )
{ return m_Materials[index]; }

//-----------------------------------------------------------------------
// Name : GetVertices()
// Desc : 頂点データを取得
//-----------------------------------------------------------------------
Vertex* MeshOBJ::GetVertices()
{ return m_Vertices; }

//-----------------------------------------------------------------------
// Name : GetSubsets()
// Desc : サブセットデータを取得
//-----------------------------------------------------------------------
Subset* MeshOBJ::GetSubsets()
{ return m_Subsets; }

//-----------------------------------------------------------------------
// Name : GetMaterails()
// Desc : マテリアルデータを取得
//-----------------------------------------------------------------------
Material* MeshOBJ::GetMaterials()
{ return m_Materials; }

//-----------------------------------------------------------------------
// Name : GetIndices()
// Desc : インデックスデータを取得
//-----------------------------------------------------------------------
unsigned int* MeshOBJ::GetIndices()
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











