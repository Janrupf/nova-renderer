#include "nova_renderer/rhi/rhi_types.hpp"

namespace nova::renderer::rhi {
    bool ResourceBindingDescription::operator==(const ResourceBindingDescription& other) {
        return set == other.set && binding == other.binding && count == other.count && type == other.type;
    }

    bool ResourceBindingDescription::operator!=(const ResourceBindingDescription& other) { return !(*this == other); }

    ResourceBarrier::ResourceBarrier() = default;

    DescriptorResourceInfo::DescriptorResourceInfo() = default;

    uint32_t PipelineInterface::get_num_descriptors_of_type(const DescriptorType type) const {
        uint32_t num_descriptors = 0;
        bindings.each_value([&](const ResourceBindingDescription& description) {
            if(description.type == type) {
                num_descriptors++;
            }
        });

        return num_descriptors;
    }

    ShaderStage operator|=(const ShaderStage lhs, const ShaderStage rhs) {
        return static_cast<ShaderStage>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    }

    bool is_depth_format(const PixelFormat format) {
        switch(format) {
            case PixelFormat::Rgba8:
                [[fallthrough]];
            case PixelFormat::Rgba16F:
                [[fallthrough]];
            case PixelFormat::Rgba32F:
                return false;

            case PixelFormat::Depth32:
                [[fallthrough]];
            case PixelFormat::Depth24Stencil8:
                return true;

            default:
                return false;
        }
    }

    uint32_t get_byte_size(const VertexFieldFormat format) {
        switch(format) {
            case VertexFieldFormat::Uint:
                return 4;

            case VertexFieldFormat::Float2:
                return 8;

            case VertexFieldFormat::Float3:
                return 12;

            case VertexFieldFormat::Float4:
                return 16;

            default:
                return 16;
        }
    }
} // namespace nova::renderer::rhi
