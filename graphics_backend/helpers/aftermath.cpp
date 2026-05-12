#ifdef RENDER_BACKEND_DX12

#include "aftermath.h"
#include "debug.h"
#include "helpers/dx12_helpers.h"
#include "file_system.h"

#include <filesystem>
#include <mutex>

#include "GFSDK_Aftermath.h"
#include "GFSDK_Aftermath_GpuCrashDumpDecoding.h"
#include "GFSDK_Aftermath_GpuCrashDump.h"

namespace Aftermath
{
	bool s_Enabled = false;
    std::mutex s_Mutex;
    std::string s_LastEventMarker;

    std::unordered_map<const void*, GFSDK_Aftermath_ResourceHandle> s_RegisteredResourceHandles;
    std::unordered_map<const void*, GFSDK_Aftermath_ContextHandle> s_ContextHandles;

    std::string GetTimestampedFilename(const std::string& base, const std::string& ext)
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::zoned_time{ std::chrono::current_zone(), now };

        return std::format("{}_{:%Y-%m-%d_%H-%M-%S}.{}", base, time, ext);
    }

    std::filesystem::path GetDumpsPath()
    {
        return FileSystem::GetSpecialFolderPath(SpecialFolder::DOCUMENTS) / "RenderEngine" / "AftermathDumps";
    }

    void GpuCrashDumpCallback(const void* pGpuCrashDump, const uint32_t gpuCrashDumpSize, void* pUserData)
    {
        std::lock_guard<std::mutex> lock(s_Mutex);

        const std::filesystem::path path = GetDumpsPath() / GetTimestampedFilename("Crash", "nv-gpudmp");
        FileSystem::WriteFileBytes(path, std::span(static_cast<const uint8_t*>(pGpuCrashDump), gpuCrashDumpSize));

        Debug::LogInfoFormat("[GraphicsBackend] Aftermath dump saved to: {}", path.string());
    }

    void ShaderDebugInfoCallback(const void* pShaderDebugInfo, const uint32_t shaderDebugInfoSize, void* pUserData)
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        
        GFSDK_Aftermath_ShaderDebugInfoIdentifier identifier = {};
        GFSDK_Aftermath_GetShaderDebugInfoIdentifier(GFSDK_Aftermath_Version_API, pShaderDebugInfo, shaderDebugInfoSize, &identifier);

        const std::string fileName = "shader-" + std::to_string(identifier.id[0]) + "-" + std::to_string(identifier.id[1]) + ".nvdbg";
        const std::filesystem::path path = GetDumpsPath() / "ShaderDebugInfo" / fileName;
        FileSystem::WriteFileBytes(path, std::span(static_cast<const uint8_t*>(pShaderDebugInfo), shaderDebugInfoSize));

        Debug::LogInfoFormat("[GraphicsBackend] Aftermath shader info saved to: {}", path.string());
    }

    void CrashDumpDescriptionCallback(PFN_GFSDK_Aftermath_AddGpuCrashDumpDescription addDescription, void* pUserData)
    {
        addDescription(GFSDK_Aftermath_GpuCrashDumpDescriptionKey_ApplicationName, "Render Engine");
    }

    void ResolveMarkerCallback(const void* pMarkerData, const uint32_t markerDataSize, void* pUserData, PFN_GFSDK_Aftermath_ResolveMarker resolveMarker)
    {
    }

	void SetEnabled(bool enabled)
	{
        s_Enabled = enabled;
        if (!s_Enabled)
	        return;

        GFSDK_Aftermath_EnableGpuCrashDumps(
	        GFSDK_Aftermath_Version_API,
	        GFSDK_Aftermath_GpuCrashDumpWatchedApiFlags_DX,
	        GFSDK_Aftermath_GpuCrashDumpFeatureFlags_Default | GFSDK_Aftermath_GpuCrashDumpFeatureFlags_DeferDebugInfoCallbacks,
	        GpuCrashDumpCallback,
	        ShaderDebugInfoCallback,
	        CrashDumpDescriptionCallback,
	        ResolveMarkerCallback,
	        nullptr);
	}

	void Initialize(void* devicePtr)
	{
        if (!s_Enabled)
            return;

        constexpr uint32_t aftermathFlags =
            GFSDK_Aftermath_FeatureFlags_EnableMarkers |
            GFSDK_Aftermath_FeatureFlags_CallStackCapturing |
            GFSDK_Aftermath_FeatureFlags_EnableResourceTracking |
            GFSDK_Aftermath_FeatureFlags_GenerateShaderDebugInfo |
            GFSDK_Aftermath_FeatureFlags_EnableShaderErrorReporting;

        GFSDK_Aftermath_DX12_Initialize(
            GFSDK_Aftermath_Version_API,
            aftermathFlags,
            static_cast<ID3D12Device*>(devicePtr));
	}

	void RegisterResource(void* resourcePtr)
	{
        if (!s_Enabled)
	        return;

        GFSDK_Aftermath_ResourceHandle handle;
        GFSDK_Aftermath_DX12_RegisterResource(static_cast<ID3D12Resource*>(resourcePtr), &handle);

        s_RegisteredResourceHandles[resourcePtr] = handle;
	}

	void UnregisterResource(const void* resourcePtr)
	{
        if (!s_Enabled)
            return;

        const auto it = s_RegisteredResourceHandles.find(resourcePtr);
        if (it == s_RegisteredResourceHandles.end())
            return;

        GFSDK_Aftermath_DX12_UnregisterResource(it->second);
        s_RegisteredResourceHandles.erase(it);
	}

	void CreateContextHandle(void* commandListPtr)
	{
        if (!s_Enabled)
            return;

        GFSDK_Aftermath_ContextHandle handle;
        GFSDK_Aftermath_DX12_CreateContextHandle(static_cast<ID3D12CommandList*>(commandListPtr), &handle);

        s_ContextHandles[commandListPtr] = handle;
	}

    void SetEventMarker_Internal(const void* commandListPtr, const std::string& name)
    {
        if (!s_Enabled)
            return;

        const auto it = s_ContextHandles.find(commandListPtr);
        if (it != s_ContextHandles.end())
	        GFSDK_Aftermath_SetEventMarker(it->second, name.c_str(), name.size() + 1);
    }

	void SetEventMarker(const void* commandListPtr, const std::string& name)
	{
        SetEventMarker_Internal(commandListPtr, name);
        s_LastEventMarker = name + " --- End";
	}

	void SetLastEventMarker(const void* commandListPtr)
	{
        SetEventMarker_Internal(commandListPtr, s_LastEventMarker);
	}

	void CreateDump()
	{
        if (!s_Enabled)
            return;

        GFSDK_Aftermath_CrashDump_Status status = GFSDK_Aftermath_CrashDump_Status_Unknown;
        GFSDK_Aftermath_GetCrashDumpStatus(&status);

        const auto tStart = std::chrono::steady_clock::now();
        auto tElapsed = std::chrono::milliseconds::zero();

        while (status != GFSDK_Aftermath_CrashDump_Status_CollectingDataFailed && status != GFSDK_Aftermath_CrashDump_Status_Finished && tElapsed.count() < std::chrono::milliseconds(10000).count())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            GFSDK_Aftermath_GetCrashDumpStatus(&status);

            tElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - tStart);
        }

        if (status == GFSDK_Aftermath_CrashDump_Status_Finished)
            Debug::LogInfo("[GraphicsBackend] Aftermath finished processing the crash dump");
        else
            Debug::LogInfoFormat("[GraphicsBackend] Unexpected crash dump status after timeout: {}", std::to_string(status));
	}
}

#endif