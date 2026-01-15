/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes some utils for the mandel example.
*/

// External includes:
#include <spdlog/spdlog.h>

// Internal includes:
#include "util.hpp"

MandelData::MandelData():
    re1(0.0),
    re2(0.0),
    im1(0.0),
    im2(0.0),
    width(800),
    height(600)
    {}

MandelData::MandelData(std::vector<uint8_t> data):
    re1(0.0),
    re2(0.0),
    im1(0.0),
    im2(0.0),
    width(800),
    height(600)
{
    spdlog::debug("Constructor with data: {}", data[0]);
}
