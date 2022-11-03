#include <string>
#include "soundsim.h"
#include "cGUI.h"

cGUI::cGUI()
    : cStarterGUI(
          "SoundSim",
          {50, 50, 1000, 500}),
      plParams(wex::maker::make<wex::panel>(fm)),
      pg(wex::maker::make<wex::propertyGrid>(plParams)),
      plResults(wex::maker::make<wex::panel>(fm)),
      bnSim(wex::maker::make<wex::button>(plParams))
{
    plParams.move(0, 0, 200, 400);
    pg.move({10, 50, 180, 200});
    pg.labelWidth(120);
    pg.string("Space delta ( cm )","10");
    pg.string("Time delta (ms)","1");
    pg.string("Duration (ms)", "10");
    pg.string("Display Z (cm)", "10");
    bnSim.move(50, 260, 100, 30);
    bnSim.text("SIMULATE");
    bnSim.events().click(
        [this]
        {
            simulate();
        });

    plResults.move(200, 0, 900, 400);
    plResults.fontName("courier");

    show();
    run();
}

void cGUI::simulate()
{
    //theSim.readParameterFile("params.txt");

    theSim.deltaSpace(
        atof(pg.value("Space delta ( cm )").c_str())/100.);
    theSim.deltaTime(
        atof(pg.value("Time delta (ms)").c_str())/1000.);
    theSim.maxTime(
        atof(pg.value("Duration (ms)").c_str()) / 1000.);
    int zreq = atoi(pg.value("Display Z (cm)").c_str());
    if( 0 > zreq || zreq > 100 )
        {
            wex::msgbox("Bad Display Z value.  Accepts 0 to 100");
            return;
        }
    int z = zreq / ( 100 * theSim.deltaSpace());

    theSim.init();
    theSim.source();
    while (!theSim.isFullTime())
    {
        std::cout << theSim.text(z);
        theSim.step();
    }

    plResults.text(theSim.text(z));
    plResults.update();
}