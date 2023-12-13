//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/greeenlaser/Elypso-engine >.

//external
#include "glad.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//engine
#include "console.hpp"
#include "gui.hpp"
#include "input.hpp"
#include "render.hpp"
#include "timeManager.hpp"

#include <iostream>
#include <string>

using namespace std;
using namespace glm;
using namespace Graphics;
using namespace Core;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core
{
    void Input::InputSetup()
    {
        key[Action::CameraForwards] = Key::W; glfwKey[Action::CameraForwards] = GLFW_KEY_W;
        key[Action::CameraBackwards] = Key::S; glfwKey[Action::CameraBackwards] = GLFW_KEY_S;
        key[Action::CameraLeft] = Key::A; glfwKey[Action::CameraLeft] = GLFW_KEY_A;
        key[Action::CameraRight] = Key::D; glfwKey[Action::CameraRight] = GLFW_KEY_D;
        key[Action::CameraUp] = Key::Space; glfwKey[Action::CameraUp] = GLFW_KEY_SPACE;
        key[Action::CameraDown] = Key::Left_control; glfwKey[Action::CameraDown] = GLFW_KEY_LEFT_CONTROL;
        key[Action::CameraSprint] = Key::Left_shift; glfwKey[Action::CameraSprint] = GLFW_KEY_LEFT_SHIFT;
        key[Action::ToggleFullscreen] = Key::Z; glfwKey[Action::ToggleFullscreen] = GLFW_KEY_Z;
        key[Action::ToggleVSYNC] = Key::X; glfwKey[Action::ToggleVSYNC] = GLFW_KEY_X;
        key[Action::PrintFPSDebugToConsole] = Key::F1; glfwKey[Action::PrintFPSDebugToConsole] = GLFW_KEY_F1;
        key[Action::PrintIMGUIDebugToConsole] = Key::F2; glfwKey[Action::PrintIMGUIDebugToConsole] = GLFW_KEY_F2;
        key[Action::PrintInputDebugToConsole] = Key::F3; glfwKey[Action::PrintInputDebugToConsole] = GLFW_KEY_F3;
    }

    Input::Input(GLFWwindow* window, float sensitivity) : 
        window(window), 
        yaw(-90.0f), 
        pitch(0.0f), 
        lastX(0.0), lastY(0.0), 
        firstMouse(true), 
        sensitivity(sensitivity),
        cameraPos(vec3(0.0f, 0.0f, 3.0f)), 
        cameraFront(vec3(0.0f, 0.0f, -1.0f)), 
        cameraUp(vec3(0.0f, 1.0f, 0.0f)) {}

	void Input::ProcessInput(GLFWwindow* window)
	{
        Input::ProcessKeyboardInput(Render::window);

        ImGuiIO& io = ImGui::GetIO();

        if (inputSettings.cameraEnabled)
        {
            //process mouse movement
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
            {
                Render::camera.ProcessMouseMovement(xpos, ypos);
            });
            Render::camera.ProcessMouseMovement(mouseX, mouseY);
            Render::cameraSpeed = static_cast<float>(2.5f * inputSettings.mouseSpeedMultiplier * TimeManager::deltaTime);

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        if (inputSettings.printInputToConsole)
        {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                ostringstream messageStream;
                messageStream << 
                    "Left mouse button pressed at (" << 
                    fixed << 
                    setprecision(2) << 
                    mouseX << ", " << 
                    mouseY << ")\n";

                ConsoleManager::WriteConsoleMessage(
                    Caller::INPUT,
                    Type::DEBUG,
                    messageStream.str());
            }
        }
	}

    void Input::ProcessKeyboardInput(GLFWwindow* window)
    {
        //toggle fullscreen
        int fullscreenKeyState = glfwGetKey(window, static_cast<int>(glfwKey[Action::ToggleFullscreen]));
        if (fullscreenKeyState == GLFW_PRESS 
            && !debugSettings.wasFullscreenKeyPressed)
        {
            Render::ToggleFullscreenMode(Render::window, Render::enableFullscreen);

            ConsoleManager::WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "User set fullscreen state to " + to_string(Render::enableFullscreen) + ".\n");

            debugSettings.wasFullscreenKeyPressed = true;
        }
        else if (fullscreenKeyState == GLFW_RELEASE)
        {
            debugSettings.wasFullscreenKeyPressed = false;
        }
        //toggle vsync
        int vsyncKeyState = glfwGetKey(window, static_cast<int>(glfwKey[Action::ToggleVSYNC]));
        if (vsyncKeyState == GLFW_PRESS 
            && !debugSettings.wasVSYNCKeyPressed)
        {
            Render::useMonitorRefreshRate = !Render::useMonitorRefreshRate;
            glfwSwapInterval(Render::useMonitorRefreshRate ? 1 : 0);

            ConsoleManager::WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "User set vsync state to " + to_string(Render::useMonitorRefreshRate) + 
                ". Monitor refresh rate: " + to_string(GUI::GetScreenRefreshRate()) + ".\n");

            debugSettings.wasVSYNCKeyPressed = true;
        }
        else if (vsyncKeyState == GLFW_RELEASE)
        {
            debugSettings.wasVSYNCKeyPressed = false;
        }

        //toggle console framerate debug messages
        int fpsKeyState = glfwGetKey(window, static_cast<int>(glfwKey[Action::PrintFPSDebugToConsole]));
        if (fpsKeyState == GLFW_PRESS
            && !debugSettings.wasFPSDebugKeyPressed)
        {
            inputSettings.printFPSToConsole = !inputSettings.printFPSToConsole;
            debugSettings.wasFPSDebugKeyPressed = true;
        }
        else if (fpsKeyState == GLFW_RELEASE)
        {
            debugSettings.wasFPSDebugKeyPressed = false;
        }
        //toggle console imgui debug messages
        int imguiKeyState = glfwGetKey(window, static_cast<int>(glfwKey[Action::PrintIMGUIDebugToConsole]));
        if (imguiKeyState == GLFW_PRESS
            && !debugSettings.wasIMGUIDebugKeyPressed)
        {
            inputSettings.printIMGUIToConsole = !inputSettings.printIMGUIToConsole;
            debugSettings.wasIMGUIDebugKeyPressed = true;
        }
        else if (imguiKeyState == GLFW_RELEASE)
        {
            debugSettings.wasIMGUIDebugKeyPressed = false;
        }
        //toggle console input debug messages
        int inputKeyState = glfwGetKey(window, static_cast<int>(glfwKey[Action::PrintInputDebugToConsole]));
        if (inputKeyState == GLFW_PRESS
            && !debugSettings.wasInputDebugKeyPressed)
        {
            inputSettings.printInputToConsole = !inputSettings.printInputToConsole;
            debugSettings.wasInputDebugKeyPressed = true;
        }
        else if (inputKeyState == GLFW_RELEASE)
        {
            debugSettings.wasInputDebugKeyPressed = false;
        }

        if (inputSettings.cameraEnabled)
        {
            bool isLeftShiftPressed = glfwGetKey(window, static_cast<int>(glfwKey[Action::CameraSprint])) == GLFW_PRESS;
            float currentSpeed = Render::cameraSpeed;
            if (isLeftShiftPressed) currentSpeed = 2.0f * inputSettings.moveSpeedMultiplier;
            else                    currentSpeed = 1.0f * inputSettings.moveSpeedMultiplier;

            vec3 front = Render::camera.GetFront();
            vec3 right = Render::camera.GetRight();

            //camera forwards
            if (glfwGetKey(window, static_cast<int>(glfwKey[Action::CameraForwards])) == GLFW_PRESS)
            {
                Render::cameraPos += Render::cameraSpeed * currentSpeed * front;
            }
            //camera backwards
            if (glfwGetKey(window, static_cast<int>(glfwKey[Action::CameraBackwards])) == GLFW_PRESS)
            {
                Render::cameraPos -= Render::cameraSpeed * currentSpeed * front;
            }
            //camera left
            if (glfwGetKey(window, static_cast<int>(glfwKey[Action::CameraLeft])) == GLFW_PRESS)
            {
                Render::cameraPos -= Render::cameraSpeed * currentSpeed * right;
            }
            //camera right
            if (glfwGetKey(window, static_cast<int>(glfwKey[Action::CameraRight])) == GLFW_PRESS)
            {
                Render::cameraPos += Render::cameraSpeed * currentSpeed * right;
            }
            //camera up
            if (glfwGetKey(window, static_cast<int>(glfwKey[Action::CameraUp])) == GLFW_PRESS)
            {
                Render::cameraPos += Render::cameraUp * Render::cameraSpeed * currentSpeed;
            }
            //camera down
            if (glfwGetKey(window, static_cast<int>(glfwKey[Action::CameraDown])) == GLFW_PRESS)
            {
                Render::cameraPos -= Render::cameraUp * Render::cameraSpeed * currentSpeed;
            }
        }
    }

    void Input::ProcessMouseMovement(double xpos, double ypos) 
    {
        if (inputSettings.cameraEnabled)
        {
            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            double xOffset = xpos - lastX;
            double yOffset = lastY - ypos;

            lastX = xpos;
            lastY = ypos;

            xOffset *= sensitivity;
            yOffset *= sensitivity;

            yaw += xOffset;
            pitch += yOffset;

            if (yaw >= 360.00f
                || yaw <= -360.00f)
            {
                yaw = 0.0f;
            }

            if (pitch > 90.0f) pitch = 90.0f;
            if (pitch < -90.0f) pitch = -90.0f;

            vec3 front{};
            front.x = cos(radians(yaw)) * cos(radians(pitch));
            front.y = sin(radians(pitch));
            front.z = sin(radians(yaw)) * cos(radians(pitch));
            cameraFront = normalize(front);
        }        
    }

    void Input::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) 
    {}

    void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
	{
        if (action == GLFW_PRESS
            && key == GLFW_KEY_ESCAPE)
        {
            inputSettings.cameraEnabled = !inputSettings.cameraEnabled;
        }
    }

    void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {}

    mat4 Input::GetViewMatrix() const
    {
        return lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }
}