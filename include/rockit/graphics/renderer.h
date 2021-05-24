#pragma once

#include <rockit/core/resource.h>

namespace Rockit
{
    class Renderer
    {
    public:

        Renderer() = delete;
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        virtual int32_t GetWidth() const = 0;
        virtual int32_t GetHeight() const = 0;
        virtual void Clear() = 0;
    };
}