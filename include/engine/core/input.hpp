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

#pragma once

//external
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //two matrix_transform.hpp files exist in glm
#include "glfw3.h"
#include "magic_enum.hpp"

//engine
#include "stringUtils.hpp"

#include <unordered_map>
#include <iostream>
#include <string>

using namespace std;
using namespace glm;
using namespace Utils;

namespace Core
{
    class Input
    {
    public:
        enum Action
        {
            CameraForwards,
            CameraBackwards,
            CameraLeft,
            CameraRight,
            CameraUp,
            CameraDown,
            CameraSprint
        };
        enum Key
        {
			W,
			A,
			S,
			D,
			Space,
			Left_control,
			Left_shift
		};

        struct InputSettings
        {
            float moveSpeedMultiplier = 1.0f;
            bool cameraEnabled = false;

            bool printFPSToConsole = false;
            bool printIMGUIToConsole = false;
            bool printInputToConsole = false;
        };
        static inline InputSettings inputSettings;

        static inline unordered_map<Action, Key> key;
        static inline unordered_map<Action, int> glfwKey;

        Input(GLFWwindow* window, float sensitivity = 0.05f);
        mat4 GetViewMatrix() const;

        static void InputSetup();
        static void ProcessInput(GLFWwindow* window);

        vec3 GetFront() const { return cameraFront; }
        vec3 GetRight() const { return normalize(cross(cameraFront, cameraUp)); }
        vec3 GetCameraRotation() const { return vec3(yaw, pitch, 0); }

        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    private:
        GLFWwindow* window;
        float yaw;
        float pitch;
        double lastX;
        double lastY;
        bool firstMouse;
        float sensitivity;

        vec3 cameraPos;
        vec3 cameraFront;
        vec3 cameraUp;

        void ProcessMouseMovement(double xpos, double ypos);
        static void ProcessKeyboardInput(GLFWwindow* window);
    };
}