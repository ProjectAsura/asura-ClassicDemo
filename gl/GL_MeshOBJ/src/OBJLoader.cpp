#pragma region File Description
//-----------------------------------------------------------------------
// File : OBJLoader.cpp
// Desc : Alias Wavefront OBJ File Loader
// Date : Mar. 04, 2009
// Version : 2.1
// Author : Pocol
// Memo : 4�p�`�Ή��ς݁B5�p�`�ȏ�̓T�|�[�g���Ȃ��B
//-----------------------------------------------------------------------
#pragma endregion 

//
// Includes
//
#include "OBJLoader.h"
#include <string>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector>


// SAFE_DELETE_ARRAY
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) { if (x) { delete [] (x); (x) = NULL; } }
#endif

using namespace std;

namespace /* anonymous */ {

//----------------------------------------------------------------------
// Name : InitMaterial()
// Desc : �}�e���A���̏�����
//-----------------------------------------------------------------------
void InitMaterial( Material* pMaterial )
{
    memset( pMaterial, 0, sizeof( Material ) );
    pMaterial->ambient   = Vec3( 0.2f, 0.2f, 0.2f );
    pMaterial->diffuse   = Vec3( 0.8f, 0.8f, 0.8f );
    pMaterial->specular  = Vec3( 1.0f, 1.0f, 1.0f );
    pMaterial->shininess = 0.0f;
    pMaterial->alpha     = 1.0f;
}

//-----------------------------------------------------------------------
// Name : SetMaterial()
// Desc : �}�e���A����ݒ肷��
//-----------------------------------------------------------------------
void SetMaterial( Material* pMaterial )
{
    glColor4f(
        pMaterial->diffuse.x,
        pMaterial->diffuse.y, 
        pMaterial->diffuse.z,
        pMaterial->alpha );
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,    pMaterial->ambient );
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,    pMaterial->diffuse );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,   pMaterial->specular );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, &pMaterial->shininess );
}

} // namespace /* anoymous */

/////////////////////////////////////////////////////////////////////////
// MeshOBJ
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
// Name : MeshOBJ()
// Desc : �R���X�g���N�^
//-----------------------------------------------------------------------
MeshOBJ::MeshOBJ()
{
    m_NumVertices = 0;
    m_NumMaterials = 0;
    m_NumSubsets = 0;
    m_NumIndices = 0;

    m_Vertices = 0;
    m_Materials = 0;
    m_Subsets = 0;
    m_Indices = 0;
}

//-----------------------------------------------------------------------
// Name : ~MeshOBJ()
// Desc : �f�X�g���N�^
//-----------------------------------------------------------------------
MeshOBJ::~MeshOBJ()
{
    Release();
};

//-----------------------------------------------------------------------
// Name : Release()
// Desc : ��������j��
//-----------------------------------------------------------------------
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

