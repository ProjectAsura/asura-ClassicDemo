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


#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY( x ) { if ( x ) { delete[] (x); (x) = nullptr; } }
#endif//SAFE_DELETE_ARRAY

namespace /* anonymous */ {

//-----------------------------------------------------------------------
// Name : InitMaterial()
// Desc : �}�e���A���̏�����
//-----------------------------------------------------------------------
void InitMaterial( OBJMATERIAL* pMaterial )
{
    memset( pMaterial, 0, sizeof( OBJMATERIAL ) );
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
void SetMaterial( OBJMATERIAL* pMaterial )
{
    glColor4f( pMaterial->diffuse.x, pMaterial->diffuse.y, pMaterial->diffuse.z, pMaterial->alpha );
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, pMaterial->ambient );
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, pMaterial->diffuse );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, pMaterial->specular );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, &pMaterial->shininess );
}

} // namespace /* anonymous */


/////////////////////////////////////////////////////////////////////////
// MeshOBJ
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
// Name : MeshOBJ()
// Desc : �R���X�g���N�^
//-----------------------------------------------------------------------
MeshOBJ::MeshOBJ()
: m_Vertices    ( nullptr )
, m_Subsets     ( nullptr )
, m_Materials   ( nullptr )
, m_Indices     ( nullptr )
, m_NumVertices ( 0 )
, m_NumSubsets  ( 0 )
, m_NumMaterials( 0 )
, m_NumIndices  ( 0 )
, m_Box         ()
, m_Sphere      ()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------
// Name : ~MeshOBJ()
// Desc : �f�X�g���N�^
//-----------------------------------------------------------------------
MeshOBJ::~MeshOBJ()
{ Release(); }

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

    m_NumVertices = 0;
    m_NumMaterials = 0;
    m_NumSubsets = 0;
    m_NumIndices = 0;
}

