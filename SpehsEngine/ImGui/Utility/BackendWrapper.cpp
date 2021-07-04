#include "stdafx.h"
#include "SpehsEngine/ImGui/Utility/BackendWrapper.h"

#include "bgfx/bgfx.h"
#include "bgfx/embedded_shader.h"
#include "SpehsEngine/ImGui/imgui.h"
#include "SpehsEngine/ImGui/imgui_impl_sdl.h"
#include "SpehsEngine/Input/EventSignaler.h"
#include "SpehsEngine/Graphics/Renderer.h"
#include "SDL/SDL_mouse.h"
#include "SDL/SDL_keyboard.h"
#include "SDL/SDL_keycode.h"

#include "SpehsEngine/Graphics/EmbeddedShaders/vs_imgui.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_imgui.h"
#include "SpehsEngine/Graphics/EmbeddedFonts/AnonymousPro-Regular.ttf.h"
#include "SpehsEngine/Graphics/EmbeddedFonts/OpenSans-Regular.ttf.h"


namespace se
{
	namespace imgui
	{
        // Graphics backend global state
        namespace impl
        {
            const bgfx::ViewId  viewId = 234;
            bgfx::VertexLayout  layout;
            bgfx::ProgramHandle imguiProgram;
            bgfx::TextureHandle fontTexture;
            bgfx::UniformHandle texUniform;
            ImFont*             font[ImGuiFont::Count];

            static const bgfx::EmbeddedShader embeddedShaders[] =
            {
                BGFX_EMBEDDED_SHADER(vs_imgui),
                BGFX_EMBEDDED_SHADER(fs_imgui),

                BGFX_EMBEDDED_SHADER_END()
            };
        }

        ImFont* getFont(const ImGuiFont _font)
        {
            return impl::font[_font];
        }


        BackendWrapper::BackendWrapper(input::EventSignaler& _eventSignaler, const int _inputPriority, graphics::Renderer& _renderer)
            : eventSignaler(_eventSignaler)
            , inputPriority(_inputPriority)
            , renderer(_renderer)
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
            ImGui::StyleColorsDark();

            SDL_Window* sdlWindow = renderer.getDefaultSDLWindow(); // NOTE: ImGui will only work in the default window!
            ImGui_ImplSDL2_Init(sdlWindow);

            // Bgfx implementation
            {
                bgfx::RendererType::Enum type = bgfx::getRendererType();
                impl::imguiProgram = bgfx::createProgram(
                      bgfx::createEmbeddedShader(impl::embeddedShaders, type, "vs_imgui")
                    , bgfx::createEmbeddedShader(impl::embeddedShaders, type, "fs_imgui")
                    , true
                    );
                impl::texUniform = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);

                impl::layout
                    .begin()
                    .add(bgfx::Attrib::Position,    2, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::TexCoord0,   2, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::Color0,      4, bgfx::AttribType::Uint8, true)
                    .end();

                // Fonts
                uint8_t* data;
                int32_t width;
                int32_t height;
                {
                    ImFontConfig config;
                    config.FontDataOwnedByAtlas = false;
                    config.MergeMode = false;

                    const ImWchar* ranges = io.Fonts->GetGlyphRangesDefault();
                    constexpr float fontSizePixels = 18.0f;
                    impl::font[ImGuiFont::Regular] = io.Fonts->AddFontFromMemoryTTF((void*)font_openSansRegularTtf, sizeof(font_openSansRegularTtf), fontSizePixels, &config, ranges);
                    impl::font[ImGuiFont::Mono] = io.Fonts->AddFontFromMemoryTTF((void*)font_anonymousProRegularTtf, sizeof(font_anonymousProRegularTtf), fontSizePixels - 3.0f, &config, ranges);
                }

                io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

