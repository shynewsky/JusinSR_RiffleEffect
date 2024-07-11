#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Image.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CGunSlotSelect final : public CImage
{
public:
	enum SLOT_NUM { SLOT1, SLOT2, SLOT3, SLOT_END };

private:
	CGunSlotSelect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGunSlotSelect(const CGunSlotSelect& Prototype);
	virtual ~CGunSlotSelect() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	void Set_SlotNum(SLOT_NUM eSlotNum) { m_eSlotNum = eSlotNum; }
	void Set_SlotPos();

private:
	SLOT_NUM m_eSlotNum = SLOT_END;

public:
	static CGunSlotSelect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END