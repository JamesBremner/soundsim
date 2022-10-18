#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"

class cGrid;

/// @brief A location in 3D space
class cNode
{
public:
    int myX, myY, myZ;
    double myPressure;
    double myVx, myVy, myVz;
    double myDensity; //  density in kg/m3
    double mySpeed;   // speed of sound in m/s

    /// @brief Update the values of the node
    /// @param prev pointer to grid of nodes at previous time step

    void updatePressure(cGrid *prev);
    void updateVelocity(cGrid *prev);

    void updateTestStub(cGrid *prev);

    /// @brief text display of node values
    /// @return text
    std::string text();

    /// @brief set node pressure
    /// @param p
    void pressure(double p)
    {
        myPressure = p;
    }
};
/// @brief A 3D grid of locations
class cGrid
{
public:
    std::vector<cNode> myGrid;
    int myNx, myNy, myNz;
    int myTime;

    /// @brief construct grid
    /// @param Nx number of nodes in x direction
    /// @param Ny number of nodes in y direction
    /// @param Nz number of nodes in x direction
    /// @param time time of simulation
    cGrid(
        int Nx, int Ny, int Nz,
        int time);

    /// @brief Impose a time source
    /// @param n Node with surce values

    void source(const cNode &n);

    /// @brief update node values
    /// @param prev pointer to grid of nodes with previous time step values
    void updatePressure(cGrid *prev);
    void updateVelocity(cGrid *prev);

    /// @brief display od node values
    /// @return text

    std::string text();

    /// @brief reference to node at a location
    /// @param x
    /// @param y
    /// @param z
    /// @return node reference
    cNode &node(int x, int y, int z);
};

/// @brief A Finite Difference Time Domain simulator
class cSim
{
public:
    /// @brief Construct simulation of specified size
    /// @param Nx
    /// @param Ny
    /// @param Nz

    cSim(int Nx, int Ny, int Nz);

    /** @brief Initialize the simulation
     * 
     * This should be called after all configuration parameters are set
     * and before the first simulation step
     */
    
    void init();

    /// @brief Impose a source
    void source();

    /// @brief Advance simulatiobn one time step
    void step();

    /// @brief Text display of simulation state
    /// @return text

    std::string text();

    double deltaTime() const
    {
        return myDeltaTime;
    }
    double deltaSpace() const
    {
        return myDeltaSpace;
    }
    double deltaSpaceTimeRatio() const
    {
        return myDeltaTimeSpaceRatio;
    }

    void deltaTime(double t);
    void deltaSpace(double s);

private:
    int myNx, myNy, myNz; // grid resolution ( node count on each axis )
    double myDeltaTime;   // simulation time step
    double myDeltaSpace;  // grid spacing
    double myDeltaTimeSpaceRatio;
    cGrid *myPrevGrid;
    cGrid *myNextGrid;
};

cSim theSim(4, 4, 4);

void cNode::updateVelocity(cGrid *prev)
{
    // Factor accounting for node density and simulation resolution
    double f = theSim.deltaSpaceTimeRatio() / myDensity;

    // Node at this location with previous time step values
    cNode &prevNode = prev->node(myX, myY, myZ);

    // calculate updated velocities ( eq 12 )
    myVx = prevNode.myVx - f * (prev->node(myX + 1, myY, myZ).myPressure - prevNode.myPressure);
    myVy = prevNode.myVy - f * (prev->node(myX, myY + 1, myZ).myPressure - prevNode.myPressure);
    myVz = prevNode.myVz - f * (prev->node(myX, myY, myZ + 1).myPressure - prevNode.myPressure);
}
void cNode::updatePressure(cGrid *prev)
{
    // Factor accounting for node density, sound speed and simulation resolution
    double f = myDensity * mySpeed * mySpeed * theSim.deltaSpaceTimeRatio();

    // Node at this location with previous time step values
    cNode &prevNode = prev->node(myX, myY, myZ);

    // calculate updated pressure ( eq 11 )
    myPressure = prevNode.myPressure

                 - f * (

                           prevNode.myVx - prev->node(myX - 1, myY, myZ).myVx

                           + prevNode.myVy - prev->node(myX, myY - 1, myZ).myVy

                           + prevNode.myVz - prev->node(myX, myY, myZ - 1).myVz);
}
void cNode::updateTestStub(cGrid *prev)
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
    n.myDensity = 1.225;
    n.mySpeed = 340;
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
void cGrid::updatePressure(cGrid *prev)
{
    myTime = prev->myTime + 1;
    for (auto &n : myGrid)
        n.updatePressure(prev);
}
void cGrid::updateVelocity(cGrid *prev)
{
    myTime = prev->myTime + 1;
    for (auto &n : myGrid)
        n.updateVelocity(prev);
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
    : myNx(Nx), myNy(Ny), myNz(Nz),
      myDeltaSpace(1), myDeltaTime(1),
      myDeltaTimeSpaceRatio(myDeltaTime / myDeltaSpace)
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
    myNextGrid->updatePressure(myPrevGrid);
    delete myPrevGrid;
    myPrevGrid = myNextGrid;
    myNextGrid = new cGrid(myNx, myNy, myNz, 0);
    myNextGrid->updateVelocity(myPrevGrid);
}
std::string cSim::text()
{
    std::cout << "\n=======\ntime = " << myNextGrid->myTime << "\n";
    return myNextGrid->text();
}
void cSim::deltaTime(double t)
{
    myDeltaTime = t;
    myDeltaTimeSpaceRatio = myDeltaTime / myDeltaSpace;
}
void cSim::deltaSpace(double s)
{
    if (s < 1e-6)
        throw std::runtime_error(
            "Space resolution too small");
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

    theSim.init();
    theSim.source();
    theSim.step();
    std::cout << theSim.text();
    theSim.step();
    std::cout << theSim.text();

    // cGUI theGUI;
    return 0;
}
