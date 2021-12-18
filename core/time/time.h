#ifndef OPENGL_STUDY_TIME_H
#define OPENGL_STUDY_TIME_H

class Time
{
public:
    static void Update();

    static inline float GetElapsedTime()
    {
        return m_ElapsedTime;
    }

    static inline float GetDeltaTime()
    {
        return m_DeltaTime;
    }

private:
    Time()             = delete;
    Time(const Time &) = delete;

    static float m_ElapsedTime;
    static float m_DeltaTime;
    static float m_PrevTime;
};

#endif //OPENGL_STUDY_TIME_H
