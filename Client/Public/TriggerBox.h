#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Cube;
class CCollider_Cube;
END

BEGIN(Client)

class CTriggerBox final : public CGameObject
{
public:
	enum EVENT_TYPE
	{
		ET_CHANGE_LEVEL,
		ET_START_SCRIPT,
		ET_SPAWN_MONSTER,
		ET_END
	};
	typedef struct TriggerInfo
	{
		EVENT_TYPE eType;
		_float3 pos;
		int iCount;
	}TRIGGER_INFO;
private:
	CTriggerBox(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTriggerBox(const CTriggerBox& Prototype);
	virtual ~CTriggerBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	EVENT_TYPE Get_Type() { return m_eType; }
	bool Get_EventState() { return m_bEvent; }
	int Get_EventCount() { return m_iCount; }

private:
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };
	_float4x4				m_CollisionMatrix;

	EVENT_TYPE				m_eType = ET_END;
	int						m_iCount = 0;
	bool					m_bEvent = false;
	bool					m_bFirst = false;

private:
	HRESULT Ready_Components();

public:
	static CTriggerBox* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END