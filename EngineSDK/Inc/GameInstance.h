#pragma once



#include "Renderer.h"
#include "Component_Manager.h"
#include "KeyMgr.h"
#include "Collider_Manager.h"
#include "Picking_Manager.h"

/* 1. 엔진과 클라이언트의 소통을 위한 클래스읻. */
/* 2. 엔진에서 클라이언트에 보여주고 싶은 함수들을 모아놓는다. */
/* 3. 함수들 -> 클래스의 멤버함수. 객체가 필요하다! 그래서 기능응ㄹ 제공해주기위한 클래스 객체를 보관한다. */
/* 4. 객체들을 보관하기위해 Initialize_Engine() 함수 안에서 기능을 제공해주기위한 클래스들을 객체화한다. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(_uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ LPDIRECT3DDEVICE9* ppDevice);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw_Engine();
	HRESULT Clear(_uint iLevelIndex);

public:
#pragma region GRAPHIC_DEVICE
	void Render_Begin();
	void Render_End(HWND hWnd = nullptr);
	LPDIRECT3DDEVICE9 Get_Device();
	LPD3DXSPRITE Get_Sprite();
	LPD3DXFONT Get_Font();
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNextLevel);
	_uint Get_CurrentLevel();
	void Set_CurrentLevel(_uint iLevel);
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class CGameObject* Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex = 0);
	list<CGameObject*>* Get_ObjectList(_uint iLevelIndex, const _wstring& strLayerTag);
	void Clear_ObjectList(_uint iLevelIndex, const _wstring& strLayerTag);
#pragma endregion

#pragma region COMPONENT_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region TIMER_MANAGER
	HRESULT Add_Timer(const _wstring& strTimerTag);
	_float Compute_TimeDelta(const _wstring& strTimerTag);
	void Update_Timer(const _wstring& strTimerTag);
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);
	void Set_RenderUI(_bool bUI);
#pragma endregion

#pragma region COLLIDER_MANAGER
public: /* For. Collider Manager */
	HRESULT Add_CollisionGroup(CCollider_Manager::COLLISION_GROUP eCollisionGroup, class CGameObject* pGameObject);
	_bool Collision_Check_Group(CCollider_Manager::COLLISION_GROUP eGroup, class CGameObject* pGameObject, CCollider_Manager::COLLISION_TYPE eType, _float3* pOutDistance = nullptr);
	_bool Collision_Check_Group_Multi(CCollider_Manager::COLLISION_GROUP eGroup, vector<class CGameObject*>& vecDamagedObj, class CGameObject* pDamageCauser, CCollider_Manager::COLLISION_TYPE eType);
#pragma endregion

#pragma region PICKING
	void Transform_MouseRay_ToLocalSpace(const _float4x4& WorldMatrix);
	_bool isPicked_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut, _float* pDist);
	_bool isPicked_InWorldSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);
	_bool isPicked_InLocalSpaceForBlending(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);
	void Picking_Check_Group(CPicking_Manager::PICKING_GROUP eGroup);
	HRESULT Add_PickingGroup(CPicking_Manager::PICKING_GROUP ePickingGroup, CGameObject* pGameObject);
#pragma endregion

#pragma region KeyMgr
	//누르고 있을 때
	bool GetButton(KeyType key);
	//맨 처음 눌렀을 때
	bool GetButtonDown(KeyType key);
	//맨 처음 눌렀다가 땔 때
	bool GetButtonUp(KeyType key);
#pragma endregion

#pragma region SOUND_MANAGER
public:
	void		Update_Sound();
	HRESULT		PlayDefault(TCHAR* _TagSound, _int _Channel, _float _fVloume, _bool _bLoop);
	HRESULT		PlayBGM(const TCHAR* _TagSound, _float _fVolume, _bool _bLoop);
	HRESULT		Play3D(TCHAR* _TagSound, _int _Channel, _float _fVolume, _float3 _vPos, _bool _bLoop);
	HRESULT		Play_NonStack(TCHAR* _TagSound, _int _Channel, _float _fVolume, _bool _bLoop);
	void		MuteAll();
	void		StopSound(int _iChannel);
	void		StopSoundALL();
	void		Set_ChannelVolume(int _Channel, _float _fVolume);
	HRESULT		Set_SoundPos(_int _iChannel, _float3 _vPos);
	HRESULT		Add_Sound(const char* _fullpath, const TCHAR* _filename, const TCHAR* _path);
	HRESULT		Load_Sound(const char* _folderName);
	HRESULT		Set_Listener(_float3 _vPos); //ㅏ
#pragma endregion

#pragma region SPEED_MANAGER
	void Update_Speed(_float _fTimeDelta);
	void Change_Speed(_float _time);
	void Set_ChangedSpeed(_float _Speed);
	_float Get_CurSpeed();
	_float Get_Timer();
#pragma endregion


private:
	class CGraphic_Device* m_pGraphic_Device = { nullptr };
	class CLevel_Manager* m_pLevel_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CComponent_Manager* m_pComponent_Manager = { nullptr };
	class CTimer_Manager* m_pTimer_Manager = { nullptr };
	class CCollider_Manager* m_pCollider_Manager = { nullptr };
	class CRenderer* m_pRenderer = { nullptr };
	class KeyMgr* m_pKeyMgr = { nullptr };
	class CSoundManager* m_pSound_Manager = { nullptr };
	class CPicking* m_pPicking = { nullptr };
	class CPicking_Manager* m_pPicking_Manager = { nullptr };
	class CSpeedManager* m_pSpeedManager = { nullptr };

public:
	void Release_Engine();
	virtual void Free() override;
};

END