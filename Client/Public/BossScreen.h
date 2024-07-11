#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CBossScreen final : public CGameObject
{
public:
	typedef struct
	{
		LEVELID eCurLevel;
	}BOSSUI_DESC;

private:
	CBossScreen(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBossScreen(const CBossScreen& Prototype);
	virtual ~CBossScreen() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BossBG();
	HRESULT Ready_Layer_Gauge();
	HRESULT Ready_Layer_Frame();
	HRESULT Ready_Layer_Face();

private:
	BOSSUI_DESC m_tGameUIInfo;
	CGameObject* Target = {nullptr};
	CGameObject* pBossBG = {nullptr};
	CGameObject* pBossGauge = {nullptr};
	CGameObject* pBossFrame = {nullptr};
	CGameObject* pFace = { nullptr };


public:
	static CBossScreen* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END