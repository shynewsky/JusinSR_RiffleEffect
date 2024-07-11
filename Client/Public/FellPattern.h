#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;

END

BEGIN(Client)

class CFellPattern final : public CGameObject
{
	enum PATTERN_STATE
	{
		PATTERN_SPAWN,
		PATTERN_IDLE,
		PATTERN_ATTACK,
		PATTERN_DEATH,
		PATTERN_END,
	};

public:
	typedef struct
	{
		_float3 vPos;
	}PATTERN_DESC;
private:
	CFellPattern(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFellPattern(const CFellPattern& Prototype);
	virtual ~CFellPattern() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Update_Spawn(_float fTimeDelta);
	void Update_Idle(_float fTimeDelta);
	void Update_Charge(_float fTimeDelta);
	void Update_Death(_float fTimeDelta);

public:

private:
	CTexture* m_pTextureCom[PATTERN_END] = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider_Cube* m_pColliderCom = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	PATTERN_STATE			m_eState = PATTERN_SPAWN;
	_float4x4				m_CollisionMatrix;
	_float					m_fSumTime = 0.f;
	_int					m_iAttackCount = 0;

public:
	static CFellPattern* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END