//-----------------------------------------------------------------------
// Name : LoadOBJFile()
// Desc : OBJ�t�@�C���̓ǂݍ���
//-----------------------------------------------------------------------
bool MeshOBJ::LoadOBJFile(const char *filename)
{
    std::ifstream file;

    char mtlFileName[OBJ_NAME_LENGTH] = {0};
    char buf[OBJ_BUFFER_LENGTH] = {0};
    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<Vec2> texcoords;
    std::vector<OBJVERTEX> t_vertices;
    std::vector<OBJSUBSET> t_subsets;
    std::vector<unsigned int> t_indices;
    bool initBox = false;
    int prevSize = 0;

    unsigned long total = 0;

    OBJMATERIAL material;
    unsigned int dwFaceIndex = 0;
    unsigned int dwFaceCount = 0;
    unsigned int dwCurSubset = 0;

    //�@�t�@�C�����J��
    file.open( filename, std::ios::in );

    //�@�`�F�b�N
    if ( !file.is_open() )
    {
        std::cerr << "Error : �t�@�C���I�[�v���Ɏ��s\n";
        std::cerr << "File Name : " << filename << std::endl;
        return false;
    }

    //�@���[�v
    for( ;; )
    {
        file >> buf;
        if ( !file )
            break;

        //�@�R�����g
        if ( 0 == strcmp( buf, "#" ) )
        {
        }

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
            OBJVERTEX vertex;
            dwFaceIndex++;
            dwFaceCount++;
            int count = 0;
            unsigned int index = 0;

            //�@�O�p�`�E�l�p�`�̂ݑΉ�
            for ( int iFace = 0; iFace < 4; iFace++ )
            {
                count++;	//�@���_���𐔂���
                ZeroMemory( &vertex, sizeof( OBJVERTEX ) );

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
                    ZeroMemory( &vertex, sizeof( OBJVERTEX ) );

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
            file >> mtlFileName;
            //�@�}�e���A���t�@�C���̓ǂݍ���
            if ( mtlFileName[0] )
            {
                if ( !LoadMTLFile( mtlFileName ) )
                {
                    std::cerr << "Error : �}�e���A���̃��[�h�Ɏ��s\n";
                    return false;
                }
            }
        }

        //�@�}�e���A��
        else if ( 0 == strcmp( buf, "usemtl" ) )
        {
            char strName[OBJ_NAME_LENGTH] = {0};
            file >> strName;
            OBJSUBSET subset;
            subset.offset = 1;

            for ( unsigned int i = 0; i < m_NumMaterials; i++ )
            {
                if ( 0 == strcmp( m_Materials[i].name, strName ) )
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

    //�@�T�u�Z�b�g
    if ( t_subsets.size() > 0 )
    {
        int maxSize = t_subsets.size();
        t_subsets[maxSize-1].count = dwFaceCount*3;
    }

    //�@�t�@�C�������
    file.close();

    positions.clear();
    normals.clear();
    texcoords.clear();

    //�@���_�f�[�^���R�s�[
    m_NumVertices = t_vertices.size();
    m_Vertices = new OBJVERTEX[ m_NumVertices ];
    for ( unsigned int i = 0; i<m_NumVertices; i++ )
        m_Vertices[i] = t_vertices[i];

    t_vertices.clear();

    //�@�T�u�Z�b�g�f�[�^���R�s�[
    m_NumSubsets = t_subsets.size();
    m_Subsets = new OBJSUBSET[ m_NumSubsets ];
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
    std::ifstream file;
    std::vector<OBJMATERIAL> t_materials;
    OBJMATERIAL material;
    InitMaterial( &material );

    //�@�t�@�C�����J��
    file.open( filename, std::ios::in );

    //�@�`�F�b�N
    if ( !file.is_open() )
    {
        std::cerr << "Error : �t�@�C���I�[�v���Ɏ��s���܂���\n";
        std::cerr << "File Name : " << filename << std::endl;
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
            char strName[OBJ_NAME_LENGTH] = {0};
            file >> strName;
            strcpy_s( t_materials[iMtlCount].name, strName );
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
            char mapKaName[OBJ_NAME_LENGTH];
            file >> mapKaName;
            strcpy_s( t_materials[iMtlCount].ambientMapName, mapKaName );
        }
        // Diffuse Map
        else if ( 0 == strcmp( buf, "map_Kd" ) )
        {
            char mapKdName[OBJ_NAME_LENGTH];
            file >> mapKdName;
            strcpy_s( t_materials[iMtlCount].diffuseMapName, mapKdName );
        }
        // Specular Map
        else if ( 0 == strcmp( buf, "map_Ks" ) )
        {
            char mapKsName[OBJ_NAME_LENGTH];
            file >> mapKsName;
            strcpy_s( t_materials[iMtlCount].specularMapName, mapKsName );
        }
        // Bump Map
        else if ( 0 == strcmp( buf, "map_Bump" ) )
        {
            char mapBumpName[OBJ_NAME_LENGTH];
            file >> mapBumpName;
            strcpy_s( t_materials[iMtlCount].bumpMapName, mapBumpName );
        }

        file.ignore( OBJ_BUFFER_LENGTH, '\n' );
    }

    //�@�t�@�C�������
    file.close();

    //�@�}�e���A���f�[�^���R�s�[
    m_NumMaterials = t_materials.size();
    m_Materials = new OBJMATERIAL[ m_NumMaterials ];
    for ( unsigned int i = 0; i<m_NumMaterials; i++ )
        m_Materials[i] = t_materials[i];

    //�@����I��
    return true;
}

//-----------------------------------------------------------------------
// Name : LoadFile()
// Desc : ���b�V���t�@�C���̓ǂݍ���
//-----------------------------------------------------------------------
bool MeshOBJ::LoadFromFile( const char* filename )
{
    //�@OBJ, MTL�t�@�C����ǂݍ���
    if ( !LoadOBJFile( filename ) )
    {
        std::cerr << "Error : ���b�V���t�@�C���̓ǂݍ��݂Ɏ��s���܂���\n";
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
        OBJMATERIAL* pMat = &m_Materials[m_Subsets[i].materialID];
        SetMaterial( pMat );	

        //�@�O�p�`�`��
        glInterleavedArrays( GL_T2F_N3F_V3F, 0, m_Vertices );
        glDrawElements( GL_TRIANGLES, m_Subsets[i].count, GL_UNSIGNED_INT, &m_Indices[m_Subsets[i].offset] );
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
OBJVERTEX MeshOBJ::GetVertex( unsigned int index )
{ return m_Vertices[index]; }

//-----------------------------------------------------------------------
// Name : GetSubsets()
// Desc : �w�肳�ꂽ�T�u�Z�b�g�f�[�^���擾
//-----------------------------------------------------------------------
OBJSUBSET MeshOBJ::GetSubset( unsigned int index )
{ return m_Subsets[index]; }

//-----------------------------------------------------------------------
// Name : GetMaterial()
// Desc : �w�肳�ꂽ�}�e���A���f�[�^���擾
//-----------------------------------------------------------------------
OBJMATERIAL MeshOBJ::GetMaterial( unsigned int index )
{ return m_Materials[index]; }

//-----------------------------------------------------------------------
// Name : GetVertices()
// Desc : ���_�f�[�^���擾
//-----------------------------------------------------------------------
OBJVERTEX* MeshOBJ::GetVertices()
{ return m_Vertices; }

//-----------------------------------------------------------------------
// Name : GetSubsets()
// Desc : �T�u�Z�b�g�f�[�^���擾
//-----------------------------------------------------------------------
OBJSUBSET* MeshOBJ::GetSubsets()
{ return m_Subsets; }

//-----------------------------------------------------------------------
// Name : GetMaterails()
// Desc : �}�e���A���f�[�^���擾
//-----------------------------------------------------------------------
OBJMATERIAL* MeshOBJ::GetMaterials()
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











