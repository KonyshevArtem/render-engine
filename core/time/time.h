#ifndef RENDER_ENGINE_TIME_H
#define RENDER_ENGINE_TIME_H

namespace Time
{
    void   Init();
    void   Update();
    double GetElapsedTime();
    double GetDeltaTime();
}; // namespace Time

#endif //RENDER_ENGINE_TIME_H
