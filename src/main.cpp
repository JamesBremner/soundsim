#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include "soundsim.h"

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
    for (int z = 0; z < myNz; z++)
    {
        ss << "z = " << z << "\n";
        for (int y = 0; y < myNy; y++)
        {
            for (int x = 0; x < myNx; x++)
            {
                ss << node(x, y, z).text() + " ";
            }
            ss << "\n";
        }
    }
    return ss.str();
}

void cGrid::binary(std::ofstream &of)
{
    for (int z = 0; z < myNz; z++)
    {
        for (int y = 0; y < myNy; y++)
        {
            for (int x = 0; x < myNx; x++)
            {
                of << node(x, y, z).myPressure;
            }
        }
    }
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
void cSim::binary()
{
    std::ofstream ofs(myPressureFilename);
    if (!ofs.is_open())
        throw std::runtime_error(
            "Cannot write " + myPressureFilename);

    ofs << myNx << myNy << myNz;
    ofs << 0.0 << 0.0 << 0.0;
    ofs << myNx * myDeltaSpace
        << myNy * myDeltaSpace
        << myNz * myDeltaSpace;
    myNextGrid->binary(ofs);
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
    myDeltaSpace = s;

    // simulate a 1m by 1m by 1m space
    myNx = 1 / s;
    myNy = 1 / s;
    myNz = 1 / s;
}
void cSim::readParameterFile(const std::string &fname)
{
    /*
delta
delta_t
max_t
sampling_rate
source_type
input_speed_filename
input_density_filename
output_pressure_base_filename
output_velocity_x_base_filename
output_velocity_y_base_filename
output_velocuty_z_base_filename
    */
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot read " + fname);
    std::string line;
    getline(ifs, line);
    deltaSpace(atof(line.c_str()));
    getline(ifs, line);
    deltaTime(atof(line.c_str()));
    getline(ifs, line);
    maxTime(atof(line.c_str()));
    getline(ifs, line);
    getline(ifs, line);
    getline(ifs, line);
    getline(ifs, line);
    getline(ifs, line);
    myPressureFilename = line;
}

bool cSim::isFullTime()
{
    if( ! myPrevGrid )
        return false;
    return myPrevGrid->myTime >= myMaxTime;
}
main(int argc, char *argv[])
{
    if (argc < 2)
        throw std::runtime_error("Missing parameter file name");

    theSim.readParameterFile(argv[1]);

    theSim.init();
    theSim.source();
    while (!theSim.isFullTime())
    {
        theSim.step();
        std::cout << theSim.text();
    }

    return 0;
}
