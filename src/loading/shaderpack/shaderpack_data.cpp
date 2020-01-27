#include "nova_renderer/shaderpack_data.hpp"

#include "nova_renderer/rhi/rhi_enums.hpp"

#include "../json_utils.hpp"

namespace nova::renderer::shaderpack {
    /*!
     * \brief If a data member isn't in the JSON (which is fully supported and is 100% fine) then we use this to fill in
     * any missing values
     */
    PipelineCreateInfo default_pipeline;

    bool TextureFormat::operator==(const TextureFormat& other) const {
        return pixel_format == other.pixel_format && dimension_type == other.dimension_type && width == other.width &&
               height == other.height;
    }

    bool TextureFormat::operator!=(const TextureFormat& other) const { return !(*this == other); }

    bool TextureAttachmentInfo::operator==(const TextureAttachmentInfo& other) const { return other.name == name; }

    glm::uvec2 TextureFormat::get_size_in_pixels(const glm::uvec2& screen_size) const {
        float pixel_width = width;
        float pixel_height = height;

        if(dimension_type == TextureDimensionTypeEnum::ScreenRelative) {
            pixel_width *= static_cast<float>(screen_size.x);
            pixel_height *= static_cast<float>(screen_size.y);
        }

        return {std::round(pixel_width), std::round(pixel_height)};
    }

    PixelFormatEnum pixel_format_enum_from_string(const std::string& str) {
        if(str == "RGBA8") {
            return PixelFormatEnum::RGBA8;
        }
        if(str == "RGBA16F") {
            return PixelFormatEnum::RGBA16F;
        }
        if(str == "RGBA32F") {
            return PixelFormatEnum::RGBA32F;
        }
        if(str == "Depth") {
            return PixelFormatEnum::Depth;
        }
        if(str == "DepthStencil") {
            return PixelFormatEnum::DepthStencil;
        }

        NOVA_LOG(ERROR) << "Unsupported pixel format " << str;
        return {};
    }

    TextureDimensionTypeEnum texture_dimension_type_enum_from_string(const std::string& str) {
        if(str == "ScreenRelative") {
            return TextureDimensionTypeEnum ::ScreenRelative;
        }
        if(str == "Absolute") {
            return TextureDimensionTypeEnum::Absolute;
        }

        NOVA_LOG(ERROR) << "Unsupported texture dimension type " << str;
        return {};
    }

    TextureFilterEnum texture_filter_enum_from_string(const std::string& str) {
        if(str == "TexelAA") {
            return TextureFilterEnum::TexelAA;
        }
        if(str == "Bilinear") {
            return TextureFilterEnum::Bilinear;
        }
        if(str == "Point") {
            return TextureFilterEnum::Point;
        }

        NOVA_LOG(ERROR) << "Unsupported texture filter " << str;
        return {};
    }

    WrapModeEnum wrap_mode_enum_from_string(const std::string& str) {
        if(str == "Repeat") {
            return WrapModeEnum::Repeat;
        }
        if(str == "Clamp") {
            return WrapModeEnum::Clamp;
        }

        NOVA_LOG(ERROR) << "Unsupported wrap mode " << str;
        return {};
    }

    StencilOpEnum stencil_op_enum_from_string(const std::string& str) {
        if(str == "Keep") {
            return StencilOpEnum::Keep;
        }
        if(str == "Zero") {
            return StencilOpEnum::Zero;
        }
        if(str == "Replace") {
            return StencilOpEnum::Replace;
        }
        if(str == "Incr") {
            return StencilOpEnum::Increment;
        }
        if(str == "IncrWrap") {
            return StencilOpEnum::IncrementAndWrap;
        }
        if(str == "Decr") {
            return StencilOpEnum::Decrement;
        }
        if(str == "DecrWrap") {
            return StencilOpEnum::DecrementAndWrap;
        }
        if(str == "Invert") {
            return StencilOpEnum::Invert;
        }

        NOVA_LOG(ERROR) << "Unsupported stencil op " << str;
        return {};
    }

