module;
#include <cstdint>

export module nx.platform.window;
export import nx.platform.window.eventloop;
import nx.platform.error_code;
import nx.core.types;
import nx.core.types.tstring;
import nx.core.utils.emitter;
export namespace nx {

    class IWindow;

    enum class EventType {
        CREATE,CLOSE,RESIZE,MOVE,

        FOCUS,BLUR,

        MOUSE_MOVE, MOUSE_BUTTON, MOUSE_WHEEL,

        KEY, CHAR
    };


    class IEvent {
    public:
        explicit IEvent(EventType type, IWindow* window = nullptr) noexcept
            : m_eventType(type), m_window(window) {}

        virtual ~IEvent() = default;

        [[nodiscard]] EventType GetType() const noexcept  { return m_eventType; }
        [[nodiscard]] IWindow* GetWindow() const noexcept   { return m_window; }

        bool NeedBroadcast() noexcept { return m_broadcast; }
        void SetNeedBroadcast(bool broadcast = true) noexcept{ m_broadcast = broadcast; }

    private:
        EventType m_eventType;
        IWindow* m_window = nullptr;
        bool m_broadcast = true;
    };

    class ECreate : public IEvent {
    public:
        explicit ECreate(IWindow*  window)noexcept:IEvent(EventType::CREATE,window){}
        ~ECreate() override = default;
    };

    class EClose : public IEvent {
    public:
        explicit EClose(IWindow* window)noexcept:IEvent(EventType::CLOSE,window){}
        ~EClose() override = default;
    };

    class EResize : public IEvent {
    public:
        ~EResize() override = default;
        EResize(IWindow* window,int width,int height)noexcept
        :IEvent(EventType::RESIZE,window) ,
        m_width(width),m_height(height)
        {}

        [[nodiscard]] int GetWidth() const noexcept{ return m_width; }
        [[nodiscard]] int GetHeight() const noexcept{ return m_height; }

    private:
        int m_width ;
        int m_height ;
    };
    class EMove : public IEvent {
    public:
        ~EMove() override = default;
        EMove(IWindow* window,int x,int y)noexcept
            : IEvent(EventType::MOVE,window) ,
        m_x(x),m_y(y){}
        [[nodiscard]] int GetX()const noexcept{ return m_x; }
        [[nodiscard]] int GetY()const noexcept{ return m_y; }
    private:
        int m_x;
        int m_y;

    };
    class EFocus : public IEvent {
        public:
        ~EFocus() override = default;

        explicit EFocus(IWindow* window)noexcept
        : IEvent(EventType::FOCUS,window){}
    };
    class EBlur : public IEvent {
        public:
        ~EBlur() override = default;

        explicit EBlur(IWindow* window)noexcept
        : IEvent(EventType::BLUR,window){}
    };

    // Mouse and keyboard related events
    class EMouseMove : public IEvent {
    public:
        ~EMouseMove() override = default;
        EMouseMove(IWindow* window, int x, int y) noexcept
            : IEvent(EventType::MOUSE_MOVE, window), m_x(x), m_y(y) {}

        [[nodiscard]] int GetX() const noexcept { return m_x; }
        [[nodiscard]] int GetY() const noexcept { return m_y; }

    private:
        int m_x;
        int m_y;
    };

    class EMouseButton : public IEvent {
    public:
        enum class Button { Left, Right, Middle };
        ~EMouseButton() override = default;
        EMouseButton(IWindow* window, Button b, bool pressed, int x, int y) noexcept
            : IEvent(EventType::MOUSE_BUTTON, window), m_button(b), m_pressed(pressed), m_x(x), m_y(y) {}

        [[nodiscard]] Button GetButton() const noexcept { return m_button; }
        [[nodiscard]] bool IsPressed() const noexcept { return m_pressed; }
        [[nodiscard]] int GetX() const noexcept { return m_x; }
        [[nodiscard]] int GetY() const noexcept { return m_y; }

    private:
        Button m_button;
        bool m_pressed;
        int m_x;
        int m_y;
    };

    class EMouseWheel : public IEvent {
    public:
        ~EMouseWheel() override = default;
        EMouseWheel(IWindow* window, double delta, int x, int y) noexcept
            : IEvent(EventType::MOUSE_WHEEL, window), m_delta(delta), m_x(x), m_y(y) {}

        [[nodiscard]] double GetDelta() const noexcept { return m_delta; }
        [[nodiscard]] int GetX() const noexcept { return m_x; }
        [[nodiscard]] int GetY() const noexcept { return m_y; }

    private:
        double m_delta;
        int m_x;
        int m_y;
    };

    class EKey : public IEvent {
    public:
        ~EKey() override = default;
        EKey(IWindow* window, int key, bool pressed, int scancode, int modifiers) noexcept
            : IEvent(EventType::KEY, window), m_key(key), m_pressed(pressed), m_scancode(scancode), m_modifiers(modifiers) {}

        [[nodiscard]] int GetKey() const noexcept { return m_key; }
        [[nodiscard]] bool IsPressed() const noexcept { return m_pressed; }
        [[nodiscard]] int GetScanCode() const noexcept { return m_scancode; }
        [[nodiscard]] int GetModifiers() const noexcept { return m_modifiers; }

    private:
        int m_key;
        bool m_pressed;
        int m_scancode;
        int m_modifiers;
    };

    class EChar : public IEvent {
    public:
        ~EChar() override = default;
        EChar(IWindow* window, uint32_t code) noexcept
            : IEvent(EventType::CHAR, window), m_code(code) {}

        [[nodiscard]] uint32_t GetCode() const noexcept { return m_code; }

    private:
        uint32_t m_code;
    };



    class IWindow :public IComponent {
    public:

        ~IWindow() override = default;

        virtual void* NativeHandle() = 0;
        Error Initialize() override = 0;
        void Shutdown() override = 0;
        virtual void Resize(int width, int height) = 0;
        virtual void Move(int x, int y) = 0;

        virtual void SetTitle(tchar_view title) = 0;
        virtual void SetVisible(bool visible) = 0;
        virtual void SetCursorVisible(bool visible) = 0;
        virtual void SetCursorLocked(bool locked) = 0;
        virtual void SetCursorPosition(double x, double y) = 0;
    protected:
        virtual bool OnCreate(ECreate&) {return true;}
        virtual bool OnClose(EClose&) {return true;}
        virtual bool OnResize(EResize&) {return true;}
        virtual bool OnMove(EMove&) {return true;}
        virtual bool OnFocus(EFocus&) {return true;}
        virtual bool OnBlur(EBlur&) {return true;}
        virtual bool OnMouseMove(EMouseMove&) { return true; }
        virtual bool OnMouseButton(EMouseButton&) { return true; }
        virtual bool OnMouseWheel(EMouseWheel&) { return true; }
        virtual bool OnKey(EKey&) { return true; }
        virtual bool OnChar(EChar&) { return true; }
    public:
        TypeEmitter<ECreate> OnCreated;
        TypeEmitter<EClose> OnClosed;
        TypeEmitter<EResize> OnResized;
        TypeEmitter<EMove> OnMoved;
        TypeEmitter<EFocus> OnFocused;
        TypeEmitter<EBlur> OnBlured;
        TypeEmitter<EMouseMove> OnMouseMoved;
        TypeEmitter<EMouseButton> OnMouseButtoned;
        TypeEmitter<EMouseWheel> OnMouseWheeled;
        TypeEmitter<EKey> OnKeyed;
        TypeEmitter<EChar> OnCharred;

    };



}