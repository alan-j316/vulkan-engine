/**
 * @file Application.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: September 23, 2026
 *
 * @brief Implementation of member functions declared in Application.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Application.h"

#include "utils/Logger.h"

#include <SDL3/SDL.h>
#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace tempest
{

    TempestApp::TempestApp(std::string name, const uint32_t version) noexcept
        : _window{ INIT_WIDTH, INIT_HEIGHT, std::move(name) },
          _renderer{ _window, name, version, ENGINE_NAME, ENGINE_VERSION },
          _appName{ std::move(name) },
          _appVersion{ version },
          _isRunning{ false }
    {}


    bool TempestApp::init() noexcept
    {
        log::init();

        if (!_window.initWindow())
        {
            log::error("There was an error initializing the window!\n");
            return false;
        }
        const RendererConfig config{ .width                  = static_cast<uint32_t>(_window.getWidth()),
                                     .height                 = static_cast<uint32_t>(_window.getHeight()),
                                     .presentMode            = renderer::PresentMode::VSYNC,
                                     .enableValidationLayers = true };
        if (!_renderer.init(config))
        {
            log::error("There was an error initializing the renderer");
            return false;
        }

        loadModel(MODEL_PATH);
        log::info("Model successfully loaded");
        log::info(std::to_string(_vertices.size()));
        log::info(std::to_string(_indices.size()));
        if (!_renderer.createVertexBuffer(_vertices))
        {
            log::error("There was an error creating the vertex buffer");
            return false;
        }
        log::info("creating vertex buffer success");

        if (!_renderer.createIndexBuffer(_indices))
        {
            log::error("There was an error creating the index buffer");
            return false;
        }

        _isRunning = true;
        return true;
    }

    void TempestApp::run() noexcept
    {
        log::info("App is running...");
        while (_isRunning)
        {
            handleEvents();
            // Update
            // Draw
        }
    }

    void TempestApp::handleEvents() noexcept
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    _isRunning = false;
                    break;
                default:
                    break;
            }
        }
    }


    void TempestApp::loadModel(const std::string_view modelPath)
    {

        // Load the object file
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::unordered_map<Vertex, uint32_t> uniqueVertices;

        std::string err;
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, modelPath.data()))
        {
            log::error(err);
            return;
        }

        // Iterate through its shapes and add it to our vertices and indices
        for (const auto& [name, mesh] : shapes)
        {
            for (const auto& index : mesh.indices)
            {
                Vertex vertex{};
                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2],
                };
                // Flip texture coordinates to Top To Bottom
                vertex.texCoord = { attrib.texcoords[2 * index.texcoord_index + 0],
                                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };
                vertex.color    = { 1.0f, 1.0f, 1.0f };

                auto [it, inserted] = uniqueVertices.insert({ vertex, static_cast<uint32_t>(_vertices.size()) });
                if (inserted)
                {
                    _vertices.push_back(vertex);
                }
                _indices.push_back(it->second);
            }
        }
    }
} // namespace tempest