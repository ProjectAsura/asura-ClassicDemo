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


/////////////////////////////////////////////////////////////////////////////////////////////
// Vertex structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct Vertex
{
    Vec2 texcoord;
    Vec3 normal;
    Vec3 position;

    Vertex()
    { /* DO_NOTHING */ }
};


/////////////////////////////////////////////////////////////////////////////////////////////
// Subset structure
/////////////////////////////////////////////////////////////////////////////////////////////
struct Subset
{
    std::string  materialName;
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
    int         id;
    Vec3        ambient;
    Vec3        diffuse;
    Vec3        specular;
    float       shininess;
    float       alpha;
    std::string ambientMap;
    std::string diffuseMap;
    std::string specularMap;
    std::string bumpMap;

    Material()
    { /* DO_NOTHING */ }

    ~Material()
    {
        ambientMap .clear();
        diffuseMap .clear();
        specularMap.clear();
        bumpMap    .clear();
    }
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
    // type definitions
    //=======================================================================================
    typedef std::vector<Vertex>                 VertexList;
    typedef std::vector<Subset>                 SubsetList;
    typedef std::map<std::string, Material>     MaterialDictionary;
    typedef std::vector<unsigned int>           IndexList;
    typedef VertexList::iterator                VertexListItr;
    typedef VertexList::const_iterator          VertexListCItr;
    typedef SubsetList::iterator                SubsetListItr;
    typedef SubsetList::const_iterator          SubsetListCItr;
    typedef MaterialDictionary::iterator        MaterialDictionaryItr;
    typedef MaterialDictionary::const_iterator  MaterialDictionaryCItr;
    typedef IndexList::iterator                 IndexListItr;
    typedef IndexList::const_iterator           IndexListCItr;

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

    VertexList&                 GetVertices    ();
    SubsetList&                 GetSubsets     ();
    MaterialDictionary&         GetMaterials   ();
    IndexList&                  GetIndices     ();
    const VertexList&           GetVertices    () const;
    const SubsetList&           GetSubsets     () const;
    const MaterialDictionary&   GetMaterials   () const;
    const IndexList&            GetIndices     () const;
    BoundingBox                 GetBox         () const;
    BoundingSphere              GetSphere      () const;

    MeshOBJ& operator = ( const MeshOBJ& value );

protected:
    //======================================================================================
    // protected variables.
    //======================================================================================
    VertexList          m_Vertices;
    SubsetList          m_Subsets;
    MaterialDictionary  m_Materials;
    IndexList           m_Indices;
    BoundingBox         m_Box;
    BoundingSphere      m_Sphere;
    std::string         m_DirectoryPath;

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