//-----------------------------------------------------------------------
// Name : LoadOBJFile()
// Desc : OBJ�t�@�C���̓ǂݍ���
//-----------------------------------------------------------------------
bool MeshOBJ::LoadOBJFile(const char *filename)
{
    ifstream file;

    char buf[OBJ_BUFFER_LENGTH] = {0};
    vector<Vec3> positions;
    vector<Vec3> normals;
    vector<Vec2> texcoords;
    vector<Vertex> t_vertices;
    vector<Subset> t_subsets;
    vector<unsigned int> t_indices;
    bool initBox = false;
    int prevSize = 0;

    unsigned long total = 0;

    Material material;
    unsigned int dwFaceIndex = 0;
    unsigned int dwFaceCount = 0;
    unsigned int dwCurSubset = 0;

    // �f�B���N�g���������o��.
    {
        std::string directoryPath;
        std::string tfile( filename );
        size_t idx = tfile.find_last_of( '\\' );
        if ( idx == std::string::npos )
        { idx = tfile.find_last_of( '/' ); }
        if ( idx != std::string::npos )
        { m_DirectoryPath = tfile.substr( 0, idx + 1 ); }
    }

    //�@�t�@�C�����J��
    file.open( filename, ios::in );

    //�@�`�F�b�N
    if ( !file.is_open() )
    {
        cerr << "Error : �t�@�C���I�[�v���Ɏ��s\n";
        cerr << "File Name : " << filename << endl;
        return false;
    }

    //�@���[�v
    for( ;; )
    {
        file >> buf;
        if ( !file || file.eof() )
        { break; }

        //�@�R�����g
        if ( 0 == strcmp( buf, "#" ) )
        { continue; }

        //�@���_���W
        else if ( 0 == strcmp( buf, "v" ) )
        {
            float x, y, z;
            file >> x >> y >> z;
            Vec3 v( x, y, z );
            positions.push_back( v );

            //�@�o�E���f�B���O�{�b�N�X�̏�����
            if ( !initBox )
            {
                m_Box = BoundingBox( v );
                initBox = true;
            }

            //�@�o�E���f�B���O�{�b�N�X�̎Z�o
            m_Box.Merge( v );
        }

        //�@�e�N�X�`�����W
        else if ( 0 == strcmp( buf, "vt" ) )
        {
            float u, v;
            file >> u >> v;
            texcoords.push_back( Vec2( u, v ) );
        }

        //�@�@���x�N�g��
        else if ( 0 == strcmp( buf, "vn" ) )
        {
            float x, y, z;
            file >> x >> y >> z;
            normals.push_back( Vec3( x, y, z) );
        }

        //�@��
        else if ( 0 == strcmp( buf, "f" ) )
        {
            unsigned int iPosition, iTexCoord, iNormal;
            unsigned int p[4] = {-1}, t[4] = {-1}, n[4] = {-1};	
            Vertex vertex;
            dwFaceIndex++;
            dwFaceCount++;
            int count = 0;
            unsigned int index = 0;

            //�@�O�p�`�E�l�p�`�̂ݑΉ�
            for ( int iFace = 0; iFace < 4; iFace++ )
            {
                count++;	//�@���_���𐔂���
                ZeroMemory( &vertex, sizeof( Vertex ) );

                file >> iPosition;
                vertex.position = positions[ iPosition - 1 ];
                p[iFace] = iPosition -1;

                if ( '/' == file.peek() )
                {
                    file.ignore();

                    //�@�e�N�X�`�����W�C���f�b�N�X
                    if ( '/' != file.peek() )
                    {
                        file >> iTexCoord;
                        vertex.texcoord = texcoords[ iTexCoord - 1 ];
                        t[iFace] = iTexCoord -1;
                    }

                    //�@�@���x�N�g���C���f�b�N�X
                    if ( '/' == file.peek() )
                    {
                        file.ignore();

                        file >> iNormal;
                        vertex.normal = normals[ iNormal - 1 ];
                        n[iFace] = iNormal -1;
                    }
                }

                //�@�J�E���g��3����
                if ( iFace < 3 )
                {
                    t_vertices.push_back( vertex );
                    index = t_vertices.size()-1;
                    t_indices.push_back( index );
                }

                //�@�������s��������I��
                if ( '\n' == file.peek() )
                {
                    break;
                }

            }

            //�@�l�p�`�|���S���̏ꍇ�C�O�p�`��ǉ�����
            if ( count > 3 )
            {
                //�@�J�E���g
                dwFaceIndex++;
                dwFaceCount++;

                //�@���_�ƃC���f�b�N�X��ǉ�
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

        //�@�}�e���A���t�@�C��
        else if ( 0 == strcmp( buf, "mtllib" ) )
        {
            std::string materialFile;
            file >> materialFile;
            //�@�}�e���A���t�@�C���̓ǂݍ���
            if ( !materialFile.empty() )
            {
                if ( !LoadMTLFile( ( m_DirectoryPath + materialFile ).c_str() ) )
                {
                    cerr << "Error : �}�e���A���̃��[�h�Ɏ��s\n";
                    return false;
                }
            }
        }

        //�@�}�e���A��
        else if ( 0 == strcmp( buf, "usemtl" ) )
        {
            std::string name;
            file >> name;
            Subset subset;
            subset.faceCount = 1;

            for ( unsigned int i = 0; i < m_NumMaterials; i++ )
            {
                if ( 0 == strcmp( m_Materials[i].name, name.c_str() ) )
                {
                    dwCurSubset = i;
                    break;
                }
            }

            subset.materialIndex = dwCurSubset;
            subset.faceStart = dwFaceIndex*3;
            prevSize = t_subsets.size();
            t_subsets.push_back( subset );
            if ( t_subsets.size() > 1 )
            {
                t_subsets[prevSize-1].faceCount = dwFaceCount*3;
                dwFaceCount = 0;
            }
        }

        file.ignore( OBJ_BUFFER_LENGTH, '\n' );
    }

    //�@�T�u�Z�b�g
    if ( t_subsets.size() > 0 )
    {
        int maxSize = t_subsets.size();
        t_subsets[maxSize-1].faceCount = dwFaceCount*3;
    }

    //�@�t�@�C�������
    file.close();

    //�@�������j��
    positions.clear();
    normals.clear();
    texcoords.clear();

    //�@���_�f�[�^���R�s�[
    m_NumVertices = t_vertices.size();
    m_Vertices = new Vertex[ m_NumVertices ];
    for ( unsigned int i = 0; i<m_NumVertices; i++ )
        m_Vertices[i] = t_vertices[i];

    t_vertices.clear();

    //�@�T�u�Z�b�g�f�[�^���R�s�[
    m_NumSubsets = t_subsets.size();
    m_Subsets = new Subset[ m_NumSubsets ];
    for ( unsigned int i =0; i<m_NumSubsets; i++ )
        m_Subsets[i] = t_subsets[i];

    t_subsets.clear();


    //�@�C���f�b�N�X�f�[�^���R�s�[
    m_NumIndices = t_indices.size();
    m_Indices = new unsigned int [ m_NumIndices ];
    for ( unsigned int i = 0; i<m_NumIndices; i++ )
        m_Indices[i] = t_indices[i];

    t_indices.clear();


    //�@�o�E���f�B���O�X�t�B�A�̍쐬
    m_Sphere = BoundingSphere( m_Box );


    //�@����I��
    return true;
}

//-----------------------------------------------------------------------
// Name : LoadMTLFile()
// Desc : MTL�t�@�C���̓ǂݍ���
//-----------------------------------------------------------------------
bool MeshOBJ::LoadMTLFile( const char* filename )
{
    char buf[OBJ_BUFFER_LENGTH] = {0};
    int iMtlCount = -1;
    ifstream file;
    vector<Material> t_materials;
    Material material;
    InitMaterial( &material );

    //�@�t�@�C�����J��
    file.open( filename, ios::in );

    //�@�`�F�b�N
    if ( !file.is_open() )
    {
        cerr << "Error : �t�@�C���I�[�v���Ɏ��s���܂���\n";
        cerr << "File Name : " << filename << endl;
        return false;
    }

    //�@���[�v
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
            strcpy_s( t_materials[iMtlCount].ambientMapName, ( m_DirectoryPath + name ).c_str() );
        }
        // Diffuse Map
        else if ( 0 == strcmp( buf, "map_Kd" ) )
        {
            std::string name;
            file >> name;
            strcpy_s( t_materials[iMtlCount].diffuseMapName, ( m_DirectoryPath + name ).c_str() );
        }
        // Specular Map
        else if ( 0 == strcmp( buf, "map_Ks" ) )
        {
            std::string name;
            file >> name;
            strcpy_s( t_materials[iMtlCount].specularMapName, ( m_DirectoryPath + name ).c_str() );
        }
        // Bump Map
        else if ( 0 == strcmp( buf, "map_Bump" ) )
        {
            std::string name;
            file >> name;
            strcpy_s( t_materials[iMtlCount].bumpMapName, ( m_DirectoryPath + name ).c_str() );
        }

        file.ignore( OBJ_BUFFER_LENGTH, '\n' );
    }

    //�@�t�@�C�������
    file.close();

    //�@�}�e���A���f�[�^���R�s�[
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

    //�@����I��
    return true;
}

