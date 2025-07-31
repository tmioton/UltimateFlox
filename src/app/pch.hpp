#pragma once

// STL
#include <malloc.h>
#include <cstdlib>
#include <exception>
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
#include <bitset>

// EXTERNAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <lwvl/lwvl.hpp>
#include <glm/vec2.hpp>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/fast_square_root.hpp>
#include <glm/gtx/norm.hpp>
#include <lua/lua.hpp>
#include <thread-pool/ThreadPool.hpp>

// APPLICATION
#include "Common.hpp"
