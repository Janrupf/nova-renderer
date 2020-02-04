#pragma once

#include "nova_renderer/constants.hpp"
#include "nova_renderer/filesystem/virtual_filesystem.hpp"
#include "nova_renderer/nova_settings.hpp"
#include "nova_renderer/pipeline_storage.hpp"
#include "nova_renderer/procedural_mesh.hpp"
#include "nova_renderer/renderables.hpp"
#include "nova_renderer/renderdoc_app.h"
#include "nova_renderer/rendergraph.hpp"
#include "nova_renderer/resource_loader.hpp"
#include "nova_renderer/rhi/device_memory_resource.hpp"
#include "nova_renderer/rhi/forward_decls.hpp"
#include "nova_renderer/rhi/render_device.hpp"
#include "nova_renderer/util/container_accessor.hpp"

void init_rex();
void rex_fini();

namespace spirv_cross {
    class CompilerGLSL;
    struct Resource;
} // namespace spirv_cross

namespace nova::renderer {
    using LogHandles = rx::vector<rx::log::event_type::handle>;

    /*!
     * \brief Registers a log message writing function
     *
     * This function removes any previously registered logging handler, replacing it with the provided function
     *
     * If you don't call this function, Nova will send all log messages to `stdout`
     *
     * You may manually unregister your handler by calling `LogHandles::clear()`, but you don't need to. This function is intentionally not
     * marked `[[nodiscard]]` because doing things with the handles is completely optional
     */
    template <typename LogHandlerFunc>
    LogHandles& set_logging_handler(LogHandlerFunc&& log_handler);

    class UiRenderpass;

    namespace rhi {
        class Swapchain;
    }

#pragma region Runtime optimized data
    struct Mesh {
        rhi::Buffer* vertex_buffer = nullptr;
        rhi::Buffer* index_buffer = nullptr;

        uint32_t num_indices = 0;
    };
#pragma endregion

    using ProceduralMeshAccessor = MapAccessor<MeshId, ProceduralMesh>;

    /*!
     * \brief Main class for Nova. Owns all of Nova's resources and provides a way to access them
     * This class exists as a singleton so it's always available
     */
    class NovaRenderer {
    public:
        /*!
         * \brief Initializes the Nova Renderer
         */
        explicit NovaRenderer(const NovaSettings& settings);

        NovaRenderer(NovaRenderer&& other) noexcept = delete;
        NovaRenderer& operator=(NovaRenderer&& other) noexcept = delete;

        NovaRenderer(const NovaRenderer& other) = delete;
        NovaRenderer& operator=(const NovaRenderer& other) = delete;

        ~NovaRenderer();

        /*!
         * \brief Loads the shaderpack with the given name
         *
         * This method will first try to load from the `shaderpacks/` folder (mimicking Optifine shaders). If the
         * shaderpack isn't found there, it'll try to load it from the `resourcepacks/` directory (mimicking Bedrock
         * shaders). If the shader can't be found at either place, a `nova::resource_not_found` exception will be thrown
         *
         * Loading a shaderpack will cause a stall in the GPU. Nova will have to wait for all in-flight frames to finish, then replace the
         * current shaderpack with the new one, then start rendering. Replacing the shaderpack might also require reloading all chunks, if
         * the new shaderpack has different geometry filters then the current one
         *
         * \param renderpack_name The name of the shaderpack to load
         */
        void load_renderpack(const rx::string& renderpack_name);

        /*!
         * \brief Gives Nova a function to use to render UI
         *
         * This function will be executed inside the builtin UI renderpass. This renderpass takes the output of the 3D renderer, adds the UI
         * on top of it, and writes that all to the backbuffer
         *
         * The first parameter to the function is the command list it must record UI rendering into, and the second parameter is the
         * rendering context for the current frame
         *
         * Before calling the UI render function, Nova records commands to begin a renderpass with one RGBA8 color attachment and one D24S8
         * depth/stencil attachment. After calling this function, Nova records commands to end that same renderpass. This allows the host
         * application to only care about rendering the UI, instead of worrying about any pass scheduling concerns
         *
         * \param ui_renderpass The renderpass to use for UI
         * \param create_info The create info for the renderpass
         *
         * \return The renderpass you added, but you no longer have ownership
         */
        template <typename RenderpassType>
        bool set_ui_renderpass(RenderpassType* ui_renderpass, const shaderpack::RenderPassCreateInfo& create_info);