//-----------------------------------------------------------------------
// Name : LoadFile()
// Desc : ���b�V���t�@�C���̓ǂݍ���
//-----------------------------------------------------------------------
bool MeshOBJ::LoadFile( const char* filename )
{
    //�@OBJ, MTL�t�@�C����ǂݍ���
    if ( !LoadOBJFile( filename ) )
    {
        cerr << "Error : ���b�V���t�@�C���̓ǂݍ��݂Ɏ��s���܂���\n";
        return false;
    }

    //�@����I��
    return true;
}


//-----------------------------------------------------------------------
// Name : Draw()
// Desc : �`�揈��
//-----------------------------------------------------------------------
void MeshOBJ::Draw()
{
    for ( unsigned int i = 0; i<m_NumSubsets; i++ )
    {
        //�@�}�e���A��
        Material* pMat = &m_Materials[m_Subsets[i].materialIndex];
        SetMaterial( pMat );	

        //�@�O�p�`�`��
        glInterleavedArrays( GL_T2F_N3F_V3F, 0, m_Vertices );
        glDrawElements( GL_TRIANGLES, m_Subsets[i].faceCount, GL_UNSIGNED_INT, &m_Indices[m_Subsets[i].faceStart] );
    }
}


//------------------------------------------------------------------------
// Name : GetNumVertices()
// Desc : ���_�����擾
//------------------------------------------------------------------------
unsigned int MeshOBJ::GetNumVertices()
{ return m_NumVertices; }

