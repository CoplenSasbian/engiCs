module;
#include <cstdint>

export  module nx.platform.event_loop.event;
export import nx.platform.window.window;


export
namespace nx
{

// --- Base Event Structure ---
struct EventBase
{
    IWindow* window;
};

// When the window is closed
struct CloseEvent : EventBase
{
};

// When the window size changes
struct ResizeEvent : EventBase
{
    int width;
    int height;
};

// When the window position moves
struct MoveEvent : EventBase
{
    int x;
    int y;
};

// When the window gains focus
struct FocusEvent: EventBase
{
};

// When the window loses focus
struct LostFocusEvent: EventBase
{
};

// --- Mouse Button Enum ---
enum class MouseButton : uint8_t
{
    Left = 0,
    Right,
    Middle,
    X1, // Usually the back button
    X2  // Usually the forward button
};

// --- Keyboard Special Key Enum ---
enum class Key : uint16_t
{
    // Control character keys
    Backspace = 0x08,
    Tab       = 0x09,
    Enter     = 0x0D,
    Escape    = 0x1B,

    // Space
    Space     = 0x20,

    // Arrow keys
    Left      = 0x25,
    Up        = 0x26,
    Right     = 0x27,
    Down      = 0x28,

    // Function keys F1-F12
    F1  = 0x70,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    // Other common special keys
    PageUp   = 0x21,
    PageDown = 0x22,
    End      = 0x23,
    Home     = 0x24,
    Insert   = 0x2D,
    Delete   = 0x2E,

    // Number pad (Numpad)
    Numpad0 = 0x60,
    Numpad1,
    Numpad2,
    Numpad3,
    Numpad4,
    Numpad5,
    Numpad6,
    Numpad7,
    Numpad8,
    Numpad9,
    NumpadAdd,      // +
    NumpadSubtract, // -
    NumpadMultiply, // *
    NumpadDivide,   // /
    NumpadDecimal,  // .
    NumpadEnter,    // Enter

    // Modifier Keys
    LShift   = 0xA0,
    RShift,
    LControl,
    RControl,
    LAlt,
    RAlt,
    LSystem, // Left System Key (e.g., Windows key)
    RSystem, // Right System Key (e.g., Windows key)
};

// --- Mouse Events ---
struct MouseMoveEvent : EventBase
{
    float x; // Current mouse position x coordinate
    float y; // Current mouse position y coordinate
};

struct MousePressEvent : EventBase
{
    MouseButton button; // The pressed button
    float x;          // x coordinate when pressed
    float y;          // y coordinate when pressed
};

struct MouseReleaseEvent : EventBase
{
    MouseButton button; // The released button
    float x;          // x coordinate when released
    float y;          // y coordinate when released
};

struct MouseWheelEvent : EventBase
{
    float delta_x; // Horizontal scroll increment
    float delta_y; // Vertical scroll increment
    float x;       // Mouse x coordinate during scrolling
    float y;       // Mouse y coordinate during scrolling
};

// --- Keyboard Events ---
struct KeyPressEvent : EventBase
{
    Key key_code; // The code of the pressed key
    bool is_repeat; // Whether it's a repeated key press
};

struct KeyReleaseEvent : EventBase
{
    Key key_code; // The code of the released key
};

struct KeyCharEvent : EventBase
{
    uint32_t character; // The input character (UTF-32)
};





}