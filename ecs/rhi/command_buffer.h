#pragma once
namespace nx
{

    class RhiGraphicsCmdView
    {
    public:
        virtual ~RhiGraphicsCmdView() = default;
    };


    class RhiComputeCmdView
    {
    public:
        virtual ~RhiComputeCmdView() = default;
    };

    class RhiTransferCmdView
    {
    public:
        virtual ~RhiTransferCmdView() = default;
    };

    class RhiCmdBuffer
    {
    public:
        virtual ~RhiCmdBuffer() = default;
        virtual void Begin() = 0;
        virtual  void End() = 0;
        virtual void Submit() = 0;

        virtual RhiGraphicsCmdView* GetGraphicsCmdView() = 0;
        virtual RhiComputeCmdView* GetComputeCmdView() = 0;
        virtual RhiTransferCmdView* GetTransferCmdView() = 0;


    };
}