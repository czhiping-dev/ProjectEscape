// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapeGameplayTags.h"

namespace EscapeGameplayTags
{
#pragma region Player Tags
	/* Input Tags */

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint, "InputTag.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump, "InputTag.Jump");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_EquipWeapon, "InputTag.EquipWeapon");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_UnequipWeapon, "InputTag.UnequipWeapon");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_FireWeapon, "InputTag.FireWeapon");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ReloadWeapon, "InputTag.ReloadWeapon");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_UseGrenade, "InputTag.UseGrenade");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SwitchCameraMode, "InputTag.SwitchCameraMode");

	/* Ability Tags */
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Equip_Weapon, "Player.Ability.Equip.Weapon");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Unequip_Weapon, "Player.Ability.Unequip.Weapon");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Reload_Weapon, "Player.Ability.Reload.Weapon");



	/* Weapon Tags */
	UE_DEFINE_GAMEPLAY_TAG(Player_Weapon_Rifle, "Player.Weapon.Rifle");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Equip_Weapon, "Player.Event.Equip.Weapon");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Unequip_Weapon, "Player.Event.Unequip.Weapon");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_Weapon_Ammunition_Use, "Player.SetByCaller.Weapon.Ammunition.Use");
	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_AttackType_Ranged, "Player.SetByCaller.AttackType.Ranged");
	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_Weapon_Reload, "Player.SetByCaller.Weapon.Reload");

#pragma endregion

#pragma region Enemy Tags

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Melee, "Enemy.Ability.Melee");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Ranged, "Enemy.Ability.Ranged");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Weapon_Left, "Enemy.Weapon.Left");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Weapon_Right, "Enemy.Weapon.Right");

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_Strafing, "Enemy.Status.Strafing");
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_UnderAttack, "Enemy.Status.UnderAttack");
	

#pragma endregion 

#pragma region Shared Tags

	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Death, "Shared.Ability.Death");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_HitReact, "Shared.Ability.HitReact");

	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_HitReact, "Shared.Event.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_MeleeHit, "Shared.Event.MeleeHit");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_GrenadeHit, "Shared.Event.GrenadeHit");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_RangedHit, "Shared.Event.RangedHit");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_Respawn, "Shared.Event.Respawn");

	UE_DEFINE_GAMEPLAY_TAG(Shared_SetByCaller_BaseDamage, "Shared.SetByCaller.BaseDamage");

	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_Dead, "Shared.Status.Dead");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Front, "Shared.Status.HitReact.Front");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Left, "Shared.Status.HitReact.Left");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Right, "Shared.Status.HitReact.Right");
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Back, "Shared.Status.HitReact.Back");


	

#pragma endregion
}