#ifndef OPENGL_STUDY_TIME_H
#define OPENGL_STUDY_TIME_H

class Time
{
public:
    static void Update();

    inline static float TimePassed = 0;
    inline static float DeltaTime  = 0;

private:
    inline static float PrevTime = 0;
};

#endif //OPENGL_STUDY_TIME_H