        [[nodiscard]] const rx::vector<MaterialPass>& get_material_passes_for_pipeline(rhi::Pipeline* const pipeline);

        [[nodiscard]] rx::optional<RenderpassMetadata> get_renderpass_metadata(const rx::string& renderpass_name) const;

        /*!
         * \brief Executes a single frame
         */
        void execute_frame();

        [[nodiscard]] NovaSettingsAccessManager& get_settings();

        [[nodiscard]] rx::memory::allocator* get_global_allocator() const;

#pragma region Meshes
        /*!
         * \brief Tells Nova how many meshes you expect to have in your scene
         *
         * Allows the Nova Renderer to preallocate space for your meshes
         *
         * \param num_meshes The number of meshes you expect to have
         */
        void set_num_meshes(uint32_t num_meshes);

        /*!
         * \brief Creates a new mesh and uploads its data to the GPU, returning the ID of the newly created mesh
         *
         * \param mesh_data The mesh's initial data
         */
        [[nodiscard]] MeshId create_mesh(const MeshData& mesh_data);

        /*!
         * \brief Creates a procedural mesh, returning both its mesh id and
         */
        [[nodiscard]] ProceduralMeshAccessor create_procedural_mesh(uint64_t vertex_size, uint64_t index_size);

        /*!
         * \brief Destroys the mesh with the provided ID, freeing up whatever VRAM it was using
         *
         * In debug builds, this method checks that no renderables are using the mesh
         *
         * \param mesh_to_destroy The handle of the mesh you want to destroy
         */
        void destroy_mesh(MeshId mesh_to_destroy);
#pragma endregion

#pragma region Resources
        [[nodiscard]] rhi::Sampler* get_point_sampler() const;
#pragma endregion

        /*!
         * \brief Binds the resources for one material to that material's descriptor sets
         *
         * This method does not perform any validation. It assumes that descriptor_descriptions has a description for
         * every descriptor that the material wants to bind to, and it assumes that material has all the needed
         * descriptors
         *
         * \param material The material to bind resources to
         * \param bindings What resources should be bound to what descriptor. The key is the descriptor name and the
         * value is the resource name
         * \param descriptor_descriptions A map from descriptor name to all the information you need to update that
         * descriptor
         */
        void bind_data_to_material_descriptor_sets(const MaterialPass& material,
                                                   const rx::map<rx::string, rx::string>& bindings,
                                                   const rx::map<rx::string, rhi::ResourceBindingDescription>& descriptor_descriptions);

        [[nodiscard]] RenderableId add_renderable_for_material(const FullMaterialPassName& material_name,
                                                               const StaticMeshRenderableData& renderable);

        [[nodiscard]] rhi::RenderDevice& get_engine() const;

        [[nodiscard]] NovaWindow& get_window() const;

        [[nodiscard]] DeviceResources& get_resource_manager() const;

        [[nodiscard]] PipelineStorage& get_pipeline_storage() const;

    private:
        NovaSettingsAccessManager render_settings;

        std::unique_ptr<rhi::RenderDevice> device;
        std::unique_ptr<NovaWindow> window;
        rhi::Swapchain* swapchain;

        RENDERDOC_API_1_3_0* render_doc;
        rx::vector<rx::memory::allocator*> frame_allocators;

        rhi::Sampler* point_sampler;

        /*!
         * \brief The allocator that all of Nova's memory will be allocated through
         *
         * Local allocators 0.1 uwu
         *
         * Right now I throw this allocator at the GPU memory allocators, because they need some way to allocate memory and I'm not about to
         * try and band-aid aid things together. Future work will have a better way to bootstrap Nova's allocators
         */
        rx::memory::allocator* global_allocator;

        /*!
         * \brief Holds all the object loaded by the current rendergraph
         */
        rx::memory::allocator* renderpack_allocator;

        DeviceResources* device_resources;

        DeviceMemoryResource* mesh_memory;

        DeviceMemoryResource* ubo_memory;

