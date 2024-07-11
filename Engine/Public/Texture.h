#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
public:
	enum TYPE { TYPE_TEXTURE2D/* : LPDIRECT3DTEXTURE9*/, TYPE_TEXTURECUBE/* : LPDIRECT3DCUBETEXTURE9*/, TYPE_END };

	typedef struct TextureInfo
	{
		int iStartTex = 0;
		int iCurrentTex = 0;
		int iEndTex = 0;
		_float fSpeed = 1;
	}TEXTUREDESC;

private:
	CTexture(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTexture(const CTexture& Prototype);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eTextureType, const _wstring& strTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT Bind_Texture(_uint iIndex = 0);
	LPDIRECT3DBASETEXTURE9 Get_Texture(_uint iIndex) {return m_Textures[iIndex];}

public:
	bool Move_Frame(const _float fTimer, _bool bLoop = true);
	void Set_Frame(int iStartTex, int iEndTex, _float fSpeed);
	void Turn_Frame(int iAniNum, int iEndTex);
	void Reset_Frame();
	void Set_ZeroFrame() { m_TextureDesc.iCurrentTex = 0; }
	TEXTUREDESC& Get_Frame() { return m_TextureDesc; }
	int Get_EndIndex() { return m_Textures.size(); }
	void Set_FrameSpeed(_float _fSpeed);

private:
	_uint									m_iNumTextures = { 0 };
	vector<IDirect3DBaseTexture9*>			m_Textures;
	TEXTUREDESC								m_TextureDesc;

	_float									m_fSumTime = 0.0f;

public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphic_Device, TYPE eTextureType, const _wstring& strTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END