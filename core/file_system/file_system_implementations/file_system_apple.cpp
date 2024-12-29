#if RENDER_ENGINE_APPLE

#include "file_system_apple.h"
#include <Foundation/NSBundle.hpp>

FileSystemApple::FileSystemApple() : FileSystemBase(),
    m_ResourcesPath(NS::Bundle::mainBundle()->resourcePath()->cString(NS::UTF8StringEncoding))
{
}

#endif
