#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "SoundManager.h"
#include "Picking.h"
#include "SpeedManager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, const ENGINE_DESC& EngineDesc, LPDIRECT3DDEVICE9* ppDevice)
{
	/* 그래픽 카드를 초기화하낟. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc, ppDevice);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;



	_float4x4		ProjMatrix{};
	(*ppDevice)->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice);
	if (nullptr == m_pRenderer)
		return E_FAIL;


	/* 사운드 카드를 초기화하낟. */

	/* 입력장치를 초기화하낟. */
	m_pKeyMgr = KeyMgr::Create();
	if (nullptr == m_pKeyMgr)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking)
		return E_FAIL;

	m_pPicking_Manager = CPicking_Manager::Create();
	if (nullptr == m_pPicking_Manager)
		return E_FAIL;

	/* 여러가지 매니져를 초기화하낟. */

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pCollider_Manager = CCollider_Manager::Create();
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;

	m_pSound_Manager = CSoundManager::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;
	m_pSound_Manager->Initialize();

	m_pSpeedManager = CSpeedManager::Create();
	if (nullptr == m_pSpeedManager)
		return E_FAIL;

	return S_OK;
}



void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pObject_Manager->Priority_Update(fTimeDelta);

	m_pObject_Manager->Update(fTimeDelta);

	m_pPicking->Update();

	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);

	m_pKeyMgr->Update();

	m_pSound_Manager->Update();

	m_pCollider_Manager->Reset_ColliderGroup();
	m_pPicking_Manager->Reset_PickingGroup();
}

HRESULT CGameInstance::Draw_Engine()
{
	m_pRenderer->Draw();

	return m_pLevel_Manager->Render();
}

HRESULT CGameInstance::Clear(_uint iLevelIndex)
{
	m_pObject_Manager->Clear(iLevelIndex);

	return S_OK;
}

void CGameInstance::Render_Begin()
{
	m_pGraphic_Device->Render_Begin();
}

void CGameInstance::Render_End(HWND hWnd)
{
	m_pGraphic_Device->Render_End(hWnd);
}

LPDIRECT3DDEVICE9 CGameInstance::Get_Device()
{
	return m_pGraphic_Device->Get_Device();
}

LPD3DXSPRITE CGameInstance::Get_Sprite()
{
	return m_pGraphic_Device->Get_Sprite();
}

LPD3DXFONT CGameInstance::Get_Font()
{
	return m_pGraphic_Device->Get_Font();
}

HRESULT CGameInstance::Change_Level(_uint iLevelIndex, CLevel* pNextLevel)
{
	return m_pLevel_Manager->Change_Level(iLevelIndex, pNextLevel);
}

_uint CGameInstance::Get_CurrentLevel()
{
	return m_pLevel_Manager->Get_CurrentLevel();
}

void CGameInstance::Set_CurrentLevel(_uint iLevel)
{
	m_pLevel_Manager->Set_CurrentLevel(iLevel);
}

HRESULT CGameInstance::Add_Prototype(const _wstring& strPrototypeTag, CGameObject* pPrototype)
{
	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}


HRESULT CGameInstance::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Add_CloneObject_ToLayer(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CComponent* CGameInstance::Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

CGameObject* CGameInstance::Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Objects(iLevelIndex, strLayerTag, iIndex);
}

list<CGameObject*>* CGameInstance::Get_ObjectList(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Get_ObjectList(iLevelIndex, strLayerTag);
}

void CGameInstance::Clear_ObjectList(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Clear_ObjectList(iLevelIndex, strLayerTag);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CComponent* pPrototype)
{
	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

void CGameInstance::Update_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Update(strTimerTag);
}

HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, CGameObject* pRenderObject)
{
	return m_pRenderer->Add_RenderObject(eRenderGroupID, pRenderObject);
}

void CGameInstance::Set_RenderUI(_bool bUI)
{
	return m_pRenderer->Set_RenderUI(bUI);
}

HRESULT CGameInstance::Add_CollisionGroup(CCollider_Manager::COLLISION_GROUP eCollisionGroup, CGameObject* pGameObject)
{
	return m_pCollider_Manager->Add_CollisionGroup(eCollisionGroup, pGameObject);
}

_bool CGameInstance::Collision_Check_Group(CCollider_Manager::COLLISION_GROUP eGroup, CGameObject* pGameObject, CCollider_Manager::COLLISION_TYPE eType, _float3* pOutDistance)
{
	return m_pCollider_Manager->Collision_Check_Group(eGroup, pGameObject, eType, pOutDistance);
}

_bool CGameInstance::Collision_Check_Group_Multi(CCollider_Manager::COLLISION_GROUP eGroup, vector<class CGameObject*>& vecDamagedObj, CGameObject* pDamageCauser, CCollider_Manager::COLLISION_TYPE eType)
{
	return m_pCollider_Manager->Collision_Check_Group_Multi(eGroup, vecDamagedObj, pDamageCauser, eType);
}

