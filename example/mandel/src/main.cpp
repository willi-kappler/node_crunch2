#include <argparse/argparse.hpp>

/*
    Node Crunch2
    Written by Willi Kappler, MIT License
    https://github.com/willi-kappler/node_crunch2

    This file includes all the test cases.

    Run with:
    normal: xmake run mandel_nc
    debug:  xmake run -d mandel_nc
*/

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

    if (program["--server"] == true) {
        std::cout << "Server mode" << std::endl;
    } else {
        std::cout << "Node (client) mode" << std::endl;
    }
}
