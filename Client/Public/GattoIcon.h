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

class CGattoIcon final : public CImage
{
public:
	enum GATTO_STATE { GATTO_NORMAL, GATTO_DAMAGED, GATTO_END };

private:
	CGattoIcon(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGattoIcon(const CGattoIcon& Prototype);
	virtual ~CGattoIcon() = default;
	
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_GattoDamaged()
	{
		m_eGattoState = GATTO_DAMAGED;
		m_dwTime = GetTickCount();
	}

private:
	GATTO_STATE m_eGattoState = GATTO_NORMAL;
	DWORD m_dwTime = 0;

public:
	static CGattoIcon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END