#pragma once
#include "Gauge.h"
class CGauge_Search_Scan :
    public CGauge
{
private:
	CGauge_Search_Scan(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CGauge_Search_Scan(const CGauge_Search_Scan& Prototype);
	virtual ~CGauge_Search_Scan() = default;

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
	HRESULT Set_Font();

protected:
	virtual 	HRESULT SetUp_FSM() override { return S_OK; }

private:
	ID3DXFont* m_pFont = { nullptr };
	LPWSTR m_FontName;
public:
	static CGauge_Search_Scan* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};