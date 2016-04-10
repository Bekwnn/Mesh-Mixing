#pragma once
#include <OpenGP/GL/GlfwWindow.h>
#include <OpenGP/GL/ArcballCamera.h>

class ArcballWindow : public OpenGP::GlfwWindow{
private:
    bool buttonPress = false;
public:
    ArcballWindow(const std::string& title, int width, int height) :
        GlfwWindow(title, width, height){ }
    ~ArcballWindow(){ }

    void mouse_press_callback(int button, int action, int mods) override {
        using namespace OpenGP;

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                double x, y;
                glfwGetCursorPos(_window, &x, &y);
                scene.camera.mouse_down_tumble(Vec2(x, y));
                buttonPress = true;
            }
            else
                buttonPress = false;
        }
    }

    void mouse_move_callback(double xPos, double yPos) override {
        if (buttonPress)
            scene.camera.mouse_drag_tumble(OpenGP::Vec2(xPos, yPos));
    }

    void scroll_callback(double xOffset, double yOffset) override {
        scene.camera.mouse_scroll((float)yOffset);
    }
};

