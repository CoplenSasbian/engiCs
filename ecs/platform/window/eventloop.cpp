module;
#include <memory>	
module nx.platform.window.eventloop;
import nx.platform.window.win32.eventloop;
import nx.core.memory;

std::shared_ptr<nx::IEventLoop> nx::MakeWin32EventLoop()
{
	return nx::MakeShared<nx::Win32EventLoop>();
}