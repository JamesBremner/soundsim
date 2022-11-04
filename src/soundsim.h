#include <vector>
#include <string>
#include <stdexcept>

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

    /// @brief Update the pressure at the node
    /// @param v velocity grid
    void updatePressure(cGrid &v);

    /// @brief Update velocity
    /// @param p presure grid
    void updateVelocity(cGrid &p);

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

    cGrid();

    /// @param Nx number of nodes in x direction
    /// @param Ny number of nodes in y direction
    /// @param Nz number of nodes in x direction
    /// @param time time of simulation
    void resize(int Nx, int Ny, int Nz, int time);

    /// @brief Impose a time source
    /// @param n Node with source values

    void source(const cNode &n);

    /// update node values
    void updatePressure(cGrid &v);
    void updateVelocity(cGrid &p);

    /// @brief display of node values
    /// @return text

    std::string text(int z);

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

/**
 * @brief Parameters
 *
 * This stores simulation parametes
 * looking after conversions between
 * user units ( milliseconds and centimeters )
 * MKS ubits ( seconds and meters )
 * grid units ( integers )
 */
class cConfig
{
public:
    void deltaSpace_cm(double s)
    {
        if (s < 1e-3)
            throw std::runtime_error(
                "Space resolution too great");
        myDeltaSpacem = s / 100.0;
    }
    double deltaSpace_m() const
    {
        return myDeltaSpacem;
    }
    void deltaTime_millisecs(double t)
    {
        if (t < 0.1)
            throw std::runtime_error(
                "delta time must be greater then 0.1 millisecond");
        myDeltaTimesecs = t / 1000.0;
    }
    double deltaTime_secs() const
    {
        return myDeltaTimesecs;
    }
    double deltaTime_millisecs() const
    {
        return myDeltaTimesecs * 1000;
    }
    double deltaSpaceTimeRatio() const
    {
        return myDeltaSpacem / myDeltaTimesecs;
    }
    void maxTime_millisecs(double t)
    {
        myMaxTimesecs = t / 1000.0;
    }
    double maxTime_secs() const
    {
        return myMaxTimesecs;
    }
    void sourceLocation_cm(double x, double y, double z)
    {
        mySrcXcm = x;
        mySrcYcm = y;
        mySrcZcm = z;
    }

    void sourceLocation_grid(int &x, int &y, int &z) const
    {
        x = mySrcXcm / (100 * myDeltaSpacem);
        y = mySrcYcm / (100 * myDeltaSpacem);
        z = mySrcZcm / (100 * myDeltaSpacem);
    }

private:
    double myDeltaSpacem;   // grid spacing
    double myDeltaTimesecs; // simulation time step
    double myDeltaSpaceTimeRatio;
    double myMaxTimesecs;
    double mySrcXcm;
    double mySrcYcm;
    double mySrcZcm;
};

/// @brief A Finite Difference Time Domain simulator
class cSim
{
public:
    cConfig config;

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
    std::string text(int z);

    /// @brief Check for simulation completed
    /// @return true if requested duration reached
    bool isFullTime();

private:
    int myNx, myNy, myNz; // grid resolution ( node count on each axis )

    cGrid myPressureGrid;
    cGrid myVelocityGrid;

    std::string myPressureFilename;

    void binary();
};

extern cSim theSim;
