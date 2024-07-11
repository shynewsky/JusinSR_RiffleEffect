#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUIPortrait : public CGameObject
{
public:
	typedef struct
	{
		int iTalker;	// 이미지 택스쳐 태그

	}SCRIPT_DESC;

public:
	CUIPortrait(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUIPortrait(const CUIPortrait& Prototype);
	virtual ~CUIPortrait() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	void Set_Variables(void* pArg);
	void Set_OrthoLH();
	virtual HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

protected:
	CTexture*				m_pTextureCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

protected:
	/* 직교투영 */
	_float4x4				m_ViewMatrix, m_ProjMatrix;
	int						m_iTalker = 0;


public:
	static CUIPortrait* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END