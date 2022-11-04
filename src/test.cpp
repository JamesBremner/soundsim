#include <iostream>
#include "soundsim.h"

cSim theSim;

main()
{
    theSim.deltaSpace(0.1);
    theSim.deltaTime(0.002);
    theSim.maxTime(0.006);

    theSim.init();
    theSim.source();
    while (!theSim.isFullTime())
    {
        theSim.step();
    }

    std::string expected =
       "0 -1.1e+07 1.09e+08 -5.27e+08 1.11e+09 -5.27e+08 1.09e+08 -1.1e+07 4.57e+05        0";
    std::cout << theSim.text(1);
    if( theSim.text(1).find(expected) == -1 )
        std::cout << "test FAILED !!!\n";
    else
        std::cout << "test OK\n";
}