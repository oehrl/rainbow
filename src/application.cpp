#include "rainbow/application.hpp"

#include <chrono>
#include <iostream>

#ifdef RAINBOW_BACKEND_CPU
#include "rainbow/backends/cpu/cpu_backend.hpp"
#endif
#ifdef RAINBOW_BACKEND_OPENGL
#include "rainbow/backends/opengl/opengl_backend.hpp"
#endif
#ifdef RAINBOW_BACKEND_METAL
#include "rainbow/backends/metal/metal_backend.hpp"
#endif
#ifdef RAINBOW_BACKEND_VULKAN
#include "rainbow/backends/vulkan/vulkan_backend.hpp"
#endif
#include "rainbow/parallel.hpp"
#include "rainbow/timing.hpp"

namespace rainbow {

Application::Application() : viewport_{512, 512} {
  if (SDL_CreateWindowAndRenderer(viewport_.GetWidth(), viewport_.GetHeight(),
                                  SDL_WINDOW_OPENGL, &window_,
                                  &renderer_) < 0) {
    std::cerr << "Failed to create window!" << std::endl;
    throw std::runtime_error("Failed to create SDL window");
  }

  texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888,
                               SDL_TEXTUREACCESS_STREAMING,
                               viewport_.GetWidth(), viewport_.GetHeight());
  if (!texture_) {
    std::cerr << "Failed to create viewport texture!" << std::endl;
    throw std::runtime_error("Failed to create viewport texture!");
  }

#if RAINBOW_BACKEND_VULKAN
  rendering_backend_ = std::make_unique<VulkanBackend>();
#elif RAINBOW_BACKEND_METAL
  rendering_backend_ = MakeMetalBackend();
#elif RAINBOW_BACKEND_OPENGL
  rendering_backend_ = std::make_unique<OpenGLBackend>(window_);
#elif RAINBOW_BACKEND_CPU
  rendering_backend_ = std::make_unique<CPUBackend>();
#else
#error No rendering backend available
#endif
}

Application::~Application() {
  rendering_backend_.reset();
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

bool Application::LoadScene(const std::string& filename) {
  if (scene_.Load(filename)) {
    rendering_backend_->Prepare(scene_, viewport_.GetWidth(),
                                viewport_.GetHeight());
    RenderPreview();
    return true;
  } else {
    std::string error_string = "Failed to load scene: `" + filename + "`";
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Loading failed",
                             error_string.c_str(), window_);
    std::cerr << error_string << std::endl;
    return false;
  }
}

void Application::Run() {
  SDL_Event event;
  while (!quit_application_) {
    if (interactive_mode_) {
      while (SDL_PollEvent(&event)) {
        ProcessEvent(event);
      }
      const auto current_time = std::chrono::steady_clock::now();
      Update(current_time - last_update_);
      last_update_ = current_time;

    } else {
      if (SDL_WaitEvent(&event)) {
        ProcessEvent(event);
      }
    }

    if (redraw_preview_) {
      RenderPreview();
      redraw_preview_ = false;
    }
  }
}

void Application::ProcessEvent(const SDL_Event& event) {
  switch (event.type) {
    case SDL_QUIT:
      quit_application_ = true;
      break;

    case SDL_MOUSEBUTTONDOWN:
      if (event.button.button == SDL_BUTTON_LEFT) {
        EnterInteractiveMode();
      }
      break;

    case SDL_MOUSEBUTTONUP:
      if (event.button.button == SDL_BUTTON_LEFT) {
        LeaveInteractiveMode();
      }
      break;

    case SDL_MOUSEMOTION:
      if (interactive_mode_) {
        camera_.Rotate(event.motion.xrel * 0.01f, -event.motion.yrel * 0.01f);
        redraw_preview_ = true;
      }
      break;
  }
}

void Application::RenderPreview() {
  RAINBOW_TIME_FUNCTION();
  viewport_.Clear(Vector4::Zero());
  rendering_backend_->Render(camera_, &viewport_);

  uint8_t* pixels;
  int pitch;
  RAINBOW_TIME_SECTION("Write texture data") {
    if (SDL_LockTexture(texture_, nullptr, reinterpret_cast<void**>(&pixels),
                        &pitch) == 0) {
      ParallelFor(viewport_.GetHeight(), [pitch, pixels, this](auto y) {
        for (size_t x = 0; x < viewport_.GetWidth(); ++x) {
          const auto pixel_color = viewport_.GetPixel(x, y);
          for (auto i : IntegralRange{4}) {
            float value = pixel_color[3 - i];
            if (value < 0.0f) {
              value = 0.0f;
            } else if (value > 1.0f) {
              value = 1.0f;
            }
            pixels[y * pitch + x * 4 + i] = value * 255;
          }
        }
      });

      SDL_UnlockTexture(texture_);
    } else {
      throw std::runtime_error("Failed to lock texture");
    }
  };
  SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
  SDL_RenderPresent(renderer_);
}  // namespace rainbow

void Application::EnterInteractiveMode() {
  interactive_mode_ = true;
  last_update_ = std::chrono::steady_clock::now();
}

void Application::Update(std::chrono::duration<float> elapsed_time) {
  auto move_vector = Vector3::Zero();
  const Uint8* keys = SDL_GetKeyboardState(nullptr);

  Vector3 right;
  Vector3 up;
  Vector3 forward;
  camera_.GetAxisVectors(&right, &up, &forward);

  if (keys[SDL_SCANCODE_W]) {
    move_vector += forward;
  }
  if (keys[SDL_SCANCODE_S]) {
    move_vector -= forward;
  }
  if (keys[SDL_SCANCODE_D]) {
    move_vector += right;
  }
  if (keys[SDL_SCANCODE_A]) {
    move_vector -= right;
  }
  if (keys[SDL_SCANCODE_E]) {
    move_vector += up;
  }
  if (keys[SDL_SCANCODE_Q]) {
    move_vector -= up;
  }

  float length = Length(move_vector);
  if (length > std::numeric_limits<float>::epsilon()) {
    camera_.Move(move_vector * (elapsed_time.count() / length));
    redraw_preview_ = true;
  }
}

void Application::LeaveInteractiveMode() { interactive_mode_ = false; }

}  // namespace rainbow
