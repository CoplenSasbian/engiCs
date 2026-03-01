module;
#include <cstdint>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <boost/container/flat_map.hpp>
#include <memory>
#include <optional>
export module nx.platform.window.win32;
export import nx.platform.window;
import nx.core.types.tstring;

export namespace nx {
	class Win32Window :public IWindow {
	public:
		Win32Window();
		void* NativeHandle() override;

		std::optional<NxError> Initialize() override;

		void Shutdown()override;
		void Resize(int width, int height) override;
		void Move(int x, int y) override;

		void SetTitle(tchar_view title) override;
		void SetVisible(bool visible) override;
		void SetCursorVisible(bool visible) override;
		void SetCursorLocked(bool locked) override;
		void SetCursorPosition(double x, double y) override;
	protected:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	private:
		int64_t m_threadId;
        HWND m_window;

   };

	
}