//-----------------------------------------------------------------------
// Name : GetNumSubsets()
// Desc : �T�u�Z�b�g�����擾
//-----------------------------------------------------------------------
unsigned int MeshOBJ::GetNumSubsets()
{ return m_NumSubsets; }

//-----------------------------------------------------------------------
// Name : GetNumMaterials()
// Desc : �}�e���A�������擾
//-----------------------------------------------------------------------
unsigned int MeshOBJ::GetNumMaterials()
{ return m_NumMaterials; }

//-----------------------------------------------------------------------
// Name : GetNumIndices()
// Desc : �C���f�b�N�X�����擾
//-----------------------------------------------------------------------
unsigned int MeshOBJ::GetNumIndices()
{ return m_NumIndices; }

//-----------------------------------------------------------------------
// Name : GetIndexData()
// Desc : �w�肳�ꂽ�C���f�b�N�X�f�[�^���擾
//-----------------------------------------------------------------------
unsigned int MeshOBJ::GetIndexData( unsigned int index )
{ return m_Indices[index]; }

//-----------------------------------------------------------------------
// Name : GetVertex()
// Desc : �w�肳�ꂽ���_�f�[�^���擾
//-----------------------------------------------------------------------
Vertex MeshOBJ::GetVertex( unsigned int index )
{ return m_Vertices[index]; }

//-----------------------------------------------------------------------
// Name : GetSubsets()
// Desc : �w�肳�ꂽ�T�u�Z�b�g�f�[�^���擾
//-----------------------------------------------------------------------
Subset MeshOBJ::GetSubset( unsigned int index )
{ return m_Subsets[index]; }

//-----------------------------------------------------------------------
// Name : GetMaterial()
// Desc : �w�肳�ꂽ�}�e���A���f�[�^���擾
//-----------------------------------------------------------------------
Material MeshOBJ::GetMaterial( unsigned int index )
{ return m_Materials[index]; }

//-----------------------------------------------------------------------
// Name : GetVertices()
// Desc : ���_�f�[�^���擾
//-----------------------------------------------------------------------
Vertex* MeshOBJ::GetVertices()
{ return m_Vertices; }

//-----------------------------------------------------------------------
// Name : GetSubsets()
// Desc : �T�u�Z�b�g�f�[�^���擾
//-----------------------------------------------------------------------
Subset* MeshOBJ::GetSubsets()
{ return m_Subsets; }

//-----------------------------------------------------------------------
// Name : GetMaterails()
// Desc : �}�e���A���f�[�^���擾
//-----------------------------------------------------------------------
Material* MeshOBJ::GetMaterials()
{ return m_Materials; }

//-----------------------------------------------------------------------
// Name : GetIndices()
// Desc : �C���f�b�N�X�f�[�^���擾
//-----------------------------------------------------------------------
unsigned int* MeshOBJ::GetIndices()
{ return m_Indices; }

//-----------------------------------------------------------------------
// Name : GetBox()
// Desc : �o�E���f�B���O�{�b�N�X���擾
//-----------------------------------------------------------------------
BoundingBox MeshOBJ::GetBox()
{ return m_Box; }

//-----------------------------------------------------------------------
// Name : GetSphere()
// Desc : �o�E���f�B���O�X�t�B�A���擾
//-----------------------------------------------------------------------
BoundingSphere MeshOBJ::GetSphere()
{ return m_Sphere; }











