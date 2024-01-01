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
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

#pragma once

#include <vector>
#include <string>

using std::vector;
using std::string;

namespace Graphics::GUI
{
	class GUIConsole
	{
	public:
		static inline bool renderConsole;
		static inline bool firstScrollToBottom;
		static inline bool allowScrollToBottom;

		static inline char inputTextBuffer[128];
		static inline vector<string> consoleMessages;

		static void RenderConsole();
		static void AddTextToConsole(const string& message);
	private:
		static inline const int maxConsoleMessages = 1000;
	};
}