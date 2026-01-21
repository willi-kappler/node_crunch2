/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes some utils for the mandel example.
*/

// STD includes:
#include <stdfloat>
#include <cstdint>
#include <vector>
#include <memory>

// External includes:
#include <spdlog/fmt/bundled/format.h>

const uint8_t FLOAT_SIZE = 8;
const uint8_t UINT32_SIZE = 4;
const uint32_t MANDEL_DATA_SIZE = (4 * FLOAT_SIZE) + (3 * UINT32_SIZE);

class MandelData {
    public:
        MandelData();
        MandelData(std::vector<uint8_t> data);

        std::vector<uint8_t> to_vector();

        std::float64_t re1, re2, im1, im2, re_step, im_step;
        uint32_t width, height, max_iteration;
};

// For spdlog:
// Specialization of fmt::formatter for MandelData
template <>
struct fmt::formatter<MandelData> {
    // Parses format specs like {:x}; we'll just support the default {}
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    // Format the real mandel data:
    template <typename FormatContext>
    auto format(const MandelData& d, FormatContext& ctx) const -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(),
            "[MandelData: Re({:.2f}, {:.2f}), Im({:.2f}, {:.2f}), Size({}x{}), Iter({})]",
            d.re1, d.re2, d.im1, d.im2, d.width, d.height, d.max_iteration);
    }
};

void vec_u8_to_vec_u32(std::vector<uint8_t> vec_in, std::vector<uint32_t> vec_out);

void vec_u32_to_vec_u8(std::vector<uint32_t> vec_in, std::vector<uint8_t> vec_out);
