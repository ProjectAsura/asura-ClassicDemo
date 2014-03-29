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
    //! @brief      �R���X�g���N�^�ł�.
    //--------------------------------------------------------------------------------------
    Token( char* pHead )
    : m_pPtr ( pHead )
    { memset( m_Token, 0, sizeof(char) * 2048 ); }

    //--------------------------------------------------------------------------------------
    //! @brief      �g�[�N�����L���ł��邩�`�F�b�N���܂�.
    //--------------------------------------------------------------------------------------
    bool IsValid( const char* token )
    { return ( 0 == strcmp( m_Token, token ) ); }

    //--------------------------------------------------------------------------------------
    //! @brief      ���̃g�[�N�����擾���܂�.
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
    //! @brief      ���̃g�[�N�����L���ł��邩�`�F�b�N���܂�.
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
    //! @brief      �g�[�N����float�Ƃ��Ď擾���܂�.
    //-------------------------------------------------------------------------------------
    float GetAsFloat()
    { return (float)atof( m_Token ); }

    //-------------------------------------------------------------------------------------
    //! @brief      �g�[�N����int�Ƃ��Ď擾���܂�.
    //-------------------------------------------------------------------------------------
    int GetAsInt()
    { return (int)atoi( m_Token ); }

    //-------------------------------------------------------------------------------------
    //! @brief      �g�[�N����std::string�Ƃ��Ď擾���܂�.
    //-------------------------------------------------------------------------------------
    std::string GetAsString()
    {
        char temp[256];
        strcpy_s( temp, m_Token );
        return std::string(temp);
    }

    //-------------------------------------------------------------------------------------
    //! @brief      ���̃g�[�N����float�Ƃ��Ď擾���܂�.
    //-------------------------------------------------------------------------------------
    float GetNextAsFloat()
    { 
        GetNext();
        return GetAsFloat();
    }

    //-------------------------------------------------------------------------------------
    //! @brief      ���̃g�[�N����int�Ƃ��Ď擾���܂�.
    //-------------------------------------------------------------------------------------
    int GetNextAsInt()
    {
        GetNext();
        return GetAsInt();
    }

    //-------------------------------------------------------------------------------------
    //! @brief      ���̃g�[�N����Vec2�Ƃ��Ď擾���܂�.
    //-------------------------------------------------------------------------------------
    Vec2 GetNextAsVec2()
    {
        float x = GetNextAsFloat();
        float y = GetNextAsFloat();
        return Vec2( x, y );
    }

    //-------------------------------------------------------------------------------------
    //! @brief      ���̃g�[�N����Vec3�Ƃ��Ď擾���܂�.
    //-------------------------------------------------------------------------------------
    Vec3 GetNextAsVec3()
    {
        float x = GetNextAsFloat();
        float y = GetNextAsFloat();
        float z = GetNextAsFloat();
        return Vec3( x, y, z );
    }

    //-------------------------------------------------------------------------------------
    //! @brief      ���̃g�[�N����Vec3�Ƃ��Ď擾���܂�.
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
    //! @brief      ���̃g�[�N����std::string�Ƃ��Ď擾���܂�.
    //-------------------------------------------------------------------------------------
    std::string GetNextAsString()
    {
        GetNext();
        return GetAsString();
    }

    //-------------------------------------------------------------------------------------
    //! @brief      �m�[�h���X�L�b�v���܂�.
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
    //! @brief      ��ł��邩�ǂ����`�F�b�N���܂�.
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
    Token           ( const Token& value );     // �A�N�Z�X�֎~.
    void operator = ( const Token& value );     // �A�N�Z�X�֎~.
};


} // namespace /* anonymous */ 


///////////////////////////////////////////////////////////////////////////////////////////
// ModelX class
///////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//-----------------------------------------------------------------------------------------
ModelX::ModelX()
: m_Meshes      ()
, m_Materials   ()
, m_Box         ()
, m_Sphere      ()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------------------
//      �R�s�[�R���X�g���N�^�ł�.
//-----------------------------------------------------------------------------------------
ModelX::ModelX( const ModelX& value )
: m_Meshes      ( value.m_Meshes )
, m_Materials   ( value.m_Materials )
, m_Box         ( value.m_Box )
, m_Sphere      ( value.m_Sphere )
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//-----------------------------------------------------------------------------------------
ModelX::~ModelX()
{ Release(); }

//-----------------------------------------------------------------------------------------
//      ����������s���܂�.
//-----------------------------------------------------------------------------------------
void ModelX::Release()
{
    m_Meshes   .clear();
    m_Materials.clear();
}

