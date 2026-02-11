/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes the main function for the mandel example.

    To just build use:
    xmake build mandel_nc

    Run with:
    ./run_mandel
*/

// External includes:
#include <argparse/argparse.hpp>

// Local includes:
#include "nc_config.hpp"
#include "nc_logger.hpp"

#include "mandel_node.hpp"
#include "mandel_server.hpp"

int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("mandel");

    program.add_argument("--server")
        .help("Set server mode")
        .flag();

    program.add_argument("--ip")
        .default_value(std::string(""))
        .help("Set the ip address for the server");

    try {
        program.parse_args(argc, argv);
    }
        catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    NCConfiguration config = nc_config_from_file("config1.json");

    if (program["--server"] == true) {
        std::cout << "Server mode" << std::endl;

        nc_server_logger();
        mandel_server_logger();

        MandelData mandel_data;
        std::shared_ptr<MandelServerProcessor> mandel_server = std::make_shared<MandelServerProcessor>(mandel_data);
        NCServer nc_server(config, mandel_server);
        nc_server.nc_run();
    } else {
        std::cout << "Node (client) mode" << std::endl;

        nc_node_logger();
        mandel_node_logger();

        if (program.is_used("--ip")) {
            std::string server_address = program.get<std::string>("--ip");
            config.server_address = server_address;
        }

        std::shared_ptr<MandelNodeProcessor> mandel_node = std::make_shared<MandelNodeProcessor>();
        NCNode nc_node(config, mandel_node);
        nc_node.nc_run();
    }
}
