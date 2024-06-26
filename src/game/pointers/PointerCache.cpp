#include "PointerCache.hpp"

namespace YimMenu
{
	using json = nlohmann::json;

	PointerCache::PointerCache(uintptr_t version) :
	    // FileMgr seems to bug out, so this is the way it must be done for now
	    m_File(std::filesystem::path(std::getenv("appdata")) / "HorseMenu" / "pointers.json"),
	    m_Version(version)
	{
	}

	PointerCache::~PointerCache()
	{
		Unload();
	}

	void PointerCache::Load()
	{
		if (!std::filesystem::exists(m_File))
		{
			LOG(VERBOSE) << "Pointer Cache Doesn't Exist";
			std::ofstream file(m_File, std::ios::out | std::ios::trunc);
			file << "{}" << std::endl;
			file.close();

			if (!std::filesystem::exists(m_File))
			{
				LOG(VERBOSE) << "Pointer Cache unable to open!";
			}
			else
				LOG(VERBOSE) << "Pointer Cache Reset!";
		}

		std::ifstream fileStream(m_File);
		if (fileStream.is_open())
		{
			json jsonData;
			fileStream >> jsonData;
			fileStream.close();

			for (const auto& item : jsonData)
			{
				std::string name = item["name"];
				uintptr_t value  = item["value"];

				m_Data.push_back(std::make_pair(name, value));
			}
		}
	}

	void PointerCache::Save() const
	{
		json data;

		for (const auto& pair : m_Data)
		{
			json item;
			item["name"]  = pair.first;
			item["value"] = pair.second;
			data.push_back(item);
		}

		std::ofstream fileStream(m_File);
		if (fileStream.is_open())
		{
			fileStream << data << std::endl;
			fileStream.close();
		}
		else
			LOG(WARNING) << "Unable to save Pointer Cache!";
	}

	uintptr_t PointerCache::GetData(std::string name)
	{
		for (const auto& pair : m_Data)
		{
			if (pair.first == name)
				return pair.second;
		}

		return 0;
	}

	uintptr_t PointerCache::GetOrUpdate(std::string name, uintptr_t value)
	{
		for (auto& pair : m_Data)
		{
			if (pair.first == name)
			{
				if (pair.second != value)
				{
					pair.second = value;

					Save();

					return value;
				}
			}
		}

		// If it doesn't exist, add it
		m_Data.push_back(std::make_pair(name, value));

		Save();

		return value;
	}

	uintptr_t PointerCache::GetCacheVersion()
	{
		return m_Version;
	}

	uintptr_t PointerCache::GetCacheFileVersion()
	{
		// If it is not present, use the GetOrUpdate to create it
		uint32_t fileVersion = GetData("version");
		if (fileVersion == 0)
		{
			fileVersion = GetOrUpdate("version", m_Version);
		}

		return fileVersion;
	}

	bool PointerCache::IsCacheOutdated()
	{
		return GetCacheVersion() != GetCacheFileVersion();
	}

	void PointerCache::IncrementCacheVersion()
	{
		GetOrUpdate("version", m_Version);
	}

	void PointerCache::Unload()
	{
		Save();
		m_Data.clear();
	}
}