#pragma region File Description
//-----------------------------------------------------------------------
// File : OBJLoader.h
// Desc : Alias Wavefron OBJ File Loader
// Date : Mar. 04, 2009
// Version : 2.1
// Author : Pocol
// Memo : 4角形対応済み。5角形以上はサポートしない
//-----------------------------------------------------------------------
#pragma endregion

#ifndef _OBJ_LOADER_H_INCLUDED_
#define _OBJ_LOADER_H_INCLUDED_


//
// Includes
//

#include <string>
#include <TinyMath.h>

//
// Using Statements
//



//
// Defines
//
#define OBJ_BUFFER_LENGTH 1024
#define OBJ_NAME_LENGTH   256
//
//
///////////////////////////////////////////////////////////////////////////
//// Vec2
///////////////////////////////////////////////////////////////////////////
//struct Vec2
//{
//    float x;
//    float y;
//    Vec2( float nx=0.0f, float ny=0.0f );
//    operator float* ();
//    operator const float* () const;
//};
//
///////////////////////////////////////////////////////////////////////////
//// Vec3
///////////////////////////////////////////////////////////////////////////
//struct Vec3
//{
//    float x;
//    float y;
//    float z;
//    Vec3( float nx=0.0f, float ny=0.0f, float nz=0.0f );
//    operator float* ();
//    operator const float* () const;
//};

/////////////////////////////////////////////////////////////////////////
// Vertex
/////////////////////////////////////////////////////////////////////////
struct Vertex
{
    Vec2 texcoord;
    Vec3 normal;
    Vec3 position;
    Vertex(){}
};

/////////////////////////////////////////////////////////////////////////
// Subset
/////////////////////////////////////////////////////////////////////////
struct Subset
{
    unsigned int materialIndex;
    unsigned int faceStart;
    unsigned int faceCount;
    Subset(){}
};

/////////////////////////////////////////////////////////////////////////
// Material
/////////////////////////////////////////////////////////////////////////
struct Material
{
    char name[OBJ_NAME_LENGTH];
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
    float shininess;
    float alpha;
    char ambientMapName[OBJ_NAME_LENGTH];
    char diffuseMapName[OBJ_NAME_LENGTH];
    char specularMapName[OBJ_NAME_LENGTH];
    char bumpMapName[OBJ_NAME_LENGTH];
    Material(){}
};
//
///////////////////////////////////////////////////////////////////////////
//// BoundingBox
///////////////////////////////////////////////////////////////////////////
//struct BoundingBox
//{
//    Vec3 maximum;
//    Vec3 minimum;
//    Vec3 size;
//    void Merge( Vec3 value );
//    BoundingBox() {}
//    BoundingBox( Vec3 value );
//};
//
///////////////////////////////////////////////////////////////////////////
//// BoundingSphere
///////////////////////////////////////////////////////////////////////////
//struct BoundingSphere
//{
//    Vec3 center;
//    float radius;
//    void Create( BoundingBox box );
//    BoundingSphere() {}
//};

/////////////////////////////////////////////////////////////////////////
// MeshOBJ
/////////////////////////////////////////////////////////////////////////
class MeshOBJ
{
private:
    Vertex* m_Vertices;
    Subset* m_Subsets;
    Material* m_Materials;
    unsigned int* m_Indices;
    unsigned int m_NumVertices;
    unsigned int m_NumSubsets;
    unsigned int m_NumMaterials;
    unsigned int m_NumIndices;
    BoundingBox m_Box;
    BoundingSphere m_Sphere;
    std::string m_DirectoryPath;

    bool LoadMTLFile( const char* filename );
    bool LoadOBJFile( const char* filename );

public:
    MeshOBJ();
    ~MeshOBJ();

    bool LoadFile( const char* filename );
    void Release();
    void Draw();
    unsigned int GetNumVertices();
    unsigned int GetNumSubsets();
    unsigned int GetNumMaterials();
    unsigned int GetNumIndices();
    unsigned int  GetIndexData( unsigned int index );
    unsigned int* GetIndices();
    Vertex  GetVertex( unsigned int index );
    Vertex* GetVertices();
    Subset  GetSubset( unsigned int index );
    Subset* GetSubsets();
    Material  GetMaterial( unsigned int index );
    Material* GetMaterials();
    BoundingBox GetBox();
    BoundingSphere GetSphere();
};

#endif	//　_OBJ_LOADER_H_INCLUDED_