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


//-------------------------------------------------------------------------------------------
// Constant Values
//-------------------------------------------------------------------------------------------
static const unsigned int OBJ_BUFFER_LENGTH = 1024;
static const unsigned int OBJ_NAME_LENGTH   = 256;


/////////////////////////////////////////////////////////////////////////////////////////////
// OBJVERTEX
/////////////////////////////////////////////////////////////////////////////////////////////
struct OBJVERTEX
{
    Vec3 position;
    Vec3 normal;
    Vec2 texcoord;

    OBJVERTEX()
    { /* DO_NOTHING */ }
};

/////////////////////////////////////////////////////////////////////////////////////////////
// OBJSUBSET
/////////////////////////////////////////////////////////////////////////////////////////////
struct OBJSUBSET
{
    unsigned int materialID;
    unsigned int offset;
    unsigned int count;

    OBJSUBSET()
    { /* DO_NOTHING */ }
};

/////////////////////////////////////////////////////////////////////////////////////////////
// OBJMATERIAL
/////////////////////////////////////////////////////////////////////////////////////////////
struct OBJMATERIAL
{
    char    name[OBJ_NAME_LENGTH];
    Vec3    ambient;
    Vec3    diffuse;
    Vec3    specular;
    float   shininess;
    float   alpha;
    char    ambientMapName [OBJ_NAME_LENGTH];
    char    diffuseMapName [OBJ_NAME_LENGTH];
    char    specularMapName[OBJ_NAME_LENGTH];
    char    bumpMapName    [OBJ_NAME_LENGTH];

    OBJMATERIAL()
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
    MeshOBJ();
    virtual ~MeshOBJ();

    bool LoadFromFile( const char* filename );
    void Release     ();
    void Draw        ();

    unsigned int        GetNumVertices  ();
    unsigned int        GetNumSubsets   ();
    unsigned int        GetNumMaterials ();
    unsigned int        GetNumIndices   ();
    unsigned int        GetIndexData    ( unsigned int index );
    OBJVERTEX           GetVertex       ( unsigned int index );
    OBJSUBSET           GetSubset       ( unsigned int index );
    OBJMATERIAL         GetMaterial     ( unsigned int index );
    OBJVERTEX*          GetVertices     ();
    OBJSUBSET*          GetSubsets      ();
    OBJMATERIAL*        GetMaterials    ();
    unsigned int*       GetIndices      ();
    BoundingBox         GetBox          ();
    BoundingSphere      GetSphere       ();

protected:
    OBJVERTEX*          m_Vertices;
    OBJSUBSET*          m_Subsets;
    OBJMATERIAL*        m_Materials;
    unsigned int*       m_Indices;
    unsigned int        m_NumVertices;
    unsigned int        m_NumSubsets;
    unsigned int        m_NumMaterials;
    unsigned int        m_NumIndices;
    BoundingBox         m_Box;
    BoundingSphere      m_Sphere;

    bool LoadMTLFile( const char* filename );
    bool LoadOBJFile( const char* filename );

private:


    MeshOBJ         ( const MeshOBJ& value );
    void operator = ( const MeshOBJ& value );


};

#endif//__MESH_OBJ_H__
