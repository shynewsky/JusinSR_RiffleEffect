#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CLoadingBG final : public CGameObject
{
private:
	CLoadingBG(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLoadingBG(const CLoadingBG& Prototype);
	virtual ~CLoadingBG() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Set_Variables();
	void Set_OrthoLH();
	HRESULT Ready_Components();

private:
	CTexture*				m_pTextureCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

private:
	/* 어떤 위치(fX, fY)에 어떤 사이즈(fSizeX, fSizeY)로 그릴건지를 설정한 데이터다.  */
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

private:
	_float4x4				m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	_float					m_fFrame = {};

public:
	static CLoadingBG* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END