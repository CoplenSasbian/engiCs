module;

export module nx.platform.window.window;
import nx.core.error_code;
import nx.platform;

namespace nx
{





    export
    struct Rect
    {
        int x, y, width, height;
    };

    export
    class IWindow:public IPlatformComponent
    {
    public:
        IWindow();
        ~IWindow() = default;

        virtual void* NativeHandle() const noexcept = 0;

        // Activate the window and bring it to the foreground.
        virtual void Activate() noexcept = 0;

        // Close the window.
        virtual void Close() noexcept = 0;

        // Get the dimensions of the client area (the drawable part of the window).
        [[nodiscard]] virtual Result<Rect> GetClientArea() const noexcept = 0;

        // Get the overall dimensions of the window, including decorations like title bar and borders.
        [[nodiscard]] virtual Result<Rect> GetWindowArea() const noexcept = 0;

        // Check if the window is currently visible.
        [[nodiscard]] virtual bool IsVisible() const noexcept = 0;

        // Maximize the window to fill the screen or its parent container.
        virtual Error Maximize() noexcept = 0;

        // Minimize the window to the taskbar or dock.
        virtual Error Minimize() noexcept = 0;

        // Move and/or resize the window to the specified rectangle.
        // @param rect The new position (x, y) and size (width, height) for the window.
        virtual Error SetWindowArea(const Rect& rect) noexcept = 0;

        // Restore the window from a minimized or maximized state.
        virtual Error Restore() noexcept = 0;

        // Show or hide the window.
        // @param show True to show the window, false to hide it.
        virtual Error SetVisible(bool show) noexcept = 0;


    };
}
