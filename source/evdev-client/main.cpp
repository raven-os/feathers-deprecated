#include "EvdevClient.hpp"

int main()
{
    EvdevClient evdevC;

    if (evdevC.initClient() && evdevC.isValid())
    {
        std::cout << "EvdevClient initialized." << std::endl;
    }
    else
    {
        std::cout << "could not initialize EvdevClient." << std::endl;
        return (1);
    }
    std::cout << evdevC.loop() << std::endl;
    return (0);
}