void CGameInstance::Transform_MouseRay_ToLocalSpace(const _float4x4& WorldMatrix)
{
	return m_pPicking->Transform_ToLocalSpace(WorldMatrix);
}

_bool CGameInstance::isPicked_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut, _float* pDist)
{
	return m_pPicking->isPicked_InLocalSpace(vPointA, vPointB, vPointC, pOut, pDist);
}

_bool CGameInstance::isPicked_InWorldSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
{
	return m_pPicking->isPicked_InWorldSpace(vPointA, vPointB, vPointC, pOut);
}

_bool CGameInstance::isPicked_InLocalSpaceForBlending(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
{
	return m_pPicking->isPicked_InLocalSpaceForBlending(vPointA, vPointB, vPointC, pOut);
}

void CGameInstance::Picking_Check_Group(CPicking_Manager::PICKING_GROUP eGroup)
{
	m_pPicking_Manager->Picking_Check_Group(eGroup);
}

HRESULT CGameInstance::Add_PickingGroup(CPicking_Manager::PICKING_GROUP ePickingGroup, CGameObject* pGameObject)
{
	return m_pPicking_Manager->Add_PickingGroup(ePickingGroup, pGameObject);
}

bool CGameInstance::GetButton(KeyType key)
{
	return m_pKeyMgr->GetButton(key);
}

bool CGameInstance::GetButtonDown(KeyType key)
{
	return m_pKeyMgr->GetButtonDown(key);
}

bool CGameInstance::GetButtonUp(KeyType key)
{
	return m_pKeyMgr->GetButtonUp(key);
}


void CGameInstance::Update_Sound()
{
	m_pSound_Manager->Update();
}

HRESULT CGameInstance::PlayDefault(TCHAR* _TagSound, _int _Channel, _float _fVloume, _bool _bLoop)
{
	return m_pSound_Manager->PlayDefault(_TagSound, CSoundManager::SOUND_CHANNEL(_Channel), _fVloume, _bLoop);
}

HRESULT CGameInstance::PlayBGM(const TCHAR* _TagSound, _float _fVolume, _bool _bLoop)
{
	return m_pSound_Manager->PlayBGM(_TagSound, _fVolume, _bLoop);
}

HRESULT CGameInstance::Play3D(TCHAR* _TagSound, _int _Channel, _float _fVolume, _float3 _vPos, _bool _bLoop)
{
	return m_pSound_Manager->Play3D(_TagSound, _Channel, _fVolume, _vPos, _bLoop);
}

HRESULT CGameInstance::Play_NonStack(TCHAR* _TagSound, _int _Channel, _float _fVolume, _bool _bLoop)
{
	return m_pSound_Manager->Play_NonStack(_TagSound, _Channel, _fVolume, _bLoop);
}

void CGameInstance::MuteAll()
{
	m_pSound_Manager->MuteAll();
}

void CGameInstance::StopSound(int _iChannel)
{
	m_pSound_Manager->StopSound(CSoundManager::SOUND_CHANNEL(_iChannel));
}

void CGameInstance::StopSoundALL()
{
	m_pSound_Manager->StopSoundALL();
}

void CGameInstance::Set_ChannelVolume(int _Channel, _float _fVolume)
{
	m_pSound_Manager->SetChannelVolume(CSoundManager::SOUND_CHANNEL(_Channel), _fVolume);
}

HRESULT CGameInstance::Set_SoundPos(_int _iChannel, _float3 _vPos)
{
	return m_pSound_Manager->Set_SoundPos(_iChannel, _vPos);
}

HRESULT CGameInstance::Add_Sound(const char* _fullpath, const TCHAR* _filename, const TCHAR* _path)
{
	return m_pSound_Manager->Add_Sound(_fullpath, _filename, _path);
}

HRESULT CGameInstance::Load_Sound(const char* _folderName)
{
	return m_pSound_Manager->Load_Sound(_folderName);
}

HRESULT CGameInstance::Set_Listener(_float3 _vPos)
{
	return m_pSound_Manager->Set_Listener(_vPos);
}

void CGameInstance::Update_Speed(_float _fTimeDelta)
{
	m_pSpeedManager->Update(_fTimeDelta);
}

void CGameInstance::Change_Speed(_float _timeer)
{
	m_pSpeedManager->Change_Speed(_timeer);
}

void CGameInstance::Set_ChangedSpeed(_float _Speed)
{
	m_pSpeedManager->Set_ChangedSpeed(_Speed);
}

_float CGameInstance::Get_CurSpeed()
{
	return m_pSpeedManager->Get_CurSpeed();
}

_float CGameInstance::Get_Timer()
{
	return m_pSpeedManager->Get_Timer();
}

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pPicking);
	Safe_Release(m_pPicking_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pKeyMgr);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pSpeedManager);
	CGameInstance::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
	__super::Free();



}
