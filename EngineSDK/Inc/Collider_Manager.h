#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CCollider_Manager final : public CBase
{

public:
	enum COLLISION_GROUP { COLLISION_PLAYER, COLLISION_MONSTER, COLLISION_BULLET, COLLISION_PROJECTILE, COLLISION_WALL, COLLISION_GROUPEND };
	enum COLLISION_TYPE {COLLSIION_BOX, COLLSION_END };
private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_CollisionGroup(COLLISION_GROUP eCollisionGroup, class CGameObject* pGameObject);


	HRESULT Reset_ColliderGroup();

	_bool Collision_Check_Group(COLLISION_GROUP eGroup, class CGameObject* pGameObject, COLLISION_TYPE eCollisionType,  _float3* pOutDistance = nullptr);
	_bool Collision_Check_Group_Multi(COLLISION_GROUP eGroup, vector<class CGameObject*>& vecDamagedObj, class CGameObject* pDamageCauser, COLLISION_TYPE eCollisionType);

private:
	list<class CGameObject*> m_GameObjects[COLLISION_GROUPEND];
	typedef list<class CGameObject*> GAMEOBJECTS;

public:
	static CCollider_Manager* Create();
	virtual void Free() override;
};
END