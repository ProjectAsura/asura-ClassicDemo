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
    SIMULATION_TYPE_EXPLICIT_EULAR = 0,     //!< �z�I�I�C���[�@.
    SIMULATION_TYPE_VERLET,                 //!< �x�����@.
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
    double  m_Mass;             //!< ���ʂł�.
    double  m_Gravity;          //!< �d�͉����x�ł�.
    double  m_TimeStep;         //!< �������Ԃł�.
    double  m_Length;           //!< ���R���ł�.
    double  m_ConstantK;        //!< �΂˒萔�ł�.
    double  m_InitAccel;        //!< ���������x�ł�.
    double  m_InitVelocity;     //!< �������x�ł�.
    double  m_InitPosition;     //!< �����ʒu�ł�.
    double  m_Accel;            //!< �����x�ł�.
    double  m_Velocity;         //!< ���x�ł�.
    double  m_Position;         //!< �ʒu�ł�.
    double  m_Force;            //!< �͂ł�.
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
    Spring1D        ( const Spring1D& value );  // �A�N�Z�X�֎~.
    void operator = ( const Spring1D& value );  // �A�N�Z�X�֎~.
};


#endif//__SPRING_H__
