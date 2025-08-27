#pragma once

namespace tryn::gfx
{
    enum struct TextureFormat
    {
        // 32-bit unsigned normalized
        R8G8B8A8_UNORM,
        B8G8R8A8_UNORM,

        // 32-bit sRGB
        R8G8B8A8_UNORM_SRGB,
        B8G8R8A8_UNORM_SRGB,

        // 32-bit float
        R32_FLOAT,
        R32G32_FLOAT,
        R32G32B32_FLOAT,
        R32G32B32A32_FLOAT,

        // 16-bit float
        R16_FLOAT,
        R16G16_FLOAT,
        R16G16B16A16_FLOAT,

        // 32-bit uint
        R32_UINT,
        R32G32_UINT,
        R32G32B32_UINT,
        R32G32B32A32_UINT,

        // Depth/stencil formats
        D32_FLOAT,
        D24_UNORM_S8_UINT,

        // Compressed formats
        BC1_UNORM,
        BC3_UNORM,
        BC7_UNORM,

        // Other
        UNKNOWN
    };

    enum struct TextureUsage
    {
	    GPUOnly,
	    CPUReadWrite,
	    Upload,
	    Readback,
	    Unknown,
        RenderTarget,
        DepthStencil
    };

    inline const char* to_string(const TextureUsage e)
    {
	    switch (e)
	    {
	    case TextureUsage::GPUOnly: return "GPUOnly";
	    case TextureUsage::CPUReadWrite: return "CPUReadWrite";
	    case TextureUsage::Upload: return "Upload";
        case TextureUsage::Readback: return "Readback";
        case TextureUsage::RenderTarget: return "RenderTarget";
        case TextureUsage::DepthStencil: return "DepthStencil";
	    case TextureUsage::Unknown: return "Unknown";
	    default: return "unknown";
	    }
    }
}