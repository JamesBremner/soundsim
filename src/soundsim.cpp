#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

#include "soundsim.h"

void cNode::updateVelocity(cGrid *prev)
{
    // Factor accounting for node density and simulation resolution
    double f = theSim.deltaSpaceTimeRatio() / myDensity;

    // Node at this location with previous time step values
    cNode &prevNode = prev->node(myX, myY, myZ);

    /* update velocity 
    The velocity is a function of the previous velocity at this location
    and the pressures at the nodes on each side of this location

    Since a velocity node is assumed to be located one half the location delta 
    away from the pressure nod with the same index ( k ) in the +ve direction
    then the indices of the pressure locations on each side are k and k+1

    Pk            Pk+1
      <<<<< Vk >>>>>
    */
    myVx = prevNode.myVx - f * (prev->node(myX + 1, myY, myZ).myPressure - prevNode.myPressure);
    myVy = prevNode.myVy - f * (prev->node(myX, myY + 1, myZ).myPressure - prevNode.myPressure);
    myVz = prevNode.myVz - f * (prev->node(myX, myY, myZ + 1).myPressure - prevNode.myPressure);

    // std::cout << myX <<" " << myY <<" "<< myZ
    //     <<" vel: "<< myVx <<" "<< myVy << "\n";
}
void cNode::updatePressure(cGrid *prev)
{
    // Factor accounting for node density, sound speed and simulation resolution
    double f = myDensity * mySpeed * mySpeed * theSim.deltaSpaceTimeRatio();

    // Node at this location with previous time step values
    cNode &prevNode = prev->node(myX, myY, myZ);

    /* update pressure 
    The pressure is a function of the previous pressure at this location
    and the velocities at the nodes on each side of this location

    Since a pressure node is assumed to be located one half the location delta 
    away from the velocity node with the same index ( k ) in the -ve direction
    then the indices of the pressure locations on each side are k-1 and k

    Vk-1            Vk
      <<<<< Pk >>>>>
    */
    myPressure = prevNode.myPressure

                 - f * (

                           prevNode.myVx - prev->node(myX - 1, myY, myZ).myVx

                           + prevNode.myVy - prev->node(myX, myY - 1, myZ).myVy

                           + prevNode.myVz - prev->node(myX, myY, myZ - 1).myVz);

    // std::cout << myX <<" " << myY <<" "<< myZ
    //     <<" pressure: "<< myPressure << "\n";
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
    std::stringstream ss;
    ss << std::setw(8) << std::setprecision(3) 
        << myPressure;
    return ss.str();
}

cGrid::cGrid(int Nx, int Ny, int Nz, int time)
    : myNx(Nx), myNy(Ny), myNz(Nz), myTimeStep(time)
{
    cNode n;
    n.myPressure = 0;
    n.myVx = 0;
    n.myVy = 0;
    n.myVz = 0;
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
    timeStep(*prev);
    for (auto &n : myGrid)
        n.updatePressure(prev);
}
void cGrid::updateVelocity(cGrid *prev)
{
    timeStep(*prev);
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
    if (x > myNx - 1)
        x = myNx - 1;
    if (y < 0)
        y = 0;
    if (y > myNy - 1)
        y = myNy - 1;
    if (z < 0)
        z = 0;
    if (z > myNz - 1)
        z = myNz - 1;
    int index = x + y * myNx + z * myNx * myNy;
    return myGrid[index];
}
std::string cGrid::text()
{
    std::stringstream ss;
    for (int z = 0; z < myNz; z++)
    {
        if (z != 1)
            continue;
        ss << "Pressure\nz = " << z << "\n";
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
    s.myX = 4;
    s.myY = 4;
    s.myZ = 1;
    s.myPressure = 0.1;
    myNextGrid->source(s);
}
void cSim::step()
{
    delete myPrevGrid;
    myPrevGrid = myNextGrid;
    myNextGrid = new cGrid(myNx, myNy, myNz, 0);
    myNextGrid->updateVelocity(myPrevGrid);
    delete myPrevGrid;
    myPrevGrid = myNextGrid;
    myNextGrid = new cGrid(myNx, myNy, myNz, 0);
    myNextGrid->updatePressure(myPrevGrid);
}
std::string cSim::text()
{
    std::stringstream ss;
    ss << "time = " 
        << 1000 * myNextGrid->time() * myDeltaTime 
        << " msecs\n";
    std::cout << ss.str();
    ss << myNextGrid->text();
    return ss.str();
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
    myDeltaTimeSpaceRatio = myDeltaTime / myDeltaSpace;

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
    if (!myPrevGrid)
        return false;
    return myPrevGrid->time() * myDeltaTime >= myMaxTime;
}