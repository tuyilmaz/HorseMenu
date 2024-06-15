#include "Helpers.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu
{
	rage::fwBasePool* Helpers::GetPedPool()
	{
		if (Pointers.PedPool->m_IsSet)
		{
			uint64_t x = _rotl64(Pointers.PedPool->m_Second, 30);
			return reinterpret_cast<rage::fwBasePool*>(~_rotl64(_rotl64(x ^ Pointers.PedPool->m_First, ((x & 0xFF) & 31) + 3), 32));
		}

		return nullptr;
	}

	rage::fwBasePool* Helpers::GetObjectPool()
	{
		if (Pointers.ObjectPool->m_IsSet)
		{
			uint64_t x = _rotl64(Pointers.ObjectPool->m_Second, 30);
			return reinterpret_cast<rage::fwBasePool*>(~_rotl64(_rotl64(x ^ Pointers.ObjectPool->m_First, 32), ((x & 0xFF) & 31) + 2));
		}

		return nullptr;
	}

	rage::fwBasePool* Helpers::GetVehiclePool()
	{
		if (Pointers.VehiclePool->m_IsSet)
		{
			uint64_t x = _rotl64(Pointers.VehiclePool->m_Second, 31);
			return reinterpret_cast<rage::fwBasePool*>(~_rotl64(_rotl64(x ^ Pointers.VehiclePool->m_First, 32), ((x & 0xFF) & 31) + 4));
		}

		return nullptr;
	}

	rage::fwBasePool* Helpers::GetPickupPool()
	{
		if (Pointers.PickupPool->m_IsSet)
		{
			uint64_t x = _rotl64(Pointers.PickupPool->m_Second, 30);
			return reinterpret_cast<rage::fwBasePool*>(~_rotl64(_rotl64(Pointers.PickupPool->m_First ^ x, 32), ((x & 0xFF) & 31) + 2));
		}

		return nullptr;
	}
}