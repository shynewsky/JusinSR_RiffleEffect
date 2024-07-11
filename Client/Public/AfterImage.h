#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CShader;
END

BEGIN(Client)

class CAfterImage final : public CBlendObject
{
public:
	typedef struct
	{
		_float3 vPos;
		int iCurTexIndex = 0; 
		const _tchar* pTextureTag;
		int iType;
		LEVELID eTextureLevel;
		_float3 Scale = _float3(1.f, 1.f, 1.f);
	}AFTER_DESC;

private:
	CAfterImage(LPDIRECT3DDEVICE9 pGraphic_Device);
	CAfterImage(const CAfterImage& Prototype);
	virtual ~CAfterImage() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*				m_pTextureCom = { nullptr };	
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };


private:
	_float					m_fSumTime = 0.f;
	AFTER_DESC				m_tDesc = {};
private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	static CAfterImage* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END