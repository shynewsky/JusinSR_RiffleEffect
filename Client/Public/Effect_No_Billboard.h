#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_No_Billboard final : public CBlendObject
{
public:
	typedef struct
	{
		const _tchar* tFrameTextureTag;	// 이미지 택스쳐 태그
		_float iFrameSpeed = 1.f;			// 이미지 순회주기
	}EFFECT_DESC;

private:
	CEffect_No_Billboard(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_No_Billboard(const CEffect_No_Billboard& Prototype);
	virtual ~CEffect_No_Billboard() = default;

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
	_float		m_fFrame = 0.f;
	EFFECT_DESC m_tEffectInfo = {};

private:
	void Set_Variables(void* pArg);
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	static CEffect_No_Billboard* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END