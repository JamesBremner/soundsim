#include <iostream>
#include "soundsim.h"

cSim theSim;

main()
{
    theSim.config.deltaSpace_cm(10);
    theSim.config.deltaTime_millisecs(2);
    theSim.config.maxTime_millisecs(6);
    theSim.config.sourceLocation_cm( 40,40,10 );

    theSim.init();
    theSim.source();
    while (!theSim.isFullTime())
    {
        theSim.step();
    }

    std::string expected =
       "0  9.9e+03 -1.2e+05  4.3e+05        0  4.3e+05 -1.2e+05  9.9e+03        0        0";
    std::cout << theSim.text(1);
    if( theSim.text(1).find(expected) == -1 )
        std::cout << "test FAILED !!!\n";
    else
        std::cout << "test OK\n";
}