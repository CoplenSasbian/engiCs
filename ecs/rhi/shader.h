#pragma once

namespace  nx{

    enum ShaderStage {
        Vertex,
        Fragment,

    };


    class Shader {
    public:
        virtual ~Shader() = default;
    };

}