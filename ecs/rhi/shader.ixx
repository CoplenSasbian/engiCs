

export module nx.rhi.shader;
export namespace  nx{

    enum ShaderStage {
        Vertex,
        Fragment,

    };


    class Shader {
    public:
        virtual ~Shader() = default;
    };

}