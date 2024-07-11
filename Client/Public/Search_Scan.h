#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "RenderingObject.h"

BEGIN(Client)

class CSearch_Scan :
    public CRenderingObject
{
protected:
	CSearch_Scan(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSearch_Scan(const CSearch_Scan& Prototype);
	virtual ~CSearch_Scan() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_State(CFSM::OBJSTATE _eState) override;
	_float	Get_SearchValue() { return m_fSearchValue; }

private:
	CTexture* m_pTextureCom[CFSM::OBJSTATE_END] = { nullptr };
	CGameObject* m_pPlayer = { nullptr };
	CGameObject* m_pScan = { nullptr };
	_float	m_fDeltaPlayer = { 0.f };
	_float	m_fSearchValue = { 0.f }; // 최대치 60 (1분)
	_float	m_fMaxValue = { 60.f };


private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	virtual HRESULT SetUp_FSM();
	void	Compute_DeltaPlayer(_float _fTimeDelta);
	HRESULT Clone_Gauge();

public:
	static CSearch_Scan* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
