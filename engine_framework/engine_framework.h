#ifndef RENDER_ENGINE_RENDER_ENGINE_H
#define RENDER_ENGINE_RENDER_ENGINE_H

class EngineFramework
{
public:
    static void Initialize(void* fileSystemData, void* graphicsBackendInitData, char** argv, int argc);
    static void TickMainLoop(int width, int height);
    static bool ShouldCloseWindow();
    static void Shutdown();

    static void ProcessMouseClick(int mouseButton, bool pressed);
    static void ProcessMouseMove(float x, float y);
    static void ProcessKeyPress(char key, bool pressed);
    static void ProcessCharInput(wchar_t ch);
    static void ProcessSpecialKey(int keyId, bool pressed);

    static void ProcessTouchDown(unsigned long touchId, float x, float y);
    static void ProcessTouchMove(unsigned long touchId, float x, float y);
    static void ProcessTouchUp(unsigned long touchId);
};

#endif //RENDER_ENGINE_RENDER_ENGINE_H
