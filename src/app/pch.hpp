#pragma once

// STL
#include <string.h>
#include <iostream>
#include <optional>
#include <functional>
#include <variant>
#include <iterator>
#include <vector>
#include <array>
#include <unordered_map>
#include <utility>
#include <thread>
#include <chrono>

// EXTERNAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/fast_square_root.hpp>
#include <glm/gtx/norm.hpp>
#include <imgui/imgui.h>
#include <thread-pool/ThreadPool.hpp>
#include <lua/lua.hpp>
#include <spdlog/spdlog.h>

// APPLICATION
#include <lwvl/lwvl.hpp>
#include "Common.hpp"
