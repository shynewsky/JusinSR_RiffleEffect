#pragma once
#include "RenderingObject.h"

class CGunEffect :
    public CRenderingObject
{
public:
	typedef struct
	{
		const _tchar* tFrameTextureTag;	// 이미지 택스쳐 태그
		_float2 vPos;
	}GF_DESC;
private:
	CGunEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGunEffect(const CGunEffect& Prototype);
	virtual ~CGunEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	virtual HRESULT	Add_RenderObject(class CGameObject* pRenderObject) override;

	virtual 	HRESULT SetUp_FSM() { return S_OK; }

private:
	_float		m_fFrame = 0.f;
	GF_DESC     m_tDESC;

public:
	static CGunEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