    CompareOpEnum compare_op_enum_from_string(const std::string& str) {
        if(str == "Never") {
            return CompareOpEnum::Never;
        }
        if(str == "Less") {
            return CompareOpEnum::Less;
        }
        if(str == "LessEqual") {
            return CompareOpEnum::LessEqual;
        }
        if(str == "Greater") {
            return CompareOpEnum::Greater;
        }
        if(str == "GreaterEqual") {
            return CompareOpEnum::GreaterEqual;
        }
        if(str == "Equal") {
            return CompareOpEnum::Equal;
        }
        if(str == "NotEqual") {
            return CompareOpEnum::NotEqual;
        }
        if(str == "Always") {
            return CompareOpEnum::Always;
        }

        NOVA_LOG(ERROR) << "Unsupported compare op " << str;
        return {};
    }

    MsaaSupportEnum msaa_support_enum_from_string(const std::string& str) {
        if(str == "MSAA") {
            return MsaaSupportEnum::MSAA;
        }
        if(str == "Both") {
            return MsaaSupportEnum::Both;
        }
        if(str == "None") {
            return MsaaSupportEnum::None;
        }

        NOVA_LOG(ERROR) << "Unsupported antialiasing mode " << str;
        return {};
    }

    PrimitiveTopologyEnum primitive_topology_enum_from_string(const std::string& str) {
        if(str == "Triangles") {
            return PrimitiveTopologyEnum::Triangles;
        }
        if(str == "Lines") {
            return PrimitiveTopologyEnum::Lines;
        }

        NOVA_LOG(ERROR) << "Unsupported primitive mode " << str;
        return {};
    }

    BlendFactorEnum blend_factor_enum_from_string(const std::string& str) {
        if(str == "One") {
            return BlendFactorEnum::One;
        }
        if(str == "Zero") {
            return BlendFactorEnum::Zero;
        }
        if(str == "SrcColor") {
            return BlendFactorEnum::SrcColor;
        }
        if(str == "DstColor") {
            return BlendFactorEnum::DstColor;
        }
        if(str == "OneMinusSrcColor") {
            return BlendFactorEnum::OneMinusSrcColor;
        }
        if(str == "OneMinusDstColor") {
            return BlendFactorEnum::OneMinusDstColor;
        }
        if(str == "SrcAlpha") {
            return BlendFactorEnum::SrcAlpha;
        }
        if(str == "DstAlpha") {
            return BlendFactorEnum::DstAlpha;
        }
        if(str == "OneMinusSrcAlpha") {
            return BlendFactorEnum::OneMinusSrcAlpha;
        }
        if(str == "OneMinusDstAlpha") {
            return BlendFactorEnum::OneMinusDstAlpha;
        }

        NOVA_LOG(ERROR) << "Unsupported blend factor " << str;
        return {};
    }

    RenderQueueEnum render_queue_enum_from_string(const std::string& str) {
        if(str == "Transparent") {
            return RenderQueueEnum::Transparent;
        }
        if(str == "Opaque") {
            return RenderQueueEnum::Opaque;
        }
        if(str == "Cutout") {
            return RenderQueueEnum::Cutout;
        }

        NOVA_LOG(ERROR) << "Unsupported render queue " << str;
        return {};
    }

    StateEnum state_enum_from_string(const std::string& str) {
        if(str == "Blending") {
            return StateEnum::Blending;
        }
        if(str == "InvertCulling") {
            return StateEnum::InvertCulling;
        }
        if(str == "DisableCulling") {
            return StateEnum::DisableCulling;
        }
        if(str == "DisableDepthWrite") {
            return StateEnum::DisableDepthWrite;
        }
        if(str == "DisableDepthTest") {
            return StateEnum::DisableDepthTest;
        }
        if(str == "EnableStencilTest") {
            return StateEnum::EnableStencilTest;
        }
        if(str == "StencilWrite") {
            return StateEnum::StencilWrite;
        }
        if(str == "DisableColorWrite") {
            return StateEnum::DisableColorWrite;
        }
        if(str == "EnableAlphaToCoverage") {
            return StateEnum::EnableAlphaToCoverage;
        }
        if(str == "DisableAlphaWrite") {
            return StateEnum::DisableAlphaWrite;
        }

        NOVA_LOG(ERROR) << "Unsupported state enum " << str;
        return {};
    }

