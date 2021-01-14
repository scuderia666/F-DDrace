// made by fokkonaut and ChillerDragon

#include "dummybase.h"
#include <engine/shared/config.h>
#include <game/server/gamecontext.h>

CGameContext *CDummyBase::GameServer() const { return m_pCharacter->GameServer(); }
CGameWorld *CDummyBase::GameWorld() const { return m_pCharacter->GameWorld(); }
IServer *CDummyBase::Server() const { return GameServer()->Server(); }

vec2 CDummyBase::GetPos() { return m_pCharacter->Core()->m_Pos; }
vec2 CDummyBase::GetVel() { return m_pCharacter->Core()->m_Vel; }
int CDummyBase::HookState() { return m_pCharacter->Core()->m_HookState; }
int CDummyBase::Jumped() { return m_pCharacter->Core()->m_Jumped; }
int CDummyBase::Jumps() { return m_pCharacter->Core()->m_Jumps; }
bool CDummyBase::IsGrounded() { return m_pCharacter->IsGrounded(); }
void CDummyBase::SetWeapon(int Weapon) { m_pCharacter->SetWeapon(Weapon); }
void CDummyBase::Die() { m_pCharacter->Die(); }
CNetObj_PlayerInput *CDummyBase::Input() { return m_pCharacter->Input(); }
CNetObj_PlayerInput *CDummyBase::LatestInput() { return m_pCharacter->LatestInput(); }

CDummyBase::CDummyBase(CCharacter *pChr, int Mode)
{
	m_pCharacter = pChr;
	m_pPlayer = pChr->GetPlayer();
	m_Mode = Mode;
}

void CDummyBase::Tick()
{
	if (!m_pPlayer->m_IsDummy || m_pPlayer->m_TeeControllerID != -1)
		return;

	// Prepare input
	m_pCharacter->ResetInput();
	Input()->m_Hook = 0;

	// Then start controlling
	OnTick();
}

void CDummyBase::Fire(bool Stroke)
{
	if (Stroke)
	{
		LatestInput()->m_Fire++;
		Input()->m_Fire++;
	}
	else
	{
		LatestInput()->m_Fire = 0;
		Input()->m_Fire = 0;
	}
}

void CDummyBase::AvoidFreeze()
{
	#define FREEZE(x, y) GameServer()->Collision()->GetTileRaw(x, y) == TILE_FREEZE
	#define AIR(x, y) GameServer()->Collision()->GetTileRaw(x, y) == TILE_AIR
	#define SOLID(x, y) GameServer()->Collision()->IsSolid(x, y)

	int x = GetPos().x;
	int y = GetPos().y;
	//int xv = GetVel().x * 32;
	int yv = GetVel().y * 32;

	// sides
	if(FREEZE(x+32, y))
		Input()->m_Direction = -1;
	if(FREEZE(x-32, y))
		Input()->m_Direction = 1;

	// corners
	if(FREEZE(x+32, y-32) && !FREEZE(x-32, y))
		Input()->m_Direction = -1;
	if(FREEZE(x-32, y-32) && !FREEZE(x+32, y))
		Input()->m_Direction = 1;

	// small edges
	if(FREEZE(x-32, y) && FREEZE(x-32, y+32))
		Input()->m_Direction = 1;

	if(AIR(x+32, y) && AIR(x+32, y+32))
		Input()->m_Direction = -1;
		
	// big edges
	if(AIR(x-32, y) && AIR(x-64, y) && AIR(x-64, y+32) && FREEZE(x-64, y+32))
		Input()->m_Direction = 1;
	if(AIR(x+32, y) && AIR(x+64, y) && AIR(x+64, y+32) && FREEZE(x+64, y+32))
		Input()->m_Direction = -1;
		
	// while falling
	if (FREEZE(x, y+yv))
	{
		if(SOLID(x-yv, y+yv))
			Input()->m_Direction = 1;
		if(SOLID(x+yv, y+yv))
			Input()->m_Direction = -1;
	}

	#undef FREEZE
	#undef AIR
	#undef SOLID
}
