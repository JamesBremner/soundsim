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
    pg.move({5, 20, 180, 360});
    pg.labelWidth(120);
    pg.string("Duration (msecs)", "2");
    bnSim.move(50, 380, 100, 30);
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
    theSim.readParameterFile("params.txt");

    theSim.maxTime(
        atof(pg.value("Duration (msecs)").c_str()) / 1000.);

    theSim.init();
    theSim.source();
    while (!theSim.isFullTime())
    {
        std::cout << theSim.text();
        theSim.step();
    }

    plResults.text(theSim.text());
    plResults.update();
}