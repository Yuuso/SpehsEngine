#include "stdafx.h"
#include "SpehsEngine/ImGui/Utility/BackendWrapper.h"

#include "SpehsEngine/ImGui/imgui.h"
#include "SpehsEngine/ImGui/imgui_impl_opengl3.h"
#include "SpehsEngine/ImGui/imgui_impl_sdl.h"
#include "SpehsEngine/Input/EventSignaler.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/Rendering/GLContext.h"
#include "SDL/SDL_mouse.h"
#include "SDL/SDL_keyboard.h"
#include "SDL/SDL_keycode.h"
#include "SpehsEngine/ImGui/imgui.h"

#pragma optimize("", off) // nocommit
namespace se
{
	namespace imgui
	{
        BackendWrapper::BackendWrapper(rendering::Window& _window, input::EventSignaler& _eventSignaler, const int _inputPriority)
            : window(_window)
            , eventSignaler(_eventSignaler)
            , inputPriority(_inputPriority)
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

            // Setup Dear ImGui style
            ::ImGui::StyleColorsDark();
            //ImGui::StyleColorsClassic();

            // Setup Platform/Renderer backends
            SDL_Window* sdlWindow = window.getSDLWindow();
            ImGui_ImplSDL2_InitForOpenGL(sdlWindow, window.getGLContext()->getImpl());
            const char* glsl_version = "#version 130";
            ImGui_ImplOpenGL3_Init(glsl_version);

            setInputPriority(_inputPriority);

            eventSignaler.connectToKeyboardPressSignal(keyboardKeyPressedConnection0, [](const input::KeyboardPressEvent& event)
                {
                    ImGuiIO& io = ::ImGui::GetIO();
                    io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                    io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
                    io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
#ifdef _WIN32
                    io.KeySuper = false;
#else
                    io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
#endif
                    return false;
                }, std::numeric_limits<int>::max());

            eventSignaler.connectToKeyboardReleaseSignal(keyboardKeyReleasedConnection0, [](const input::KeyboardReleaseEvent& event)
                {
                    ImGuiIO& io = ImGui::GetIO();
                    const int key = event.scancode;
                    IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
                    io.KeysDown[key] = false;
                    io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                    io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
                    io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
#ifdef _WIN32
                    io.KeySuper = false;
#else
                    io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
#endif
                    return false;
                }, std::numeric_limits<int>::max());

            // After event signaler has updated, begin a new frame with the freshly received input state
            eventSignaler.connectToPostUpdateSignal(eventSignalerPostUpdateConnection, [this]()
                {
                    ImGui_ImplOpenGL3_NewFrame();
                    ImGui_ImplSDL2_NewFrame(window.getSDLWindow());
                    ImGui::NewFrame();
                });
        }

        BackendWrapper::~BackendWrapper()
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();
        }

        void BackendWrapper::render()
        {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        void BackendWrapper::setInputPriority(const int _inputPriority)
        {
            inputPriority = _inputPriority;
            eventSignaler.connectToMouseHoverSignal(mouseHoverConnection, [](const input::MouseHoverEvent& event)
                {
                    ImGuiIO& io = ImGui::GetIO();
                    return io.WantCaptureMouse;
                }, inputPriority);

            eventSignaler.connectToMouseWheelSignal(mouseWheelConnection, [](const input::MouseWheelEvent& event)
                {
                    ImGuiIO& io = ImGui::GetIO();
                    if (io.WantCaptureMouse)
                    {
                        if (event.delta.x > 0) io.MouseWheelH += 1;
                        if (event.delta.x < 0) io.MouseWheelH -= 1;
                        if (event.delta.y > 0) io.MouseWheel += 1;
                        if (event.delta.y < 0) io.MouseWheel -= 1;
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }, inputPriority);

            eventSignaler.connectToMouseButtonPressSignal(mouseButtonPressedConnection, [this](const input::MouseButtonPressEvent& event)
                {
                    ImGuiIO& io = ImGui::GetIO();
                    if (io.WantCaptureKeyboard)
                    {
                        if (int(event.button) == SDL_BUTTON_LEFT)
                        {
                            mousePressedStates[0] = true;
                            ImGui_ImplSDL2_UpdateMousePressedStates(mousePressedStates);
                            return true;
                        }
                        else if (int(event.button) == SDL_BUTTON_RIGHT)
                        {
                            mousePressedStates[1] = true;
                            ImGui_ImplSDL2_UpdateMousePressedStates(mousePressedStates);
                            return true;
                        }
                        else if (int(event.button) == SDL_BUTTON_MIDDLE)
                        {
                            mousePressedStates[2] = true;
                            ImGui_ImplSDL2_UpdateMousePressedStates(mousePressedStates);
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        return false;
                    }
                }, inputPriority);

            eventSignaler.connectToTextInputSignal(textInputConnection, [](const input::TextInputEvent& event)
                {
                    ImGuiIO& io = ImGui::GetIO();
                    if (io.WantCaptureKeyboard)
                    {
                        io.AddInputCharactersUTF8(event.buffer.c_str());
                        return false;
                    }
                    else
                    {
                        return false;
                    }
                }, inputPriority);

            eventSignaler.connectToKeyboardPressSignal(keyboardKeyPressedConnection, [](const input::KeyboardPressEvent& event)
                {
                    ImGuiIO& io = ImGui::GetIO();
                    if (io.WantCaptureKeyboard)
                    {
                        int key = event.scancode;
                        IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
                        io.KeysDown[key] = true;
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }, inputPriority);
		}
	}
}