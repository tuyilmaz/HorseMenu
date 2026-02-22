#include "Entity.hpp"
#include "core/scripting/LatentFunction.hpp"
#include "core/scripting/LuaLibrary.hpp"
#include "core/scripting/LuaScript.hpp"
#include "core/scripting/LuaUtils.hpp"
#include "game/rdr/Ped.hpp"

namespace YimMenu::Lua
{
	class Ped : LuaLibrary
	{
	public:
		using LuaLibrary::LuaLibrary;

		static int New(lua_State* state)
		{
			CreateObject<YimMenu::Ped>(state, luaL_checkinteger(state, 1));
			return 1;
		}

		static int Create(lua_State* state)
		{
			CopyObject<YimMenu::Ped>(state, YimMenu::Ped::Create(GetHashArgument(state, 1), GetObject<rage::fvector3>(state, 2), lua_gettop(state) >= 3 ? luaL_checknumber(state, 3) : 0.0f));
			return 1;
		}

		static int GetVehicle(lua_State* state)
		{
			CopyObject<YimMenu::Vehicle>(state, GetObject<YimMenu::Ped>(state, 1).GetVehicle());
			return 1;
		}

		static int GetMount(lua_State* state)
		{
			CopyObject<YimMenu::Ped>(state, GetObject<YimMenu::Ped>(state, 1).GetMount());
			return 1;
		}

		static int GetLastMount(lua_State* state)
		{
			CopyObject<YimMenu::Ped>(state, GetObject<YimMenu::Ped>(state, 1).GetLastMount());
			return 1;
		}

		static int GetRagdoll(lua_State* state)
		{
			lua_pushboolean(state, GetObject<YimMenu::Ped>(state, 1).GetRagdoll());
			return 1;
		}

		static int SetRagdoll(lua_State* state)
		{
			GetObject<YimMenu::Ped>(state, 1).SetRagdoll(CheckBooleanSafe(state, 2));
			return 0;
		}

		static int GetBonePosition(lua_State* state)
		{
			MoveObject<rage::fvector3>(state, GetObject<YimMenu::Ped>(state, 1).GetBonePosition(luaL_checkinteger(state, 2)));
			return 1;
		}

		// GetConfigFlag
		// SetConfigFlag
		// SetCombatAttribute

		static int IsEnemy(lua_State* state)
		{
			lua_pushboolean(state, GetObject<YimMenu::Ped>(state, 1).IsEnemy());
			return 1;
		}

		static int GetAccuracy(lua_State* state)
		{
			lua_pushinteger(state, GetObject<YimMenu::Ped>(state, 1).GetAccuracy());
			return 1;
		}

		static int SetAccuracy(lua_State* state)
		{
			GetObject<YimMenu::Ped>(state, 1).SetAccuracy(luaL_checkinteger(state, 2));
			return 0;
		}

		static int GetStamina(lua_State* state)
		{
			lua_pushnumber(state, GetObject<YimMenu::Ped>(state, 1).GetStamina());
			return 1;
		}

		static int SetStamina(lua_State* state)
		{
			GetObject<YimMenu::Ped>(state, 1).SetStamina(luaL_checknumber(state, 2));
			return 0;
		}

		static int GetMaxStamina(lua_State* state)
		{
			lua_pushnumber(state, GetObject<YimMenu::Ped>(state, 1).GetMaxStamina());
			return 1;
		}

		static int GetMotivation(lua_State* state)
		{
			lua_pushnumber(state, GetObject<YimMenu::Ped>(state, 1).GetMotivation(static_cast<MotivationState>(luaL_checkinteger(state, 2))));
			return 1;
		}

		static int SetMotivation(lua_State* state)
		{
			GetObject<YimMenu::Ped>(state, 1).SetMotivation(static_cast<MotivationState>(luaL_checkinteger(state, 2)), luaL_checknumber(state, 3));
			return 0;
		}

