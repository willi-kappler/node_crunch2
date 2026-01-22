/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes the main function for the mandel example.

    Run with:
    normal: xmake run mandel_nc
    debug:  xmake run -d mandel_nc

    To just build use:
    xmake build
*/

// External includes:
#include <argparse/argparse.hpp>

// Local includes:
#include "nc_config.hpp"

#include "mandel_node.hpp"
#include "mandel_server.hpp"

int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("test");

    program.add_argument("--server")
        .help("increase output verbosity")
        .flag();

    try {
        program.parse_args(argc, argv);
    }
        catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    NCConfiguration config("config1.json");

    if (program["--server"] == true) {
        std::cout << "Server mode" << std::endl;
        MandelData mandel_data;
        std::shared_ptr<MandelServerProcessor> mandel_server = std::make_shared<MandelServerProcessor>(mandel_data);
        NCServer nc_server(config, mandel_server);
    } else {
        std::cout << "Node (client) mode" << std::endl;
        std::shared_ptr<MandelNodeProcessor> mandel_node = std::make_shared<MandelNodeProcessor>();
        NCNode nc_node(config, mandel_node);
    }
}
