module nx.game.actor;


namespace nx {
    class Actor {
    public:
        Actor() = default;
        virtual ~Actor() = default;

        //
        // virtual exec::task<void> Initialize() { co_return; }
        // virtual exec::task<void> Update(double deltaTime) { co_return; }
        // virtual exec::task<void> Render() { co_return; }
        // virtual exec::task<void> Shutdown() { co_return; }

    };
}