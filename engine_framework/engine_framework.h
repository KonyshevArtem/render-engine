#ifndef RENDER_ENGINE_RENDER_ENGINE_H
#define RENDER_ENGINE_RENDER_ENGINE_H

class EngineFramework
{
public:
    static void Initialize(void* fileSystemData, void* graphicsBackendInitData, const char* graphicsBackend);
    static void TickMainLoop(void* graphicsBackendFrameData, int width, int height);
    static bool ShouldCloseWindow();
    static void Shutdown();

    static void ProcessMouseMove(float x, float y);
    static void ProcessKeyPress(char key, bool pressed);
};

#endif //RENDER_ENGINE_RENDER_ENGINE_H