//-----------------------------------------------------------------------------------------
//      �t�@�C������ǂݍ��݂��܂�.
//-----------------------------------------------------------------------------------------
bool ModelX::LoadFromFile( const char* filename )
{
    // �����̃`�F�b�N.
    if ( filename == nullptr )
    {
        ELOG( "Error : Invalid Argument." );
        return false;
    }

    // �t�@�C�����J��.
    FILE* pFile;
    errno_t err = fopen_s( &pFile, filename, "rb" );
    if ( err != 0 )
    {
        ELOG( "Error : File Open Failed." );
        return false;
    }

    // �t�@�C���T�C�Y�𒲂ׂ�.
    long curr = ftell( pFile );
    fseek( pFile, 0, SEEK_END );
    long end = ftell( pFile );
    fseek( pFile, curr, SEEK_SET );
    size_t size = end - curr;

    // ����������C�Ɋm��.
    char* pBuffer = new (std::nothrow) char [ size ];
    if ( pBuffer == nullptr )
    {
        ELOG( "Error : Memory Allocate Failed." );
        fclose( pFile );
        return false;
    }

    // ��C�ɓǂݍ���.
    size_t readSize = fread( pBuffer, sizeof(char), size, pFile );

    // �t�@�C�������.
    fclose( pFile );

    // �ǂݍ��݃T�C�Y���`�F�b�N.
    if ( readSize != size )
    {
        ELOG( "Error : Read Size Not Matched." );
        SAFE_DELETE_ARRAY( pBuffer );
        return false;
    }

    // �g�[�J�i�C�U�[��p��.
    Token token( pBuffer );

    // ���b�V���̃C���f�b�N�X.
    int meshID = -1;

    // �o�b�t�@�Ō�܂Ń��[�v.
    while( !token.IsEmpty() )
    {
        // �g�[�N���擾.
        token.GetNext();

        // �e���v���[�g�m�[�h.
        if ( token.IsValid( "template") )
        {
            // �m�[�h��ǂݔ�΂�.
            token.SkipNode();
        }
        // ���b�V���m�[�h.
        else if ( token.IsValid( "Mesh" ) )
        {
            // �g�[�N���擾.
            token.GetNext();

            // �g�[�N����"{"�o�Ȃ��ꍇ.
            if ( !token.IsValid( "{" ) )
            {
                /* ���̎����ł̓��b�V���t�@�C�����͎g��Ȃ� */

                token.IsNextValid( "{" );
            }

            // ���b�V����ǉ�.
            {
                MeshX mesh;
                m_Meshes.push_back( mesh );

                // ���b�V���ԍ����X�V.
                meshID++;
            }

            // �ʒu���W�����擾.
            int posCount = token.GetNextAsInt();

            // ���������m��.
            m_Meshes[ meshID ].positions.resize( posCount );

            // �ʒu���W�f�[�^��ǂݍ���.
            for( int i=0; i<posCount; ++i )
            { m_Meshes[ meshID ].positions[i] = token.GetNextAsVec3(); }

            // �ʐ����擾.
            int faceCount = token.GetNextAsInt();

            // ���������m��.
            m_Meshes[ meshID ].faces.resize( faceCount );

            // �ʃf�[�^��ǂݎ��..
            for( int i=0; i<faceCount; ++i )
            {
                // �ʂ̍\���v�f�����擾���C�ݒ肷��.
                int element = token.GetNextAsInt();
                m_Meshes[ meshID ].faces[i].element = element;

                // �C���f�b�N�X��ǂݎ��.
                for( int j=0; j<element; ++j )
                {
                    int index = token.GetNextAsInt();
                    m_Meshes[ meshID ].faces[ i ].indexP[ j ] = index;
                    m_Meshes[ meshID ].faces[ i ].indexU[ j ] = index;
                }
            }
        }
        // ���b�V���@���x�N�g���m�[�h.
        else if ( token.IsValid( "MeshNormals" ) )
        {
            token.IsNextValid( "{" );

            // �@���x�N�g���𐔂��擾.
            size_t normalCount = token.GetNextAsInt();

            // ���������m��.
            m_Meshes[ meshID ].normals.resize( normalCount );

            // �@���x�N�g���f�[�^��ǂݎ��.
            for( size_t i=0; i<normalCount; ++i )
            { m_Meshes[ meshID ].normals[ i ] = token.GetNextAsVec3(); }

            // �ʐ����擾.
            size_t faceCount = token.GetNextAsInt();

            // �ʐ�����v���邱�Ƃ��m�F.
            if ( faceCount != m_Meshes[ meshID ].faces.size() )
            {
                // �G���[���O�o��.
                ELOG( "Error : Face Count Not Matched." );

                // �����������.
                SAFE_DELETE_ARRAY( pBuffer );
                Release();

                // �ُ�I��.
                return false;
            }

            // �ʃf�[�^��ǂݎ��.
            for( size_t i=0; i<faceCount; ++i )
            {
                // �ʂ̍\���v�f�����擾.
                int element = token.GetNextAsInt();

                // �@���x�N�g���C���f�b�N�X��ǂݎ��.
                for( int j=0; j<element; ++j )
                { m_Meshes[ meshID ].faces[ i ].indexN[ j ] = token.GetNextAsInt(); }
            }
        }
        // ���b�V���e�N�X�`�����W�m�[�h.
        else if ( token.IsValid( "MeshTextureCoords" ) )
        {
            token.IsNextValid( "{" );

            // �e�N�X�`�����W�����擾.
            size_t uvCount = token.GetNextAsInt();

            // ���������m��.
            m_Meshes[ meshID ].texcoords.resize( uvCount );

            // �e�N�X�`�����W�f�[�^��ǂݎ��.
            for( size_t i=0; i<uvCount; ++i )
            { m_Meshes[ meshID ].texcoords[i] = token.GetNextAsVec2(); }
        }
        // ���b�V���}�e���A�����X�g�m�[�h.
        else if ( token.IsValid( "MeshMaterialList" ) )
        {
            token.IsNextValid( "{" );

            // �}�e���A�������擾.
            size_t materialCount = token.GetNextAsInt();

            // ���������m��.
            m_Materials.resize( materialCount );

            // �ʐ����擾.
            size_t faceCount = token.GetNextAsInt();

            // �ʐ�����v���邱�Ƃ��m�F.
            if ( faceCount != m_Meshes[ meshID ].faces.size() )
            {
                // �G���[���O�o��.
                ELOG( "Error : Face Count Not Matched." );

                // �����������.
                SAFE_DELETE_ARRAY( pBuffer );
                Release();

                // �ُ�I��.
                return false;
            }

            // �}�e���A���C���f�b�N�X�f�[�^��ǂݎ��.
            for( size_t i=0; i<faceCount; ++i )
            { m_Meshes[ meshID ].faces[ i ].indexM = token.GetNextAsInt(); }

            // �}�e���A���f�[�^��ǂݎ��.
            for( size_t i=0; i<materialCount; ++i )
            {
                token.IsNextValid("Material");
                token.GetNext();

                // �}�e���A�������擾.
                if ( !token.IsValid( "{" ) )
                {
                    m_Materials[ i ].name = token.GetAsString();
                    token.IsNextValid( "{" );
                }
                else
                {
                    // �擾�ł��Ȃ��ꍇ�͓K���Ȗ��O��t����.
                    char temp[ 256 ];
                    sprintf_s( temp, "material_%u", i );
                    m_Materials[ i ].name = std::string(temp);
                }

                m_Materials[ i ].diffuse  = token.GetNextAsVec4();
                m_Materials[ i ].power    = token.GetNextAsFloat();
                m_Materials[ i ].specular = token.GetNextAsVec3();
                m_Materials[ i ].emissive = token.GetNextAsVec3();

                // �e�N�X�`���t�@�C���f�[�^�̃`�F�b�N.
                if ( !token.IsNextValid( "}" ) )
                {
                    if ( token.IsValid( "TextureFileName" ) )
                    {
                        token.IsNextValid( "{" );
                        m_Materials[ i ].texture = token.GetNextAsString();
                        token.IsNextValid( "}" ); // �e�N�X�`���t�@�C�����̏I���.
                        token.IsNextValid( "}" ); // �}�e���A���̏I���.
                    }
                }
            }
        }
    }

    // �s�v�ȃ����������.
    SAFE_DELETE_ARRAY( pBuffer );

    // �œK��.
    {
        for( size_t i=0; i<m_Meshes.size(); ++i )
        { m_Meshes[i].Optimize(); }

        m_Meshes   .shrink_to_fit();
        m_Materials.shrink_to_fit();
    }

    // �o�E���f�B���O�{�b�N�X�ƃo�E���f�B���O���X�t�B�A�����߂�.
    {
        m_Box = BoundingBox( m_Meshes[0].positions[0] );
        for( size_t i=0; i<m_Meshes.size(); ++i )
        {
            for( size_t j=0; j<m_Meshes[i].positions.size(); ++j )
            { m_Box.Merge( m_Meshes[i].positions[j] );  }
        }

        m_Sphere = BoundingSphere( m_Box );
    }

    // ����I��.
    return true;
}

