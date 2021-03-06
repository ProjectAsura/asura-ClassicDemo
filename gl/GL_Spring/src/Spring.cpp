﻿//----------------------------------------------------------------------------------------
// File : Spring.cpp
// Desc : Spring Simulator Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------------
#include <Spring.h>


//////////////////////////////////////////////////////////////////////////////////////////
// Spring1D class
//////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
//      コンストラクタです.
//----------------------------------------------------------------------------------------
Spring1D::Spring1D()
: m_Mass        ( 1.0 )
, m_Gravity     ( 9.8 )
, m_TimeStep    ( 0.00001 )
, m_Length      ( 0.0f )
, m_ConstantK   ( 0.0 )
, m_InitAccel   ( 0.0 )
, m_InitVelocity( 0.0 )
, m_InitPosition( 0.0 )
, m_Accel       ( 0.0 )
, m_Velocity    ( 0.0 )
, m_Force       ( 0.0 )
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------------
//      デストラクタです.
//----------------------------------------------------------------------------------------
Spring1D::~Spring1D()
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------------
//      更新処理を行います.
//----------------------------------------------------------------------------------------
void Spring1D::Update( SIMULATION_TYPE type )
{
    // 加速度を更新.
    UpdateAccel();

    switch( type )
    {
    // 陽的オイラー法で更新.
    case SIMULATION_TYPE_EXPLICIT_EULAR:
        { IntegrateExplicitEular(); }
        break;

    // ベルレ法で更新.
    case SIMULATION_TYPE_VERLET:
        { IntegrateVerlet(); }
        break;
    }
}

//----------------------------------------------------------------------------------------
//      加速度を更新します.
//----------------------------------------------------------------------------------------
void Spring1D::UpdateAccel()
{
    // フックの法則により力を求める.
    m_Force = - m_ConstantK * ( m_Position - m_Length ) + m_Mass * m_Gravity;

    // 加速度を求める.
    m_Accel = m_Force / m_Mass;
}

//----------------------------------------------------------------------------------------
//      陽的オイラー法による積分計算を行います.
//----------------------------------------------------------------------------------------
void Spring1D::IntegrateExplicitEular()
{
    // 前の位置を更新.
    m_PrevPosition = m_Position;

    // 陽的オイラー法を適用.
    m_Velocity += m_Accel    * m_TimeStep;
    m_Position += m_Velocity * m_TimeStep;
}

//----------------------------------------------------------------------------------------
//      ベルレ法による積分計算を行います.
//----------------------------------------------------------------------------------------
void Spring1D::IntegrateVerlet()
{
    // ベルレ法を適用.
    double newPos  = 2.0f * m_Position - m_PrevPosition + m_Accel * ( m_TimeStep * m_TimeStep );

    // 速度を更新.
    m_Velocity = ( newPos - m_PrevPosition ) / ( 2.0 * m_TimeStep );

    // 位置を更新する.
    m_PrevPosition = m_Position;
    m_Position     = newPos;
}

//----------------------------------------------------------------------------------------
//      質量を設定します.
//----------------------------------------------------------------------------------------
void Spring1D::SetMass( const double value )
{ m_Mass = value; }

//----------------------------------------------------------------------------------------
//      重力加速度を設定します.
//----------------------------------------------------------------------------------------
void Spring1D::SetGravity( const double value )
{ m_Gravity = value; }

//----------------------------------------------------------------------------------------
//      タイムステップ(微小時間）を設定します.
//----------------------------------------------------------------------------------------
void Spring1D::SetTimeStep( const double value )
{ m_TimeStep = value; }

//----------------------------------------------------------------------------------------
//      自然長を設定します.
//----------------------------------------------------------------------------------------
void Spring1D::SetLength( const double value )
{ m_Length = value; }

//----------------------------------------------------------------------------------------
//      ばね定数を設定します.
//----------------------------------------------------------------------------------------
void Spring1D::SetConstantK( const double value )
{ m_ConstantK = value; }

//----------------------------------------------------------------------------------------
//      初期速度を設定します.
//----------------------------------------------------------------------------------------
void Spring1D::SetInitVelocity( const double value )
{
    m_InitVelocity = value;
    m_Velocity = m_InitVelocity;
}

//----------------------------------------------------------------------------------------
//      初期位置を設定します.
//----------------------------------------------------------------------------------------
void Spring1D::SetInitPosition( const double value )
{
    m_InitPosition = value;
    m_Position = m_InitPosition;
    m_PrevPosition = m_Position;
}

//----------------------------------------------------------------------------------------
//      質量を取得します.
//----------------------------------------------------------------------------------------
double Spring1D::GetMass() const
{ return m_Mass; }

//----------------------------------------------------------------------------------------
//      重力加速度を取得します.
//----------------------------------------------------------------------------------------
double Spring1D::GetGravity() const
{ return m_Gravity; }

//----------------------------------------------------------------------------------------
//      タイムステップを取得します.
//----------------------------------------------------------------------------------------
double Spring1D::GetTimeStep() const
{ return m_TimeStep; }

//----------------------------------------------------------------------------------------
//      自然長を取得します.
//----------------------------------------------------------------------------------------
double Spring1D::GetLength() const
{ return m_Length; }

//----------------------------------------------------------------------------------------
//      ばね定数を取得します.
//----------------------------------------------------------------------------------------
double Spring1D::GetConstantK() const
{ return m_ConstantK; }

//----------------------------------------------------------------------------------------
//      位置座標を取得します.
//----------------------------------------------------------------------------------------
double Spring1D::GetPosition() const
{ return m_Position; }

