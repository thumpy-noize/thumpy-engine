

#include <iostream>
#include <input_manager.hpp>

bool DEBUG_MODE = true;

bool APLICATION_RUNNING = true;

int main()
{
    std::cout << "Starting Engine..." << std::endl;

    int ticks = 10000000;

    Thumpy::Core::IO::init();

    while (APLICATION_RUNNING) {
        
        Thumpy::Core::IO::poll_input();

        ticks--;
        if (ticks == 0) {
            APLICATION_RUNNING = false;
        }
    }

    std::cout << "Shuting Down." << std::endl;

    return 0;
}
