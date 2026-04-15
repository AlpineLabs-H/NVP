#include <iostream>
#include <cstring>
#include <cstdlib>

// forward declarations


bool isWSL() {
    return std::getenv("WSL_DISTRO_NAME") != nullptr;
}

void printHelp() {
    std::cout << "nvp - Neovim Package Manager\n\n";
    std::cout << "Commands:\n";
    std::cout << "  nvp init              Login and set up Neovim\n";
    std::cout << "  nvp install <name>    Install a package\n";
    std::cout << "  nvp search <name>     Search for a package\n";
    std::cout << "  nvp publish           Publish a package\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 0;
    }

    if (strcmp(argv[1], "init") == 0) {
        cmd_init();
    } else if (strcmp(argv[1], "install") == 0) {
        if (argc < 3) {
            std::cout << "Usage: nvp install <package>\n";
            return 1;
        }
        cmd_install(argv[2]);
    } else if (strcmp(argv[1], "search") == 0) {
        if (argc < 3) {
            std::cout << "Usage: nvp search <query>\n";
            return 1;
        }
        cmd_search(argv[2]);
    } else if (strcmp(argv[1], "publish") == 0) {
        cmd_publish();
    } else {
        std::cout << "Unknown command: " << argv[1] << "\n";
        printHelp();
        return 1;
    }

    return 0;
}