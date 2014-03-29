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
//      �}�e���A���̏��������s���܂�.
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
//      �}�e���A����ݒ肵�܂�.
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
//      �R���X�g���N�^�ł�.
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
//      �f�X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------
MeshOBJ::~MeshOBJ()
{ Release(); }


//-------------------------------------------------------------------------------------------
//      ��������������܂�.
//-------------------------------------------------------------------------------------------
void MeshOBJ::Release()
{
    m_Vertices .clear();
    m_Subsets  .clear();
    m_Materials.clear();
    m_Indices  .clear();
}

//-------------------------------------------------------------------------------------------
//      OBJ�t�@�C������ǂݍ��ݏ������s���܂�.
//-------------------------------------------------------------------------------------------
bool MeshOBJ::LoadOBJFile( const char *filename )
{
    std::ifstream   file;
    std::string     materialFile;
    std::string     curMaterial;

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

    // �t�@�C�����J��
    file.open( filename, std::ios::in );

    //�@�`�F�b�N
    if ( !file.is_open() )
    {
        std::cerr << "Error : File Open Failed.\n";
        std::cerr << "File Name : " << filename << std::endl;
        return false;
    }

    // ���[�v
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
            Vec3 v;
            file >> v.x >> v.y >> v.z;
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
            Vec2 uv;
            file >> uv.x >> uv.y;
            texcoords.push_back( uv );
        }

        //�@�@���x�N�g��
        else if ( 0 == strcmp( buf, "vn" ) )
        {
            Vec3 n;
            file >> n.x >> n.y >> n.z;
            normals.push_back( n );
        }

        //�@��
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

            //�@�O�p�`�E�l�p�`�̂ݑΉ�
            for ( int iFace = 0; iFace < 4; iFace++ )
            {
                count++;    //�@���_���𐔂���
                memset( &vertex, 0, sizeof( vertex ) );

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
                    index = m_Vertices.size();
                    m_Vertices.push_back( vertex );
                    m_Indices .push_back( index );
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
                faceIndex++;
                faceCount++;

                //�@���_�ƃC���f�b�N�X��ǉ�
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

        //�@�}�e���A���t�@�C��
        else if ( 0 == strcmp( buf, "mtllib" ) )
        {
            file >> materialFile;

            //�@�}�e���A���t�@�C���̓ǂݍ���
            if ( !materialFile.empty() )
            {
                if ( !LoadMTLFile( ( m_DirectoryPath + materialFile ).c_str() ) )
                {
                    std::cerr << "Error : �}�e���A���̃��[�h�Ɏ��s\n";
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
            subset.offset = 1;

            MaterialDictionaryItr itr = m_Materials.find( name );
            if ( itr != m_Materials.end() )
            { curMaterial = name; }

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

    //�@�T�u�Z�b�g
    if ( m_Subsets.size() > 0 )
    {
        int maxSize = m_Subsets.size();
        m_Subsets[maxSize-1].count = faceCount * 3;
    }

    // �t�@�C�������
    file.close();

    // �����������.
    positions.clear();
    normals  .clear();
    texcoords.clear();

    // �œK��
    {
        m_Vertices.shrink_to_fit();
        m_Subsets .shrink_to_fit();
        m_Indices .shrink_to_fit();
    }

    //�@�o�E���f�B���O�X�t�B�A�̍쐬
    m_Sphere = BoundingSphere( m_Box );

    //�@����I��
    return true;
}

//-------------------------------------------------------------------------------------------
//      MTL�t�@�C������ǂݍ��ݏ������s���܂�.
//-------------------------------------------------------------------------------------------
bool MeshOBJ::LoadMTLFile( const char* filename )
{
    const int BUFFER_LENGTH = 2048;

    char buf[BUFFER_LENGTH] = { 0 };
    int  count = -1;

    std::ifstream file;
    std::string   name;

    //�@�t�@�C�����J��
    file.open( filename, std::ios::in );

    //�@�`�F�b�N
    if ( !file.is_open() )
    {
        std::cerr << "Error : File Open Failed\n";
        std::cerr << "File Name : " << filename << std::endl;
        return false;
    }

    //�@���[�v
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
            std::string name;
            file >> name;
            m_Materials[ name ].ambientMap = m_DirectoryPath + name;
        }
        // Diffuse Map
        else if ( 0 == strcmp( buf, "map_Kd" ) )
        {
            std::string name;
            file >> name;
            m_Materials[ name ].diffuseMap = m_DirectoryPath + name;
        }
        // Specular Map
        else if ( 0 == strcmp( buf, "map_Ks" ) )
        {
            std::string name;
            file >> name;
            m_Materials[ name ].specularMap = m_DirectoryPath + name;
        }
        // Bump Map
        else if ( 0 == strcmp( buf, "map_Bump" ) )
        {
            std::string name;
            file >> name;
            m_Materials[ name ].bumpMap = m_DirectoryPath + name;
        }

        file.ignore( BUFFER_LENGTH, '\n' );
    }

    //�@�t�@�C�������
    file.close();

    //�@����I��
    return true;
}

//-------------------------------------------------------------------------------------------
//      �t�@�C������ǂݍ��ݏ������s���܂�.
//-------------------------------------------------------------------------------------------
bool MeshOBJ::LoadFromFile( const char* filename )
{
    //�@OBJ, MTL�t�@�C����ǂݍ���
    if ( !LoadOBJFile( filename ) )
    {
        std::cerr << "Error : Load File Failed.\n";
        return false;
    }

    //�@����I��
    return true;
}


//-------------------------------------------------------------------------------------------
//      �`�揈�����s���܂�.
//-------------------------------------------------------------------------------------------
void MeshOBJ::Draw()
{
    for ( size_t i = 0; i<m_Subsets.size(); i++ )
    {
        // �T�u�Z�b�g���擾
        Subset& subset = m_Subsets[ i ];

        // �}�e���A��
        Material& material = m_Materials[ subset.materialName ];
        SetMaterial( material );

        //�@�O�p�`�`��
        glInterleavedArrays( GL_T2F_N3F_V3F, 0, &m_Vertices[0] );
        glDrawElements( GL_TRIANGLES, subset.count, GL_UNSIGNED_INT, &m_Indices[ subset.offset ] );
    }
}

//-------------------------------------------------------------------------------------------
//      ���_�o�b�t�@���擾���܂�.
//-------------------------------------------------------------------------------------------
MeshOBJ::VertexList& MeshOBJ::GetVertices()
{ return m_Vertices; }

//-------------------------------------------------------------------------------------------
//      �T�u�Z�b�g���擾���܂�.
//-------------------------------------------------------------------------------------------
MeshOBJ::SubsetList& MeshOBJ::GetSubsets()
{ return m_Subsets; }

//-------------------------------------------------------------------------------------------
//      �}�e���A�����擾���܂�.
//-------------------------------------------------------------------------------------------
MeshOBJ::MaterialDictionary& MeshOBJ::GetMaterials()
{ return m_Materials; }

//-------------------------------------------------------------------------------------------
//      ���_�C���f�b�N�X���擾���܂�.
//-------------------------------------------------------------------------------------------
MeshOBJ::IndexList& MeshOBJ::GetIndices()
{ return m_Indices; }

//-------------------------------------------------------------------------------------------
//      ���_�o�b�t�@���擾���܂�.
//-------------------------------------------------------------------------------------------
const MeshOBJ::VertexList& MeshOBJ::GetVertices() const
{ return m_Vertices; }

//-------------------------------------------------------------------------------------------
//      �T�u�Z�b�g���擾���܂�.
//-------------------------------------------------------------------------------------------
const MeshOBJ::SubsetList& MeshOBJ::GetSubsets() const
{ return m_Subsets; }

//-------------------------------------------------------------------------------------------
//      �}�e���A�����擾���܂�.
//-------------------------------------------------------------------------------------------
const MeshOBJ::MaterialDictionary& MeshOBJ::GetMaterials() const
{ return m_Materials; }

//-------------------------------------------------------------------------------------------
//      ���_�C���f�b�N�X���擾���܂�.
//-------------------------------------------------------------------------------------------
const MeshOBJ::IndexList& MeshOBJ::GetIndices() const
{ return m_Indices; }

//-------------------------------------------------------------------------------------------
//      �o�E���f�B���O�{�b�N�X���擾���܂�.
//-------------------------------------------------------------------------------------------
BoundingBox MeshOBJ::GetBox() const
{ return m_Box; }

//-------------------------------------------------------------------------------------------
//      �o�E���f�B���O�X�t�B�A���擾���܂�.
//-------------------------------------------------------------------------------------------
BoundingSphere MeshOBJ::GetSphere() const
{ return m_Sphere; }











