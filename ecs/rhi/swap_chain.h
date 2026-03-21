#pragma once
#include "resource.h"
#include "surface.h"
#include "core/error_code.h"


namespace nx
{

    class RhiSwapChain
    {
    public:
        RhiSwapChain()=default;
        virtual Error Initialize(RhiSurface*,uint32_t,uint32_t)noexcept=0;
        virtual Error Resize(uint32_t,uint32_t)noexcept=0;
        virtual const RhiImage *GetImage(uint32_t)const noexcept=0;
        virtual uint32_t GetImageCount()const noexcept=0;
        virtual uint32_t CurrentImageIndex()const noexcept=0;
        virtual Error Present()noexcept=0;
        virtual ~RhiSwapChain()=default;
    };
}

