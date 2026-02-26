
export module game.render_window;
import nx.platform.window.win32;
export namespace  game{

    using namespace nx;

    class RenderWindow :public Win32Window {
    public:
        bool OnCreate(ECreate& )override;
    };


}