    rx::string to_string(const PixelFormatEnum val) {
        switch(val) {
            case PixelFormatEnum::RGBA8:
                return "RGBA8";

            case PixelFormatEnum::RGBA16F:
                return "RGBA16F";

            case PixelFormatEnum::RGBA32F:
                return "RGBA32F";

            case PixelFormatEnum::Depth:
                return "Depth";

            case PixelFormatEnum::DepthStencil:
                return "DepthStencil";
        }

        return "Unknown value";
    }

    rx::string to_string(const TextureDimensionTypeEnum val) {
        switch(val) {
            case TextureDimensionTypeEnum::ScreenRelative:
                return "ScreenRelative";

            case TextureDimensionTypeEnum::Absolute:
                return "Absolute";
        }

        return "Unknown value";
    }

    rx::string to_string(const TextureFilterEnum val) {
        switch(val) {
            case TextureFilterEnum::TexelAA:
                return "TexelAA";

            case TextureFilterEnum::Bilinear:
                return "Bilinear";

            case TextureFilterEnum::Point:
                return "Point";
        }

        return "Unknown value";
    }

    rx::string to_string(const WrapModeEnum val) {
        switch(val) {
            case WrapModeEnum::Repeat:
                return "Repeat";

            case WrapModeEnum::Clamp:
                return "Clamp";
        }

        return "Unknown value";
    }

    rx::string to_string(const StencilOpEnum val) {
        switch(val) {
            case StencilOpEnum::Keep:
                return "Keep";

            case StencilOpEnum::Zero:
                return "Zero";

            case StencilOpEnum::Replace:
                return "Replace";

            case StencilOpEnum::Increment:
                return "Incr";

            case StencilOpEnum::IncrementAndWrap:
                return "IncrWrap";

            case StencilOpEnum::Decrement:
                return "Decr";

            case StencilOpEnum::DecrementAndWrap:
                return "DecrWrap";

            case StencilOpEnum::Invert:
                return "Invert";
        }

        return "Unknown value";
    }

    rx::string to_string(const CompareOpEnum val) {
        switch(val) {
            case CompareOpEnum::Never:
                return "Never";

            case CompareOpEnum::Less:
                return "Less";

            case CompareOpEnum::LessEqual:
                return "LessEqual";

            case CompareOpEnum::Greater:
                return "Greater";

            case CompareOpEnum::GreaterEqual:
                return "GreaterEqual";

            case CompareOpEnum::Equal:
                return "Equal";

            case CompareOpEnum::NotEqual:
                return "NotEqual";

            case CompareOpEnum::Always:
                return "Always";
        }

        return "Unknown value";
    }

    rx::string to_string(const MsaaSupportEnum val) {
        switch(val) {
            case MsaaSupportEnum::MSAA:
                return "MSAA";

            case MsaaSupportEnum::Both:
                return "Both";

            case MsaaSupportEnum::None:
                return "None";
        }

        return "Unknown value";
    }

    rx::string to_string(const PrimitiveTopologyEnum val) {
        switch(val) {
            case PrimitiveTopologyEnum::Triangles:
                return "Triangles";

            case PrimitiveTopologyEnum::Lines:
                return "Lines";
        }

        return "Unknown value";
    }

