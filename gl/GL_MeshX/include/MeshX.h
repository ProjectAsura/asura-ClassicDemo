//------------------------------------------------------------------------------------------
// File : MeshX.h
// Desc : X File Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

#ifndef __MESH_X_H__
#define __MESH_X_H__


//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include <TinyMath.h>
#include <string>
#include <vector>


////////////////////////////////////////////////////////////////////////////////////////////
// Face structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Face
{
    int element;        //!< 要素素です.
    int indexM;         //!< マテリアル番号です.
    int indexP[ 4 ];    //!< 位置座標番号です.
    int indexN[ 4 ];    //!< 法線ベクトル番号です.
    int indexU[ 4 ];    //!< テクスチャ座標番号です.

    //--------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //--------------------------------------------------------------------------------------
    Face()
    : element( 0 )
    , indexM ( 0 )
    {
        indexP[0] = indexP[1] = indexP[2] = indexP[3] = -1;
        indexN[0] = indexN[1] = indexN[2] = indexN[3] = -1;
        indexU[0] = indexU[1] = indexU[2] = indexU[3] = -1;
    }

    //--------------------------------------------------------------------------------------
    //! @brief      コピーコンストラクタです.
    //--------------------------------------------------------------------------------------
    Face( const Face& value )
    : element( value.element )
    , indexM ( value.indexM )
    {
        for( int i=0; i<4; ++i )
        {
            indexP[ i ] = value.indexP[ i ];
            indexN[ i ] = value.indexN[ i ];
            indexU[ i ] = value.indexU[ i ];
        }
    }
};


////////////////////////////////////////////////////////////////////////////////////////////
// Material structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Material
{
    std::string name;       //!< マテリアル名です.
    Vec4        ambient;    //!< 環境色です.
    Vec4        diffuse;    //!< 拡散反射色です.
    Vec4        specular;   //!< 鏡面反射色です.
    Vec4        emissive;   //!< 自己発光色です.
    float       power;      //!< 鏡面反射強度です.
    std::string texture;    //!< テクスチャ名です.

    //--------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //--------------------------------------------------------------------------------------
    Material()
    : name      ()
    , ambient   ()
    , diffuse   ()
    , specular  ()
    , emissive  ()
    , power     ( 0.0f )
    , texture   ()
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------------
    //! @brief      コピーコンストラクタです.
    //--------------------------------------------------------------------------------------
    Material( const Material& value )
    : name      ( value.name )
    , ambient   ( value.ambient )
    , diffuse   ( value.diffuse )
    , specular  ( value.specular )
    , emissive  ( value.emissive )
    , power     ( value.power )
    , texture   ( value.texture )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //--------------------------------------------------------------------------------------
    ~Material()
    {
        name   .clear();
        texture.clear();
    }
};


////////////////////////////////////////////////////////////////////////////////////////////
// MeshX structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MeshX
{
    std::vector<Vec3>   positions;      //!< 位置座標です.
    std::vector<Vec3>   normals;        //!< 法線ベクトルです.
    std::vector<Vec2>   texcoords;      //!< テクスチャ座標です.
    std::vector<Face>   faces;          //!< 面です.

    //--------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //--------------------------------------------------------------------------------------
    MeshX()
    : positions ()
    , normals   ()
    , texcoords ()
    , faces     ()
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------------
    //! @brief      コピーコンストラクタです.
    //--------------------------------------------------------------------------------------
    MeshX( const MeshX& value )
    : positions ( value.positions )
    , normals   ( value.normals )
    , texcoords ( value.texcoords )
    , faces     ( value.faces )
    { /* DO_NOTHING */ }

    //--------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //--------------------------------------------------------------------------------------
    ~MeshX()
    { Release(); }

    //--------------------------------------------------------------------------------------
    //! @brief      解放処理を行います.
    //--------------------------------------------------------------------------------------
    void Release()
    {
        positions.clear();
        normals  .clear();
        texcoords.clear();
        faces    .clear();
    }
};


////////////////////////////////////////////////////////////////////////////////////////////
// ModelX class
////////////////////////////////////////////////////////////////////////////////////////////
class ModelX
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
    ModelX();
    ModelX( const ModelX& value );
    virtual ~ModelX();

    bool LoadFromFile( const char* filename );
    void Release();
    void Draw   ();

    std::vector<MeshX>&     GetMeshes   ();
    std::vector<Material>&  GetMaterials();
    BoundingBox             GetBox      () const;
    BoundingSphere          GetSphere   () const;

protected:
    //======================================================================================
    // protected variables.
    //======================================================================================
    std::vector<MeshX>      m_Meshes;
    std::vector<Material>   m_Materials;
    BoundingBox             m_Box;
    BoundingSphere          m_Sphere;

    //======================================================================================
    // protected methods.
    //======================================================================================
    void DrawMesh   ( unsigned int index );
    void SetMaterial( const Material& material );

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

#endif//__MESH_X_H__
