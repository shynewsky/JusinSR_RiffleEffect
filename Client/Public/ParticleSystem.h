#pragma once
#include "Particle.h"
#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CShader;
END

BEGIN(Client)

// 파티클 갱신과 디스플레이, 소멸, 생성등을 관장하는 역할을 한다 
class CParticleSystem final : public CBlendObject
{
public:
	enum PARTICLE_TYPE { PARTICLE_FLARE, PARTICLE_SPARK, PARTICLE_TEST, PARTICLE_WEAPON, PARTICLE_FLAME,PARTICLE_END };


public:
	typedef struct tagStateDesc
	{
		PARTICLE_TYPE	eType;
		// 파티클 텍스쳐
		LEVELID				eTextureLevel;
		const _tchar*		strTextureTag;
		_uint				iTextureNum;

		// 파티클 생존정보
		_bool				bActive;
		_double				dDuration;

		// 파티클 정보
		_float2				vParticleScale;//크기
		_float3				vPosition;//위치
		_float3				vVelocity;//속도
		_float				fVelocityDeviation;//속도 편차

		//전체 파티클 정보
		_int				iMaxParticleCount; //최대 파티클 정보
		_double				dParticleLifeTime; // 최대 파티클 생존정보

		// 시간
		_double				dSpawnTime;
		_float				fParticlePerSecond; //시간
		_float3				vParticleDeviation; // 파티클 편차
	}STATE_DESC;  /* 기준으로 잡을 파티클 정보*/


	typedef struct tagParticleInfo
	{
		// Particle Info
		_bool			bActive;

		_float3			vVelocity;
		_float3			vPosition;

		_double		dCurrentLifeTime;
	}PARTICLE_INFO; /*현재 파티클 정보*/

private:
	CParticleSystem(LPDIRECT3DDEVICE9 pGraphic_Device);
	CParticleSystem(const CParticleSystem& prototype);
	virtual ~CParticleSystem() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	HRESULT Ready_Components(void* pArg = nullptr);
	HRESULT Ready_ParticleSystem();
	HRESULT Ready_VIBuffer();
	HRESULT Update_Particles(_float fTimeDelta);
	HRESULT Update_VIBuffer();
	HRESULT Render_VIBuffer();
	
public:
	HRESULT Remove_DeadParticles();
	

public:
	HRESULT Particle_Flare_Play(_float fTimeDelta);
	HRESULT Particle_Spark_Play(_float fTimeDelta);
	HRESULT Particle_TEST_Play(_float fTimeDelta);
	HRESULT Particle_Weapon_Play(_float fTimeDelta);
	HRESULT Particle_Flame_Play(_float fTimeDelta);


public:
	_bool Check_AllParticleDead();
	_float Calculate_ViewZ(_float3 WorldPos);
	void SetUp_BillBoard();

private:
	CTransform*			m_pTransformCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CShader*			m_pShaderCom = nullptr;

	STATE_DESC		m_StateDesc;

private:
	void Set_OrthoLH();
	_float4x4				m_ViewMatrix, m_ProjMatrix;
	_float4x4				m_PrevViewMatrix, m_PrevProjMatrix;

private:
	_bool		m_bStart = false;
	_double		m_dCurrentDuration = 0;
	_double		m_dCurrentSpawnTime = 0;
	_int		m_iCurrentParticleCount = 0;

private:
	PARTICLE_INFO*			m_ParticleArray = nullptr;		// 파티클의 정보들
	VTXTEX*					m_Vertices = nullptr;			// 정점들의 정보들

protected: // 버텍스 버퍼 셋팅용
	LPDIRECT3DVERTEXBUFFER9		m_pVBuffer = nullptr;
	_uint			m_iStride = 0;
	_uint			m_iNumVertices = 0;
	_uint			m_iNumPrimitive = 0;
	_ulong			m_dwFVF = 0;

public:
	static CParticleSystem* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END