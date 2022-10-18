#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"

class cGrid;

class cNode
{
public:
    int myX, myY, myZ;
    double myPressure;
    double myVx, myVy, myVz;

    void update(cGrid *prev);
    std::string text();
    void pressure(double p)
    {
        myPressure = p;
    }
};
class cGrid
{
public:
    std::vector<cNode> myGrid;
    int myNx, myNy, myNz;
    int myTime;
    cGrid(
        int Nx, int Ny, int Nz,
        int time);
    void source(const cNode &n);
    void update(cGrid *prev);
    std::string text();

    cNode &node(int x, int y, int z);
};
class cSim
{
public:
    cSim(int Nx, int Ny, int Nz);
    void init();
    void source();
    void step();
    std::string text();

private:
    int myNx, myNy, myNz;
    cGrid *myPrevGrid;
    cGrid *myNextGrid;
};

void cNode::update(cGrid *prev)
{
    // calculate new node values
    // a simple stub for testing the infrastructure
    // if the node at x=1 as finite pressure
    // then this node will have 90% of that pressure at the next time increment

    myPressure =
        0.9 * prev->node(myX - 1, myY, myZ).myPressure;
}
std::string cNode::text()
{
    return std::to_string(myPressure);
}

cGrid::cGrid(int Nx, int Ny, int Nz, int time)
    : myNx(Nx), myNy(Ny), myNz(Nz), myTime(time)
{
    cNode n;
    n.myPressure = 0;
    n.myVx = -1;
    n.myVy = -1;
    n.myVz = -1;
    myGrid.resize(myNx * myNy * myNz, n);
    for (int x = 0; x < myNx; x++)
    {
        for (int y = 0; y < myNy; y++)
        {
            for (int z = 0; z < myNz; z++)
            {
                n.myX = x;
                n.myY = y;
                n.myZ = z;
                node(x, y, z) = n;
            }
        }
    }
}
void cGrid::update(cGrid *prev)
{
    myTime = prev->myTime + 1;
    for (auto &n : myGrid)
        n.update(prev);
}
void cGrid::source(const cNode &n)
{
    auto &nx = node(n.myX, n.myY, n.myZ);
    nx = n;
}
cNode &cGrid::node(int x, int y, int z)
{
    if (x < 0)
        x = 0;
    if (x > myNx)
        x = myNx;
    if (y < 0)
        y = 0;
    if (y > myNy)
        y = myNy;
    if (z < 0)
        z = 0;
    if (z > myNz)
        z = myNz;
    int index = x + y * myNx + z * myNx * myNy;
    return myGrid[index];
}
std::string cGrid::text()
{
    std::stringstream ss;
    for (int x = 0; x < myNx; x++)
    {
        ss << "x = " << x << "\n";
        for (int y = 0; y < myNy; y++)
        {
            for (int z = 0; z < myNz; z++)
            {
                ss << node(x, y, z).text() + " ";
            }
            ss << "\n";
        }
    }
    return ss.str();
}
cSim::cSim(int Nx, int Ny, int Nz)
    : myNx(Nx), myNy(Ny), myNz(Nz)
{
}
void cSim::init()
{
    myPrevGrid = 0;
    myNextGrid = new cGrid(myNx, myNy, myNz, 0);
}
void cSim::source()
{
    cNode s;
    s.myX = 1;
    s.myY = 1;
    s.myZ = 1;
    s.myPressure = 10;
    myNextGrid->source(s);
}
void cSim::step()
{
    delete myPrevGrid;

    myPrevGrid = myNextGrid;
    myNextGrid = new cGrid(myNx, myNy, myNz, 0);
    myNextGrid->update(myPrevGrid);
}
std::string cSim::text()
{
    std::cout << "\n=======\ntime = " << myNextGrid->myTime << "\n";
    return myNextGrid->text();
}

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Starter",
              {50, 50, 1000, 500}),
          lb(wex::maker::make<wex::label>(fm))
    {
        lb.move(50, 50, 100, 30);
        lb.text("Hello World");

        show();
        run();
    }

private:
    wex::label &lb;
};

main()
{
    cSim S(4, 4, 4);
    S.init();
    S.source();
    S.step();
    std::cout << S.text();
    S.step();
    std::cout << S.text();

    // cGUI theGUI;
    return 0;
}
