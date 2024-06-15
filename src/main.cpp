#include "common.hpp"
#include "core/byte_patch_manager/byte_patch_manager.hpp"
#include "core/commands/HotkeySystem.hpp"
#include "core/filemgr/FileMgr.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/hooking/Hooking.hpp"
#include "core/memory/ModuleMgr.hpp"
#include "core/renderer/Renderer.hpp"
#include "core/settings/Settings.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/ScriptMgr.hpp"
#include "game/bigfeatures/CustomTeleport.hpp"
#include "game/features/Features.hpp"
#include "game/frontend/GUI.hpp"
#include "game/hooks/NativeHooks/NativeHook.hpp"
#include "game/pointers/Pointers.hpp"
#include "game/rdr/RuleService.hpp"


namespace YimMenu
{
	DWORD unload()
	{
		Pointers.Cache.Unload();
		Hooking::Destroy();
		Renderer::Destroy();
		Pointers.Restore();

		LogHelper::Destroy();

		CloseHandle(g_MainThread);
		FreeLibraryAndExitThread(g_DllInstance, EXIT_SUCCESS);
	}

	DWORD Main(void*)
	{
		const auto documents = std::filesystem::path(std::getenv("appdata")) / "HorseMenu";
		FileMgr::Init(documents); // TODO

		LogHelper::Init("HorseMenu", FileMgr::GetProjectFile("./cout.log"));

		g_HotkeySystem.RegisterCommands();
		CustomTeleport::FetchSavedLocations();
		Settings::Initialize(FileMgr::GetProjectFile("./settings.json"));

		if (!ModuleMgr.LoadModules())
			return unload();
		if (!Pointers.Init())
			return unload();
		if (!Renderer::Init())
			return unload();

		Byte_Patch_Manager::Init();

		auto awards_service = std::make_unique<AwardService>();
		LOG(INFO) << "Registered service instances.";

		Hooking::Init();

		ScriptMgr::Init();
		LOG(INFO) << "ScriptMgr Initialized";

		FiberPool::Init(5);
		LOG(INFO) << "FiberPool Initialized";

		auto native_hooks_instance = std::make_unique<native_hooks>();
		LOG(INFO) << "Dynamic native hooker initialized.";

		GUI::Init();

		ScriptMgr::AddScript(std::make_unique<Script>(&FeatureLoop));
		ScriptMgr::AddScript(std::make_unique<Script>(&BlockControlsForUI));
		ScriptMgr::AddScript(std::make_unique<Script>(&ContextMenuTick));

		Notifications::Show("HorseMenu", "Loaded succesfully", NotificationType::Success);

		while (g_Running)
		{
			// Needed incase UI is malfunctioning or for emergencies
			if (GetAsyncKeyState(VK_DELETE) & 0x8000 && !*Pointers.IsSessionStarted)
			{
				g_Running = false;
			}

			std::this_thread::sleep_for(3000ms);
			Settings::Save(); // TODO: move this somewhere else
		}

		LOG(INFO) << "Unloading";

		ScriptMgr::Destroy();
		LOG(INFO) << "ScriptMgr Uninitialized";

		FiberPool::Destroy();
		LOG(INFO) << "FiberPool Uninitialized";

		return unload();
	}
}

BOOL WINAPI DllMain(HINSTANCE dllInstance, DWORD reason, void*)
{
	using namespace YimMenu;

	DisableThreadLibraryCalls(dllInstance);

	if (reason == DLL_PROCESS_ATTACH)
	{
		g_DllInstance = dllInstance;

		g_MainThread = CreateThread(nullptr, 0, Main, nullptr, 0, &g_MainThreadId);
	}
	return true;
}