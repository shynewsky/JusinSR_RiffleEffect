#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CLandObject abstract : public CGameObject
{
public:
	typedef struct
	{
		CVIBuffer_Terrain*		pTerrainVIBuffer = { nullptr };
		CTransform*				pTerrainTransform = { nullptr };
		LEVELID					eLevel = { LEVEL_GAMEPLAY };
	}LANDOBJECT_DESC;
protected:
	CLandObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLandObject(const CLandObject& Prototype);
	virtual ~CLandObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void KillSwitch() override {};

public:
	virtual HRESULT SetUp_OnTerrain(CTransform* pTransform, _float fOffsetY = 0.f);
	virtual bool SetUp_UnderTerrain(CTransform* pTransform, _float fOffsetY = 0.f); //지형 아래일 경우만 위로 올려줌? True:False

protected:
	CVIBuffer_Terrain*				m_pTerrainVIBuffer = { nullptr };
	CTransform*						m_pTerrainTransform = { nullptr };
	LEVELID		m_eCurLevel;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END