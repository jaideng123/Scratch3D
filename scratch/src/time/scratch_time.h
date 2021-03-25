#pragma once
namespace scratch {
    class Time {
    public:
        static void initializeClock();

        static void updateClock();

        static float getDeltaTime();

        static double getElapsedTime();

    private:
        inline static double _elapsedTime;
        inline static float _deltaTime;
        inline static double _lastFrameTime;
    };

}
