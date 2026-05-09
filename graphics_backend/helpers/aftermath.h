#ifndef GRAPHICS_BACKEND_AFTERMATH_H
#define GRAPHICS_BACKEND_AFTERMATH_H

#include <string>

namespace Aftermath
{
	void SetEnabled(bool enabled);
	void Initialize(void* devicePtr);

	void RegisterResource(void* resourcePtr);
	void UnregisterResource(const void* resourcePtr);

	void CreateContextHandle(void* commandListPtr);
	void SetEventMarker(const void* commandListPtr, const std::string& name);
	void SetLastEventMarker(const void* commandListPtr);

	void CreateDump();
}

#endif
