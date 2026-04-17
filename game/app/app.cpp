#include "app.h"
#include "core/log/log.h"
#include "concurrency/utils/processor_info.h"
#include "platform/win32/event_loop/event_converter.h"
#include "platform/window/window.h"
LOGGER(app);


game::App::App(int argc, char** argv)noexcept
    : m_event_loop()
{

	auto res= CreateThreadPool()
        .and_then( std::bind_front(&App::CreateRenderWindow, this))
        .and_then( std::bind_front(&App::CreateRenderer, this));
   
    if (!res) {
		_logger.Error(std::format("Failed to initialize App: {}", res.error().message()));
    }
}

void game::App::Quit()noexcept
{
    m_event_loop.PostQuit();
}

game::App::~App()
{
}


void game::App::Run()noexcept
{
    m_event_loop.Run();
}

nx::IMessageLoop* game::App::GetEventLoop()noexcept
{
    return &m_event_loop;
}

void game::App::RegisterEvent() noexcept
{
    m_event_loop.SetHandler<nx::CloseEvent>(&App::HandleMainWindowClose, this);
    m_event_loop.On<nx::MousePressEvent>(&App::OnMousePress, this);
}

nx::Error game::App::CreateThreadPool() noexcept
{
    return
        nx::GetCoreMask(nx::High)
        .and_then([&](auto highCore)
        {
                m_threadpool = nx::MakeCommonPtr<nx::Threadpool>(
                    highCore.size(), [&](size_t i)
                    {
                        nx::SetCurrentThreadAffinity(highCore[i]);
                    }
                );
                return nx::Succeeded;
        });
  
}

nx::Error game::App::CreateRenderWindow() noexcept
{
    RETURN_ON_ERROR( m_window.Create(nx::String::from("application"), { 100, 100, 800, 600 }));
    RegisterEvent();
    m_window.Activate();
}

nx::Error game::App::CreateRenderer() noexcept
{
    return m_renderer.Initialize({ .name = "app",.mode = renderer::Mode::Editor }, &m_window, renderer::RendererType::Vulkan);
}

bool game::App::HandleMainWindowClose(const nx::CloseEvent& e)noexcept
{
    nx::EMessageBox<Platform> msgbox;
    auto res = msgbox.show(nx::String::from(L"app"), nx::String::from(L"Quit?"), nx::IMessageBox::Icon::Question,
        nx::IMessageBox::Button::YesNo, e.window);
    if (res == nx::EMessageBox<Platform>::Result::Yes)
    {
        Quit();
        return true;
    }
    return false;
}

void game::App::OnMousePress(const nx::MousePressEvent& e)noexcept
{
    m_event_loop.PostTask([button = e.button, x = e.x, y = e.y, window = e.window]()
        {
            std::string buttonName;
            switch (button)
            {
            case nx::MouseButton::Left:
                buttonName = "Left";
                break;
            case nx::MouseButton::Right:
                buttonName = "Right";
                break;
            case nx::MouseButton::Middle:
                buttonName = "Middle";
                break;
            case nx::MouseButton::X1:
                buttonName = "X1";
                break;
            case nx::MouseButton::X2:
                buttonName = "X2";
            }
            _logger.Info(std::format("Mouse Pressed: button={}, x={}, y={}", buttonName, x, y));
            auto title = window->GetTitle();
            _logger.Info(std::format("Event received from window: {}", title.to_string()));
        });
}


