#include "application.hpp"

#include "core/log.hpp"
#include "core/window.hpp"
#include "core/editor.hpp"

#include "raytracer/raytracer.hpp"

#include "app/camera.hpp"
#include "app/viewport.hpp"
#include "app/render_settings.hpp"

#include <chrono>

namespace app
{
    void application::run()
    {
        core::window window{ 1920, 1080, "Engine" };
        core::editor editor;
        rAI::raytracer raytracer{ 1085, 1026 };
        rAI::scene scene;

        {
            rAI::sphere sphere_ground{ glm::vec3{ 0.0f, -96.5f, -1.0f }, 100.0f, rAI::material{ glm::vec3{ 0.7f, 0.4f, 0.9f }, glm::vec3{ 0.0f }, 0.0f } };

            rAI::sphere sphere_a = rAI::sphere{ glm::vec3{ 0.0f, 2.0f, -1.2f }, 0.5f, rAI::material{ glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f }, 0.0f } };
            rAI::sphere sphere_b = rAI::sphere{ glm::vec3{ -1.0f, 2.0f, -1.0f }, 0.5f, rAI::material{ glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec3{ 0.0f }, 0.0f } };
            rAI::sphere sphere_c = rAI::sphere{ glm::vec3{ 1.0f, 2.0f, -1.0f }, 0.5f, rAI::material{ glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 0.0f }, 0.0f } };

            std::vector<rAI::sphere> spheres;

            spheres.push_back(sphere_ground);
            spheres.push_back(sphere_a);
            spheres.push_back(sphere_b);
            spheres.push_back(sphere_c);

            rAI::upload_scene(scene, spheres);
        }

        camera camera{ 0.1f, 100.0f, 45.f, 1085.f / 1026.f };
        
        editor.add_widget<viewport>(raytracer.get_render_texture());

        const auto clock = std::chrono::high_resolution_clock{};
        auto last_time = clock.now();

        render_settings_data render_settings_data;
        render_settings_data.max_bounces = 10;
        render_settings_data.rays_per_pixel = 100;
        render_settings_data.sky_box.is_hidden = false;
        render_settings_data.sky_box.horizon_color = glm::vec3(1.f, 1.f, 1.f),
        render_settings_data.sky_box.zenith_color = glm::vec3(0.36f, 0.58f, 0.8f),
        render_settings_data.sky_box.ground_color = glm::vec3(0.4f, 0.4f, 0.4f);
        render_settings_data.sky_box.sun_direction = glm::normalize(glm::vec3(1.0f, 1.0f, -1.0f));
        render_settings_data.sky_box.sun_intensity = 50.0f;
        render_settings_data.sky_box.sun_focus = 514.0f;

        editor.add_widget<render_settings>(render_settings_data);

        while (window.is_open())
        {
            using seconds = std::chrono::duration<float, std::ratio<1>>;
        
            float delta_time = std::chrono::duration_cast<seconds>(clock.now() - last_time).count();
            last_time = clock.now();

            window.update();

            camera.update(delta_time);

            rAI::rendering_context rendering_context
            { 
                camera.get_position(), 
                camera.get_inverse_view_matrix(), 
                camera.get_inverse_projection_matrix(),
                render_settings_data.max_bounces,
                render_settings_data.rays_per_pixel,
                render_settings_data.sky_box,  
            };
            
            raytracer.render(rendering_context, scene);
            
            editor.render();
        }
    }
}

