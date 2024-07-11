#pragma once
#include "Client_Defines.h"
#include "LandObject.h"
#include "FSM.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
END

BEGIN(Client)
class CPlayer_SM :
    public CLandObject
{
private:
    CPlayer_SM(LPDIRECT3DDEVICE9 pGrapihc_Device);
    CPlayer_SM(const CPlayer_SM& Prototype);
    virtual ~CPlayer_SM() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Move_Player(_float3 _vDir);

private:
	//component
	CTexture* m_pTextureCom[CFSM::OBJSTATE_END] = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };
	CFSM* m_pFSM = { nullptr };

private:
	HRESULT Ready_Component();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	HRESULT SetUp_FSM();
	void Key_Input(_float _fTimedelta);

private:
	_float					m_fActCooltime = { 0.f };
	_float4x4				m_CollisionMatrix;
	WAY						m_eMoveWay;
public:
	static CPlayer_SM* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END