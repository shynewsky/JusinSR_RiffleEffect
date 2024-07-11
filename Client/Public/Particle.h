#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Particle.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CTransform;
class CTexture;
class CShader;
END


BEGIN(Client)

class CParticle : public CGameObject
{
public:
	typedef struct tagStateDesc
	{
		LEVELID				eTextureLevel;
		const _tchar*		strTextureTag;
		_uint				iTextureNum;
		_float3				vVelocity;

	}STATE_DESC;

private:
	CParticle(LPDIRECT3DDEVICE9 pGraphic_Device);
	CParticle(const CParticle& rhs);
	virtual ~CParticle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Components(void* pArg);
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	
private:
	STATE_DESC			m_StateDesc;
public:
	static CParticle* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END