    rx::string to_string(const BlendFactorEnum val) {
        switch(val) {
            case BlendFactorEnum::One:
                return "One";

            case BlendFactorEnum::Zero:
                return "Zero";

            case BlendFactorEnum::SrcColor:
                return "SrcColor";

            case BlendFactorEnum::DstColor:
                return "DstColor";

            case BlendFactorEnum::OneMinusSrcColor:
                return "OneMinusSrcColor";

            case BlendFactorEnum::OneMinusDstColor:
                return "OneMinusDstColor";

            case BlendFactorEnum::SrcAlpha:
                return "SrcAlpha";

            case BlendFactorEnum::DstAlpha:
                return "DstAlpha";

            case BlendFactorEnum::OneMinusSrcAlpha:
                return "OneMinusSrcAlpha";

            case BlendFactorEnum::OneMinusDstAlpha:
                return "OneMinusDstAlpha";
        }

        return "Unknown value";
    }

    rx::string to_string(const RenderQueueEnum val) {
        switch(val) {
            case RenderQueueEnum::Transparent:
                return "Transparent";

            case RenderQueueEnum::Opaque:
                return "Opaque";

            case RenderQueueEnum::Cutout:
                return "Cutout";
        }

        return "Unknown value";
    }

    rx::string to_string(const StateEnum val) {
        switch(val) {
            case StateEnum::Blending:
                return "Blending";

            case StateEnum::InvertCulling:
                return "InvertCulling";

            case StateEnum::DisableCulling:
                return "DisableCulling";

            case StateEnum::DisableDepthWrite:
                return "DisableDepthWrite";

            case StateEnum::DisableDepthTest:
                return "DisableDepthTest";

            case StateEnum::EnableStencilTest:
                return "EnableStencilTest";

            case StateEnum::StencilWrite:
                return "StencilWrite";

            case StateEnum::DisableColorWrite:
                return "DisableColorWrite";

            case StateEnum::EnableAlphaToCoverage:
                return "EnableAlphaToCoverage";

            case StateEnum::DisableAlphaWrite:
                return "DisableAlphaWrite";
        }

        return "Unknown value";
    }

    uint32_t pixel_format_to_pixel_width(const PixelFormatEnum format) {
        switch(format) {
            case PixelFormatEnum::RGBA8:
                return 4 * 8;

            case PixelFormatEnum::RGBA16F:
                return 4 * 16;

            case PixelFormatEnum::RGBA32F:
                return 4 * 32;

            case PixelFormatEnum::Depth:
                return 32;

            case PixelFormatEnum::DepthStencil:
                return 32;

            default:
                return 32;
        }
    }

    PixelFormatEnum to_pixel_format_enum(const rhi::PixelFormat format) {
        switch(format) {
            case rhi::PixelFormat::Rgba8:
                return PixelFormatEnum::RGBA8;

            case rhi::PixelFormat::Rgba16F:
                return PixelFormatEnum::RGBA16F;

            case rhi::PixelFormat::Rgba32F:
                return PixelFormatEnum::RGBA32F;

            case rhi::PixelFormat::Depth32:
                return PixelFormatEnum::Depth;

            case rhi::PixelFormat::Depth24Stencil8:
                return PixelFormatEnum::DepthStencil;

            default:
                return PixelFormatEnum::RGBA8;
        }
    }

    rhi::PixelFormat to_rhi_pixel_format(const PixelFormatEnum format) {
        switch(format) {
            case PixelFormatEnum::RGBA8:
                return rhi::PixelFormat::Rgba8;

            case PixelFormatEnum::RGBA16F:
                return rhi::PixelFormat::Rgba16F;

            case PixelFormatEnum::RGBA32F:
                return rhi::PixelFormat::Rgba32F;

            case PixelFormatEnum::Depth:
                return rhi::PixelFormat::Depth32;

            case PixelFormatEnum::DepthStencil:
                return rhi::PixelFormat::Depth24Stencil8;

            default:
                return rhi::PixelFormat::Rgba8;
        }
    }
} // namespace nova::renderer::shaderpack