#pragma once
#include <rage/fwBasePool.hpp>
#include <script/types.hpp>
#include "game/rdr/natives.hpp"

namespace YimMenu::Helpers
{
	extern rage::fwBasePool* GetPedPool();
	extern rage::fwBasePool* GetObjectPool();
	extern rage::fwBasePool* GetVehiclePool();
	extern rage::fwBasePool* GetPickupPool();

	inline std::vector<Ped> GetAllPeds()
	{
		std::vector<Ped> result;

		if (const rage::fwBasePool* pool = GetPedPool())
		{
			for (uint32_t i = 0; i < pool->m_Size; i++)
			{
				if (pool->IsValid(i))
				{
					if (void* obj = pool->GetAt(i))
					{
						uint32_t ent = Pointers.FwScriptGuidCreateGuid(obj);
						if (ENTITY::DOES_ENTITY_EXIST(ent))
							result.push_back(ent);
					}
				}
			}
		}

		return result;
	}

	inline std::vector<Object> GetAllObjects()
	{
		std::vector<Object> result;

		if (const rage::fwBasePool* pool = GetObjectPool())
		{
			for (uint32_t i = 0; i < pool->m_Size; i++)
			{
				if (pool->IsValid(i))
				{
					if (void* obj = pool->GetAt(i))
					{
						uint32_t ent = Pointers.FwScriptGuidCreateGuid(obj);
						if (ENTITY::DOES_ENTITY_EXIST(ent))
							result.push_back(ent);
					}
				}
			}
		}

		return result;
	}

	inline std::vector<Vehicle> GetAllVehicles()
	{
		std::vector<Vehicle> result;

		if (const rage::fwBasePool* pool = GetVehiclePool())
		{
			for (uint32_t i = 0; i < pool->m_Size; i++)
			{
				if (pool->IsValid(i))
				{
					if (void* obj = pool->GetAt(i))
					{
						uint32_t ent = Pointers.FwScriptGuidCreateGuid(obj);
						if (ENTITY::DOES_ENTITY_EXIST(ent))
							result.push_back(ent);
					}
				}
			}
		}

		return result;
	}

	inline std::vector<Pickup> GetAllPickups()
	{
		std::vector<Pickup> result;

		if (const rage::fwBasePool* pool = GetPickupPool())
		{
			for (uint32_t i = 0; i < pool->m_Size; i++)
			{
				if (pool->IsValid(i))
				{
					if (void* obj = pool->GetAt(i))
					{
						uint32_t ent = Pointers.FwScriptGuidCreateGuid(obj);
						if (ENTITY::DOES_ENTITY_EXIST(ent))
							result.push_back(ent);
					}
				}
			}
		}

		return result;
	}
}