#include <boost/test/unit_test.hpp>
#include "ArrayND.h"
#include <iostream>
#include <vector>
#include "go.h"
#include "meta.h"
#include "params.h"
#include <stdio.h>
#include <random>
#include "fileIO.h"
#include "H5Cpp.h"
#include "fftw3.h"
#include "ioTests.h"

namespace Prismatic{

class basicSim{

    public:
    basicSim()     {setupSim(),BOOST_TEST_MESSAGE( "Setting up fixture");}
    ~basicSim()    {BOOST_TEST_MESSAGE( "Tearing down fixture");}
    Metadata<PRISMATIC_FLOAT_PRECISION> meta;
    Parameters<PRISMATIC_FLOAT_PRECISION> pars;
    std::string logPath = "ioTests.log";
    int fd;
    fpos_t pos;

    void setupSim()
    {
        //running from build directory
        meta.filenameAtoms = "../SI100.XYZ";
        meta.filenameOutput = "../test/fileIOtests.h5";
        meta.includeThermalEffects = 0;
        meta.save2DOutput = true;
        meta.save3DOutput = true;
        meta.save4DOutput = true;
        meta.saveDPC_CoM  = true;
        meta.savePotentialSlices = true;
        pars = Parameters<PRISMATIC_FLOAT_PRECISION>(meta);
    }
    

};

class logFile{
    public:
    logFile()       {setupLog(), BOOST_TEST_MESSAGE("Setting up matrixRefocus.log file.");}
    ~logFile()      {BOOST_TEST_MESSAGE("Releasing matrixRefocus.log file.");}
    std::string logPath;

    void setupLog()
    {
        logPath = "matrixRefocus.log";
        FILE *fp = fopen(logPath.c_str(),"w");
        fprintf(fp,"####### BEGIN TEST SUITE: matrixRefocus #######\n");
        fclose(fp);
    }
};

void divertOutput(fpos_t &pos, int &fd, const std::string &file);
void revertOutput(const int &fd, fpos_t &pos);
void removeFile(const std::string &filepath);


BOOST_GLOBAL_FIXTURE(logFile);

BOOST_AUTO_TEST_SUITE(refocusTests);

BOOST_FIXTURE_TEST_CASE(matrixRefocus, basicSim)
{   
    /*
    Test case is designed as follows.
    Matrix refocusing is only relevant for the PRISM method; i.e, there is no error to correct for in multislice
    Therefore, ideally we can compare to a multislice simulation.

    tile the Z dimension for the SI example cell 10-20 times, so it is decently thick and refocusing matters

    In multislice, we choose three defocii - C1 = -100, 0, 100 and simulate the complex output wave 4D output
    apply a backpropagation operator to each image so that probes can compared on equivalent bases

    In PRISM, simulate the same defocii with f=1 refocusing the matrix (and using the C1 abberation in the probe formation)

    Compare values of complex output probes and ensure equality within tolerance

    Could be expanded to f > 1 but would need carefulc ropping and downsampling for proper checking


    */

    std::string fname_m = "../test/matrixRefocus_m.h5";
    std::string fname_p = "../test/matrixRefocus_p.h5";
    meta.tileZ = 10;
    meta.filenameOutput = fname_m;
    meta.probeDefocus = 0.0;
    meta.matrixRefocus = true;
    meta.numGPUs = 1;
    meta.algorithm = Algorithm::Multislice;
    meta.numThreads = 1;
    meta.saveComplexOutputWave = true;
    meta.savePotentialSlices = true;
    meta.matrixRefocus = false;
    divertOutput(pos, fd, logPath);
    std::cout << "\n####### BEGIN TEST CASE: refocus_test #########\n";
    go(meta);

    std::cout << "\n--------------------------------------------\n";
    meta.algorithm = Algorithm::PRISM;

    meta.interpolationFactorX = 1;
    meta.interpolationFactorY = 1;
    meta.filenameOutput = fname_p;
    meta.importPotential = false; //import potential to ensure that image size is the same
    meta.importPath = "4DSTEM_simulation/data/realslices/ppotential_fp0000/realslice";
    meta.importFile = fname_m;
    go(meta);
    std::cout << "######### END TEST CASE: refocus_test #########\n";
    revertOutput(fd, pos);


    //read and compare probes    
    std::string dataPath4D = "4DSTEM_simulation/data/datacubes/CBED_array_depth0000/datacube";
    Array4D<std::complex<PRISMATIC_FLOAT_PRECISION>> refProbes;
    Array4D<std::complex<PRISMATIC_FLOAT_PRECISION>> testProbes;
    readComplexDataSet(refProbes, fname_m, dataPath4D);
    readComplexDataSet(testProbes, fname_p, dataPath4D);

    PRISMATIC_FLOAT_PRECISION tol = 0.001;
    BOOST_TEST(compareSize(refProbes, testProbes));
    if(compareSize(refProbes, testProbes))
    {
        std::cout << compareValues(refProbes, testProbes) << std::endl;
        std::cout << compareValues(refProbes, testProbes) / refProbes.size() << std::endl;
        BOOST_TEST(compareValues(refProbes, testProbes) < tol);
    }
    else
    {
        std::array<size_t, 4> refDims = refProbes.get_dimarr();
        std::array<size_t, 4> testDims = testProbes.get_dimarr();
        for(auto i = 0; i < 4; i++)
        {
            std::cout << refDims[i] << " " << testDims[i] << std::endl;
        }
    }


    // removeFile(fname_m);
    // removeFile(fname_p);
}

BOOST_AUTO_TEST_SUITE_END();
}