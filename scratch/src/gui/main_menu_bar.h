#pragma once

namespace scratch {

    class MainMenuBar {
    public:
        // Draws Gizmo
        void render();
    private:
        bool demoWindowOpen;

        void reloadCurrentScene() const;

        void saveCurrentScene() const;
    };

}
