#if RENDER_ENGINE_APPLE

#define NS_PRIVATE_IMPLEMENTATION

#include "file_system_apple.h"
#include <Foundation/NSBundle.hpp>

FileSystemApple::FileSystemApple() : FileSystemBase()
{
    m_BuildResourcesPath = NS::Bundle::mainBundle()->resourcePath()->cString(NS::UTF8StringEncoding);
    m_EditorResourcesPath = m_BuildResourcesPath / "editor_resources";
}

#endif
