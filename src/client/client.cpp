#include "Game.hpp"

int main(int argc, char** argv) {
    try {
        Game game{"127.0.0.1", 12345};
        game.run();
        return EXIT_SUCCESS;
    }
    catch (std::exception& e) {
        std::cerr << "Caught exception in main(): " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Caught unknown exception in main()\n";
        return EXIT_FAILURE;
    }
}