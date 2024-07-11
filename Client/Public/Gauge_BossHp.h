#pragma once
#include "Gauge.h"

class CGauge_BossHp :
    public CGauge
{
private:
    CGauge_BossHp(LPDIRECT3DDEVICE9 _pGraphic_Device);
    CGauge_BossHp(const CGauge_BossHp& Prototype);
	virtual ~CGauge_BossHp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_OrthoLH();
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	virtual void KillSwitch();

protected:
	virtual 	HRESULT SetUp_FSM() override { return S_OK; }

public:
	static CGauge_BossHp* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};


