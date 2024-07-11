#pragma once

#include "Base.h"

/* 모든 게임내에 사용되는 게임 오브젝트들의 부모 클래스다. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;
	
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual int Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	
public:
	class CComponent* Find_Component(const _wstring& strComponentTag);
	virtual void IsPicked(_float3* pOutPos, _float* pDist) {}

public:
	void	Set_Dead(_bool bDead) { m_bDead = bDead; }
	_bool	Get_Dead() { return m_bDead; }

	void	Set_Pos(_float3& _pos);
	_float3	Get_Pos();
	virtual void KillSwitch() {};

protected:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };


protected:
	map<const _wstring, class CComponent*>			m_Components;

protected:
	HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

protected:
	_bool	m_bDead = false;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END