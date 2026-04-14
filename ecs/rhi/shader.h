#pragma once
#include "defs.h"
#include <vector>
#include <string>
namespace  nx{



    struct ShaderCreateInfo
    {
        std::vector<uint32_t> spvData;
    };


    class RhiShader {
    public:
        virtual ~RhiShader() = default;
        virtual  EShaderTypeBits GetType()noexcept = 0;
        virtual  const std::string& GetEntry()noexcept =0;
    };

}