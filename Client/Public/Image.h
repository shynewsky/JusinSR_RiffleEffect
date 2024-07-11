#pragma once

#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CImage abstract : public CGameObject
{
public:
	typedef struct
	{
		_float fX;		// 이미지 중점 좌표
		_float fY;
		_float fSizeX;	// 이미지 크기
		_float fSizeY;

		LEVELID	eLevelIndex;			// 텍스쳐 컴포넌트 레벨인덱스
		const _tchar* tFrameTextureTag;	// 이미지 택스쳐 태그
		int iFrameCount = 1;			// 이미지 개수
		int iFrameSpeed = 1;			// 이미지 순회주기
	}IMAGE_DESC;

protected:
	CImage(LPDIRECT3DDEVICE9 pGraphic_Device);
	CImage(const CImage& Prototype);
	virtual ~CImage() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual int Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	void Set_Variables(void* pArg);
	void Set_OrthoLH();
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

protected:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

protected:
	/* 직교투영 */
	_float4x4				m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	_float					m_fFrame = {};
	IMAGE_DESC				m_tImageInfo = {};

public:
	//static CButton* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END