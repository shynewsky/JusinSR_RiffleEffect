#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPotal final : public CBlendObject
{
public:
	typedef struct
	{
		_float3 vPos;

	}POTAL_DESC;

private:
	CPotal(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPotal(const CPotal& Prototype);
	virtual ~CPotal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Scaling(_float _fScalex, _float _fScaley, _float _fScalez);
	void Lie_On();

private:
	CTexture*		m_pTextureCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };
	CVIBuffer_Rect*	m_pVIBufferCom = { nullptr };

private:
	_float		m_fTime = 0.f;
	_float		m_fScale = 1.f;
	POTAL_DESC m_tEffectInfo = {};

private:
	void Set_Variables(void* pArg);
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	static CPotal* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END