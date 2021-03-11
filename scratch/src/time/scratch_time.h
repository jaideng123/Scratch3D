#pragma once
namespace scratch {
    class Time {
    public:
        static void InitializeClock();
        static void UpdateClock();

        static float GetDeltaTime();

        static double GetElapsedTime();

    private:
        inline static double elapsedTime;
        inline static float deltaTime;
        inline static double lastFrameTime;
    };

}