                impl::fontTexture = bgfx::createTexture2D(
                      (uint16_t)width
                    , (uint16_t)height
                    , false
                    , 1
                    , bgfx::TextureFormat::BGRA8
                    , 0
                    , bgfx::copy(data, width * height * 4)
                    );
            }

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
                    SDL_Window* sdlWindow = renderer.getDefaultSDLWindow();
                    ImGui_ImplSDL2_NewFrame(sdlWindow);
                    ImGui::NewFrame();
                });
        }

        BackendWrapper::~BackendWrapper()
        {
            ImGui_ImplSDL2_Shutdown();

            ImGui::DestroyContext();

            bgfx::destroy(impl::fontTexture);
            bgfx::destroy(impl::texUniform);
            bgfx::destroy(impl::imguiProgram);
        }

        void BackendWrapper::render()
        {
            preRenderSignal();
            ImGui::Render();

            // Bgfx implementation
            {
                bgfx::setViewFrameBuffer(impl::viewId, BGFX_INVALID_HANDLE); // Use default backbuffer, which should be the default window
                ImDrawData* drawData = ImGui::GetDrawData();

                const ImGuiIO& io = ImGui::GetIO();
                const float viewWidth = io.DisplaySize.x;
                const float viewHeight = io.DisplaySize.y;

                bgfx::setViewName(impl::viewId, "ImGui");
                bgfx::setViewMode(impl::viewId, bgfx::ViewMode::Sequential);

                const glm::mat4 projectionMatrix = glm::orthoRH_ZO(0.0f, viewWidth, viewHeight, 0.0f, 0.05f, 1000.0f);
                bgfx::setViewTransform(impl::viewId, NULL, &projectionMatrix);
                bgfx::setViewRect(impl::viewId, 0, 0, uint16_t(viewWidth), uint16_t(viewHeight));

                // Render command lists
                for (int32_t ii = 0, num = drawData->CmdListsCount; ii < num; ++ii)
                {
                    bgfx::TransientVertexBuffer tvb;
                    bgfx::TransientIndexBuffer tib;

                    const ImDrawList* drawList = drawData->CmdLists[ii];
                    const uint32_t numVertices = (uint32_t)drawList->VtxBuffer.size();
                    const uint32_t numIndices = (uint32_t)drawList->IdxBuffer.size();

                    if (bgfx::getAvailTransientVertexBuffer(numVertices, impl::layout) != numVertices ||
                        bgfx::getAvailTransientIndexBuffer(numIndices) != numIndices)
                    {
                        const char* msg = "Imgui: Out of transient buffer memory!";
                        log::warning(msg);
                        se_assert_m(false, msg);
                        break;
                    }

                    bgfx::allocTransientVertexBuffer(&tvb, numVertices, impl::layout);
                    bgfx::allocTransientIndexBuffer(&tib, numIndices, sizeof(ImDrawIdx) == 4);

                    ImDrawVert* verts = (ImDrawVert*)tvb.data;
                    bx::memCopy(verts, drawList->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert));

                    ImDrawIdx* indices = (ImDrawIdx*)tib.data;
                    bx::memCopy(indices, drawList->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx));

                    uint32_t offset = 0;
                    for (const ImDrawCmd* cmd = drawList->CmdBuffer.begin(), *cmdEnd = drawList->CmdBuffer.end(); cmd != cmdEnd; ++cmd)
                    {
                        if (cmd->UserCallback)
                        {
                            cmd->UserCallback(drawList, cmd);
                        }
                        else if (cmd->ElemCount != 0)
                        {
                            uint64_t state = 0
                                | BGFX_STATE_WRITE_RGB
                                | BGFX_STATE_WRITE_A
                                | BGFX_STATE_MSAA
                                | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
                                ;

                            bgfx::TextureHandle textureHandle = impl::fontTexture;

                            if (cmd->TextureId != nullptr)
                            {
                                se::imgui::ImGuiUserTextureData userTextureData;
                                userTextureData.id = cmd->TextureId;
                                textureHandle = { userTextureData.resourceHandle } ;
                            }

                            const uint16_t xx = uint16_t(bx::max(cmd->ClipRect.x, 0.0f));
                            const uint16_t yy = uint16_t(bx::max(cmd->ClipRect.y, 0.0f));
                            bgfx::setScissor(xx , yy
                                , uint16_t(bx::min(cmd->ClipRect.z, 65535.0f) - xx)
                                , uint16_t(bx::min(cmd->ClipRect.w, 65535.0f) - yy)
                                );

                            bgfx::setState(state);
                            bgfx::setTexture(0, impl::texUniform, textureHandle);
                            bgfx::setVertexBuffer(0, &tvb, 0, numVertices);
                            bgfx::setIndexBuffer(&tib, offset, cmd->ElemCount);
                            bgfx::submit(impl::viewId, impl::imguiProgram);
                        }

                        offset += cmd->ElemCount;
                    }
                }
            }
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
                    return io.WantCaptureMouse;
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