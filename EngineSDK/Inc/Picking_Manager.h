#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CPicking_Manager final : public CBase
{

public:
	enum PICKING_GROUP { PICK_BULLET, PICK_END };
private:
	CPicking_Manager();
	virtual ~CPicking_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_PickingGroup(PICKING_GROUP ePickingGroup, class CGameObject* pGameObject);


	HRESULT Reset_PickingGroup();

	void Picking_Check_Group(PICKING_GROUP eGroup);

private:
	vector<class CGameObject*> m_GameObjects[PICK_END];

public:
	static CPicking_Manager* Create();
	virtual void Free() override;
};
END