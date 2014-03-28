//-------------------------------------------------------------------------------------------
// File : MeshOBJ.h
// Desc : Wavefront Object File Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

#ifndef __MESH_OBJ_H__
#define __MESH_OBJ_H__

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <TinyMath.h>
#include <vector>
#include <map>
#include <string>

static const unsigned int NAME_LENGTH = 256;

/////////////////////////////////////////////////////////////////////////////////////////////
// Vertex structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct Vertex
{
    Vec3 position;
    Vec3 normal;
    Vec2 texcoord;

    Vertex()
    { /* DO_NOTHING */ }
};


/////////////////////////////////////////////////////////////////////////////////////////////
// Subset structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct Subset
{
    unsigned int materialID;
    unsigned int offset;
    unsigned int count;

    Subset()
    { /* DO_NOTHING */ }
};


/////////////////////////////////////////////////////////////////////////////////////////////
// Material structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct Material
{
    char        name[ NAME_LENGTH ];
    Vec3        ambient;
    Vec3        diffuse;
    Vec3        specular;
    float       shininess;
    float       alpha;
    char        ambientMap [ NAME_LENGTH ];
    char        diffuseMap [ NAME_LENGTH ];
    char        specularMap[ NAME_LENGTH ];
    char        bumpMap    [ NAME_LENGTH ];

    Material()
    { /* DO_NOTHING */ }
};


/////////////////////////////////////////////////////////////////////////////////////////////
// MeshOBJ class
/////////////////////////////////////////////////////////////////////////////////////////////
class MeshOBJ
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
    MeshOBJ();
    MeshOBJ( const MeshOBJ& value );
    virtual ~MeshOBJ();

    bool LoadFromFile( const char* filename );
    void Release     ();
    void Draw        ();

    unsigned int    GetNumVertices  ();
    unsigned int    GetNumSubsets   ();
    unsigned int    GetNumMaterials ();
    unsigned int    GetNumIndices   ();
    unsigned int    GetIndexData    ( unsigned int index );
    unsigned int*   GetIndices      ();
    Vertex          GetVertex       ( unsigned int index );
    Vertex*         GetVertices     ();
    Subset          GetSubset       ( unsigned int index );
    Subset*         GetSubsets      ();
    Material        GetMaterial     ( unsigned int index );
    Material*       GetMaterials    ();
    BoundingBox     GetBox          () const;
    BoundingSphere  GetSphere       () const;

    MeshOBJ& operator = ( const MeshOBJ& value );

protected:
    //======================================================================================
    // protected variables.
    //======================================================================================
    Vertex*         m_Vertices;
    Subset*         m_Subsets;
    Material*       m_Materials;
    unsigned int*   m_Indices;
    unsigned int    m_NumVertices;
    unsigned int    m_NumSubsets;
    unsigned int    m_NumMaterials;
    unsigned int    m_NumIndices;
    BoundingBox     m_Box;
    BoundingSphere  m_Sphere;
    std::string     m_DirectoryPath;

    //======================================================================================
    // protected methods.
    //======================================================================================
    bool LoadMTLFile( const char* filename );
    bool LoadOBJFile( const char* filename );

private:
    //======================================================================================
    // private variables.
    //======================================================================================
    /* NOTHING */

    //======================================================================================
    // private methods.
    //======================================================================================
    /* NOTHING */
};

#endif//__MESH_OBJ_H__
