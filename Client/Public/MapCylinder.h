#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Cube;
class CCollider_Cube;
END

BEGIN(Client)

class CMapCylinder final : public CGameObject
{
private:
	CMapCylinder(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMapCylinder(const CMapCylinder& Prototype);
	virtual ~CMapCylinder() = default;

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
	CVIBuffer_Cube*			m_pVIBufferCom = { nullptr };
	CCollider_Cube*			m_pColliderCom = { nullptr };
	_float4x4				m_CollisionMatrix;

private:
	HRESULT Ready_Components();


public:
	static CMapCylinder* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END