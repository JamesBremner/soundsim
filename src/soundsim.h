#include <vector>
#include <string>

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

    /// @brief construct grid
    /// @param Nx number of nodes in x direction
    /// @param Ny number of nodes in y direction
    /// @param Nz number of nodes in x direction
    /// @param time time of simulation
    cGrid(
        int Nx, int Ny, int Nz,
        int time);

    /// @brief Impose a time source
    /// @param n Node with source values

    void source(const cNode &n);

    /// @brief update node values
    /// @param prev pointer to grid of nodes with previous time step values
    void updatePressure(cGrid *prev);
    void updateVelocity(cGrid *prev);

    /// @brief display of node values
    /// @return text

    std::string text( int z );

    void binary(std::ofstream &of);

    /* @brief reference to node at a location
    /// @param x
    /// @param y
    /// @param z
    /// @return node reference

    * If the requested location is outside the grid
    * return reference to nearest node inside grid
    */
    cNode &node(int x, int y, int z);

    int time() const
    {
        return myTimeStep / 2;
    }
    void timeStep(const cGrid &prev)
    {
        myTimeStep = prev.timeStep() + 1;
    }
    int timeStep() const
    {
        return myTimeStep;
    }

private:
    int myTimeStep;
};

/// @brief A Finite Difference Time Domain simulator
class cSim
{
public:

    /// @brief Construct simulation with default config
    cSim();

    void readParameterFile(const std::string &fname);

    /** @brief Initialize the simulation
     *
     * This should be called after all configuration parameters are set
     * and before the first simulation step
     */

    void init();

    /// @brief Impose a source
    void source();

    /// @brief Advance simulation one time step
    void step();

    /// @brief human readable display of results
    /// @return text
    std::string text( int z );

    /// @brief Check for simulation completed
    /// @return true if requested duration reached
    bool isFullTime();

    double deltaSpaceTimeRatio() const
    {
        return myDeltaTimeSpaceRatio;
    }
    double deltaSpace() const
    {
        return myDeltaSpace;
    }

    void deltaTime(double t);
    void deltaSpace(double s);

    /// @brief set simulation duration
    /// @param max seconds
    void maxTime(double max)
    {
        myMaxTime = max;
    }

private:
    int myNx, myNy, myNz; // grid resolution ( node count on each axis )
    double myDeltaTime;   // simulation time step
    double myMaxTime;
    double myDeltaSpace; // grid spacing
    double myDeltaTimeSpaceRatio;
    cGrid *myPrevGrid;
    cGrid *myNextGrid;

    std::string myPressureFilename;

        void binary();

    double deltaTime() const
    {
        return myDeltaTime;
    }

    double maxTime() const
    {
        return myMaxTime;
    }
};

extern cSim theSim;
