//------------------------------------------------------------------------------------------
// File : Spring.h
// Desc : Spring Simulator Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------------

#ifndef __SPRING_H__
#define __SPRING_H__

//------------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------------
#include <TinyMath.h>


////////////////////////////////////////////////////////////////////////////////////////////
// SIMULATION_TYPE
////////////////////////////////////////////////////////////////////////////////////////////
enum SIMULATION_TYPE
{
    SIMULATION_TYPE_EXPLICIT_EULAR = 0,     //!< 陽的オイラー法.
    SIMULATION_TYPE_VERLET,                 //!< ベルレ法.
};


////////////////////////////////////////////////////////////////////////////////////////////
// Spring1D class
////////////////////////////////////////////////////////////////////////////////////////////
class Spring1D
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
    Spring1D();
    virtual ~Spring1D();

    void SetMass        ( const double value );
    void SetGravity     ( const double value );
    void SetTimeStep    ( const double value );
    void SetLength      ( const double value );
    void SetConstantK   ( const double value );
    void SetInitVelocity( const double value );
    void SetInitPosition( const double value );

    double GetMass      () const;
    double GetGravity   () const;
    double GetTimeStep  () const;
    double GetLength    () const;
    double GetConstantK () const;
    double GetPosition  () const;

    void Update( SIMULATION_TYPE type );

protected:
    //======================================================================================
    // protected variables.
    //======================================================================================
    double  m_Mass;             //!< 質量です.
    double  m_Gravity;          //!< 重力加速度です.
    double  m_TimeStep;         //!< 微小時間です.
    double  m_Length;           //!< 自然長です.
    double  m_ConstantK;        //!< ばね定数です.
    double  m_InitAccel;        //!< 初期加速度です.
    double  m_InitVelocity;     //!< 初期速度です.
    double  m_InitPosition;     //!< 初期位置です.
    double  m_Accel;            //!< 加速度です.
    double  m_Velocity;         //!< 速度です.
    double  m_Position;         //!< 位置です.
    double  m_Force;            //!< 力です.
    double  m_PrevPosition;

    //======================================================================================
    // protected methods.
    //======================================================================================
    void IntegrateExplicitEular();
    void IntegrateVerlet();
    void UpdateAccel();

private:
    //======================================================================================
    // private variables.
    //======================================================================================
    /* NOTHING */

    //======================================================================================
    // private methods.
    //======================================================================================
    Spring1D        ( const Spring1D& value );  // アクセス禁止.
    void operator = ( const Spring1D& value );  // アクセス禁止.
};


#endif//__SPRING_H__