        rhi::DescriptorPool* global_descriptor_pool;

        DeviceMemoryResource* staging_buffer_memory;
        void* staging_buffer_memory_ptr;

#pragma region Initialization
        void create_global_allocators();

        static void initialize_virtual_filesystem();

        /*!
         * \brief Creates global GPU memory pools
         *
         * Creates pools for mesh data and for uniform buffers. The size of the mesh memory pool is a guess that might be true for some
         * games, I'll get more accurate guesses when I have actual data. The size of the uniform buffer pool is the size of the builtin
         * uniform buffers plus memory for the estimated number of renderables, which again will just be a guess and probably not a
         * super good one
         */
        void create_global_gpu_pools();

        void create_global_sync_objects();

        void create_resource_storage();

        void create_builtin_render_targets() const;

        void create_uniform_buffers();

        void create_renderpass_manager();

        void create_builtin_renderpasses() const;
#pragma endregion

#pragma region Renderpack
        struct PipelineReturn {
            Pipeline pipeline;
            PipelineMetadata metadata;
        };

        bool shaderpack_loaded = false;

        rx::concurrency::mutex shaderpack_loading_mutex;

        rx::optional<shaderpack::RenderpackData> loaded_renderpack;

        Rendergraph* rendergraph;
#pragma endregion

#pragma region Rendergraph
        rx::map<rx::string, rhi::Image*> builtin_images;
        rx::map<rx::string, renderer::Renderpass*> builtin_renderpasses;

        rx::map<rx::string, shaderpack::TextureCreateInfo> dynamic_texture_infos;

        void create_dynamic_textures(const rx::vector<shaderpack::TextureCreateInfo>& texture_create_infos);

        void create_render_passes(const rx::vector<shaderpack::RenderPassCreateInfo>& pass_create_infos,
                                  const rx::vector<shaderpack::PipelineCreateInfo>& pipelines) const;

        void destroy_dynamic_resources();

        void destroy_renderpasses();
#pragma endregion

#pragma region Rendering pipelines
        PipelineStorage* pipeline_storage;

        rx::map<rhi::Pipeline*, rx::vector<MaterialPass>> passes_by_pipeline;

        rx::map<FullMaterialPassName, MaterialPassMetadata> material_metadatas;

        void create_pipelines_and_materials(const rx::vector<shaderpack::PipelineCreateInfo>& pipeline_create_infos,
                                            const rx::vector<shaderpack::MaterialData>& materials);

        void create_materials_for_pipeline(const renderer::Pipeline& pipeline,
                                           const rx::vector<shaderpack::MaterialData>& materials,
                                           const rx::string& pipeline_name);

        void destroy_pipelines();

        void destroy_materials();
#pragma endregion

#pragma region Meshes
        MeshId next_mesh_id = 0;

        rx::map<MeshId, Mesh> meshes;
        rx::map<MeshId, ProceduralMesh> proc_meshes;
#pragma endregion

#pragma region Rendering
        uint64_t frame_count = 0;
        uint8_t cur_frame_idx = 0;

        rx::vector<rx::string> builtin_buffer_names;
        uint32_t cur_model_matrix_index = 0;

        rx::array<rhi::Fence* [NUM_IN_FLIGHT_FRAMES]> frame_fences;

        rx::map<FullMaterialPassName, MaterialPassKey> material_pass_keys;

        rx::concurrency::mutex ui_function_mutex;
#pragma endregion
    };

    template <typename LogHandlerFunc>
    LogHandles& set_logging_handler(LogHandlerFunc&& log_handler) {
        rx::global_group* system_group{rx::globals::find("system")};

        auto* log_handles = system_group->find("log_handles")->cast<LogHandles>();
        log_handles->clear();

        rx::globals::find("loggers")->each(
            [&](rx::global_node* _logger) { log_handles->push_back(_logger->cast<rx::log>()->on_write(log_handler)); });

        return *log_handles;
    }

    template <typename RenderpassType>
    bool NovaRenderer::set_ui_renderpass(RenderpassType* ui_renderpass, const shaderpack::RenderPassCreateInfo& create_info) {
        return rendergraph->add_renderpass(ui_renderpass, create_info, *device_resources);
    }
} // namespace nova::renderer
