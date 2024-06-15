#pragma once
#include "PointerCache.hpp"
#include "core/filemgr/File.hpp"
#include "core/filemgr/FileMgr.hpp"
#include "game/rdr/GraphicsOptions.hpp"
#include "game/rdr/RenderingInfo.hpp"
#include <rage/pools.hpp>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <rage/atArray.hpp>
#include <script/scrNativeHandler.hpp>
#include <script/scrProgramTable.hpp>
#include <script/scrThread.hpp>
#include <vulkan/vulkan.h>
#include <windows.h>

class CNetGamePlayer;
class CVehicle;
class CPed;
class CNetworkPlayerMgr;
class gBaseScriptDirectory;

namespace rage
{
	class scrThread;
	class netEventMgr;
	class netSyncTree;
	class netObject;
	class rlGamerInfo;
}

namespace YimMenu
{
	namespace Functions
	{
		using GetRendererInfo  = RenderingInfo* (*)();
		using GetNativeHandler = rage::scrNativeHandler (*)(rage::scrNativeHash hash);
		using FixVectors       = void (*)(rage::scrNativeCallContext* call_ctx);
		using SendEventAck = void (*)(rage::netEventMgr* eventMgr, void* event, CNetGamePlayer* sourcePlayer, CNetGamePlayer* targetPlayer, int eventIndex, int handledBitset);
		using HandleToPtr               = void* (*)(int handle);
		using PtrToHandle               = int (*)(void* pointer);
		using GetLocalPed               = CPed* (*)();
		using GetSyncTreeForType        = rage::netSyncTree* (*)(void* netObjMgr, uint16_t type);
		using GetNetworkPlayerFromPid   = CNetGamePlayer* (*)(uint8_t player);
		using WorldToScreen             = bool (*)(float* world_coords, float* out_x, float* out_y);
		using GetNetObjectById          = rage::netObject* (*)(uint16_t id);
		using RequestControlOfNetObject = bool (*)(rage::netObject** netId, bool unk);
		using SendNetInfoToLobby        = bool (*)(rage::rlGamerInfo* player, int64_t a2, int64_t a3, DWORD* a4);
		using ScriptVM = rage::eThreadState (*)(uint64_t* stack, int64_t** scr_globals, __int64 unkByte, rage::scrProgram* program, rage::scrThreadContext* ctx);
		using GetScriptProgram = rage::scrProgram* (*)(gBaseScriptDirectory*, uint32_t);
	};

	struct PointerData
	{
		// RDR
		bool* IsSessionStarted;
		std::int64_t** ScriptGlobals;
		void* NativeRegistrationTable;
		Functions::GetNativeHandler GetNativeHandler;
		Functions::FixVectors FixVectors;
		rage::atArray<rage::scrThread*>* ScriptThreads;
		PVOID RunScriptThreads;
		rage::scrThread** CurrentScriptThread;
		Functions::GetLocalPed GetLocalPed;
		Functions::SendNetInfoToLobby SendNetInfoToLobby;
		rage::scrProgramTable* ScriptProgramTable;
		PVOID InitNativeTables;
		Functions::ScriptVM ScriptVM;
		PVOID ScriptVMByte;
		Functions::GetScriptProgram GetScriptProgram;
		gBaseScriptDirectory* scrProgramDirectory;

		PoolEncryption* PedPool;
		PoolEncryption* ObjectPool;
		PoolEncryption* VehiclePool;
		PoolEncryption* PickupPool;
		uint32_t (*FwScriptGuidCreateGuid)(void*);

		// Security
		PVOID SendMetric;
		bool* RageSecurityInitialized;
		PVOID* VmDetectionCallback;
		PVOID QueueDependency;
		PVOID UnkFunction;

		// Protections
		PVOID HandleNetGameEvent;
		Functions::SendEventAck SendEventAck;
		PVOID HandleCloneCreate;
		PVOID HandleCloneSync;
		PVOID CanApplyData;
		Functions::GetSyncTreeForType GetSyncTreeForType;
		PVOID ResetSyncNodes;
		PVOID HandleScriptedGameEvent;
		PVOID AddObjectToCreationQueue;

		// Player Stuff
		PVOID PlayerHasJoined;
		PVOID PlayerHasLeft;
		Functions::GetNetworkPlayerFromPid GetNetPlayerFromPid;

		// Voice
		PVOID EnumerateAudioDevices;
		PVOID DirectSoundCaptureCreate;

		// Native Handles
		Functions::HandleToPtr HandleToPtr;
		Functions::PtrToHandle PtrToHandle;
		Functions::WorldToScreen WorldToScreen;
		Functions::GetNetObjectById GetNetObjectById;
		Functions::RequestControlOfNetObject RequestControlOfNetObject;

		// Misc
		PVOID ThrowFatalError;

		// Vulkan
		PVOID QueuePresentKHR;      //Init in renderer
		PVOID CreateSwapchainKHR;   //Init in renderer
		PVOID AcquireNextImageKHR;  //Init in renderer
		PVOID AcquireNextImage2KHR; //Init in renderer

		VkDevice* VkDevicePtr;

		// DX12
		IDXGISwapChain1** SwapChain;
		ID3D12CommandQueue** CommandQueue;

		// Misc Renderer Related
		HWND Hwnd;

		Functions::GetRendererInfo GetRendererInfo;
		GraphicsOptions GameGraphicsOptions;

		PVOID WndProc;
		BOOL IsVulkan;

		uint32_t ScreenResX;
		uint32_t ScreenResY;

		PVOID NetworkRequest;

		CNetworkPlayerMgr* NetworkPlayerMgr;
		void* NetworkObjectMgr;

		PVOID WritePlayerHealthData;

		//Patches
		bool* ExplosionBypass;
	};

	struct Pointers : PointerData
	{
		bool Init();
		void Restore();

		PointerCache Cache{1};
	};

	inline YimMenu::Pointers Pointers;
}