		static int SetInMount(lua_State* state)
		{
			auto seat = lua_gettop(state) >= 3 ? luaL_checkinteger(state, 3) : -1;
			GetObject<YimMenu::Ped>(state, 1).SetInMount(GetObject<YimMenu::Ped>(state, 2), seat);
			return 0;
		}

		static int GetConfigFlag(lua_State* state)
		{
			lua_pushboolean(state, GetObject<YimMenu::Ped>(state, 1).GetConfigFlag(static_cast<PedConfigFlag>(luaL_checkinteger(state, 2))));
			return 1;
		}

		static int SetConfigFlag(lua_State* state)
		{
			GetObject<YimMenu::Ped>(state, 1).SetConfigFlag(static_cast<PedConfigFlag>(luaL_checkinteger(state, 2)), CheckBooleanSafe(state, 3));
			return 0;
		}

		static int SetTargetActionDisableFlag(lua_State* state)
		{
			GetObject<YimMenu::Ped>(state, 1).SetTargetActionDisableFlag(luaL_checkinteger(state, 2), CheckBooleanSafe(state, 3));
			return 0;
		}

		static int SetScale(lua_State* state)
		{
			GetObject<YimMenu::Ped>(state, 1).SetScale(luaL_checknumber(state, 2));
			return 0;
		}

		static int SetQuality(lua_State* state)
		{
			GetObject<YimMenu::Ped>(state, 1).SetQuality(luaL_checkinteger(state, 2));
			return 0;
		}

		static int GetPlayer(lua_State* state)
		{
			lua_pushinteger(state, GetObject<YimMenu::Ped>(state, 1).GetPlayer());
			return 1;
		}

		static int SetVariation(lua_State* state)
		{
			GetObject<YimMenu::Ped>(state, 1).SetVariation(luaL_checkinteger(state, 2));
			return 0;
		}

		virtual void Register(lua_State* state) override
		{
			luaL_newmetatable(state, "Ped");
			{
				lua_newtable(state);
				{
					RegisterEntityMethods(state); // re-registering the entity methods under the vehicle table is more efficient than subclassing
					SetFunction(state, GetVehicle, "get_vehicle");
					SetFunction(state, GetMount, "get_mount");
					SetFunction(state, GetLastMount, "get_last_mount");
					SetFunction(state, GetRagdoll, "get_ragdoll");
					SetFunction(state, SetRagdoll, "set_ragdoll");
					SetFunction(state, GetBonePosition, "get_bone_position");
					SetFunction(state, IsEnemy, "is_enemy");
					SetFunction(state, GetAccuracy, "get_accuracy");
					SetFunction(state, SetAccuracy, "set_accuracy");
					SetFunction(state, GetStamina, "get_stamina");
					SetFunction(state, SetStamina, "set_stamina");
					SetFunction(state, GetMaxStamina, "get_max_stamina");
					SetFunction(state, GetMotivation, "get_motivation");
					SetFunction(state, SetMotivation, "set_motivation");
					SetFunction(state, SetInMount, "set_in_mount");
					SetFunction(state, GetConfigFlag, "get_config_flag");
					SetFunction(state, SetConfigFlag, "set_config_flag");
					SetFunction(state, SetTargetActionDisableFlag, "set_target_action_disable_flag");
					SetFunction(state, SetScale, "set_scale");
					SetFunction(state, SetQuality, "set_quality");
					SetFunction(state, GetPlayer, "get_player");
					SetFunction(state, SetVariation, "set_variation");
				}
				lua_setfield(state, -2, "__index"); // prototype
			}
			Metatable<YimMenu::Ped>::Register(state);
			Metatable<YimMenu::Entity>::AddSubclass<YimMenu::Ped>();

			lua_newtable(state);
			SetConstructor<New>(state);
			SetFunction(state, LatentFunction<Create>, "create");
			lua_setglobal(state, "Ped");
		}
	};

	Ped _Ped;
}