//-----------------------------------------------------------------------------------------
//      �}�e���A����ݒ肵�܂�.
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
//      ���b�V����`�悵�܂�.
//-----------------------------------------------------------------------------------------
void ModelX::DrawMesh( unsigned int index )
{
    /* �ȈՎ��� */

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
//      ���f����`�悵�܂�.
//-----------------------------------------------------------------------------------------
void ModelX::Draw()
{
    for( size_t i=0; i<m_Meshes.size(); ++i )
    { DrawMesh( i ); }
}

//-----------------------------------------------------------------------------------------
//      ���b�V�����擾���܂�.
//-----------------------------------------------------------------------------------------
std::vector<MeshX>& ModelX::GetMeshes()
{ return m_Meshes; }

//-----------------------------------------------------------------------------------------
//      �}�e���A�����擾���܂�.
//-----------------------------------------------------------------------------------------
std::vector<Material>& ModelX::GetMaterials()
{ return m_Materials; }

//-----------------------------------------------------------------------------------------
//      �o�E���f�B���O�{�b�N�X���擾���܂�.
//-----------------------------------------------------------------------------------------
BoundingBox ModelX::GetBox() const
{ return m_Box; }

//-----------------------------------------------------------------------------------------
//      �o�E���f�B���O�X�t�B�A���擾���܂�.
//-----------------------------------------------------------------------------------------
BoundingSphere ModelX::GetSphere() const
{ return m_Sphere; }
