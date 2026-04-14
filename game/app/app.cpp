#include "app.h"
#include "core/log/log.h"
#include "concurrency/utils/processor_info.h"
#include "platform/win32/event_loop/event_converter.h"
#include "platform/window/window.h"
LOGGER(app);


game::App::App(int argc, char** argv)
    : m_event_loop()
{
    m_window.Create(nx::String::from("app"), {100, 100, 800, 600});
    auto ret = nx::GetCoreMask(nx::High);
    if (!ret)
    {
        _logger.Critical(std::format("Get core mask failed :{}", ret.error().message()));
        std::abort();
    }
    auto highCore = ret.value();

    m_threadpool = nx::MakeCommonPtr<nx::Threadpool>(
        highCore.size(), [&](size_t i)
        {
            nx::SetCurrentThreadAffinity(highCore[i]);
        }
    );


    m_window.Activate();

    m_event_loop.SetHandler<nx::CloseEvent>([&](const nx::CloseEvent& e)
    {
        nx::EMessageBox<Platform> msgbox;
        auto res = msgbox.show(nx::String::from(L"app"), nx::String::from(L"Quit?"), nx::IMessageBox::Icon::Question,
                               nx::IMessageBox::Button::YesNo);
        if (res == nx::EMessageBox<Platform>::Result::Yes)
        {
            Quit();
			return true;
        }
        return false;
    });

    if (auto ret = m_renderer.Initialize({.name = "app",.mode = renderer::Mode::Editor}, &m_window, renderer::RendererType::Vulkan); !ret)
    {
        _logger.Critical(std::format("Get core mask failed :{}", ret.error().message()));
        std::abort();
    }
}

void game::App::Quit()
{
    m_event_loop.PostQuit();
}

game::App::~App()
{
}


void game::App::Run()
{
    m_event_loop.Run();
}

nx::IMessageLoop* game::App::GetEventLoop()
{
    return &m_event_loop;
}
