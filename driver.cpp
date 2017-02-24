#include "include/fparams.h"
#include "include/ArrayND.h"
#include "include/PRISM01.h"
#include "include/PRISM03.h"
#include "include/atom.h"
#include "include/emdSTEM.h"
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <numeric>
#include <string>

using namespace std;
int main(){
    using PRISM_FLOAT_TYPE = double;
    using vec_d = std::vector<PRISM_FLOAT_TYPE>;
    using Array3D = PRISM::ArrayND<3, vec_d>;
    using Array2D = PRISM::ArrayND<2, vec_d>;
    using Array1D = PRISM::ArrayND<1, vec_d>;
    using Array1D_dims = PRISM::ArrayND<1, std::vector<size_t> >;

    std::string filename = "atoms.txt";
    //std::string filename = "test_atom.txt";
    PRISM::emdSTEM<PRISM_FLOAT_TYPE> prism_pars;
    PRISM_FLOAT_TYPE one_pixel_size = 100.0/1000.0;
    prism_pars.potBound = 1.0;
    prism_pars.numFP = 8.0/8.0;
    prism_pars.sliceThickness = 2;
    prism_pars.interpolationFactor = 10;
    Array1D_dims cellDim({100,100,80},{3});
    prism_pars.cellDim = cellDim;

    PRISM_FLOAT_TYPE f = 4*prism_pars.interpolationFactor;
    Array1D_dims imageSize({cellDim[0], cellDim[1]},{2});
    std::transform(imageSize.begin(),imageSize.end(),imageSize.begin(),[&f, &prism_pars, &one_pixel_size](size_t& a){
        return (size_t)(f*round((PRISM_FLOAT_TYPE)a / one_pixel_size / f));
    });
    cout << "imageSize[0] = " << imageSize[0] << endl;
    cout << "imageSize[1] = " << imageSize[1] << endl;
    prism_pars.imageSize = imageSize;

    Array1D pixelSize({(PRISM_FLOAT_TYPE)cellDim[0], (PRISM_FLOAT_TYPE)cellDim[1]},{2});
    prism_pars.pixelSize = pixelSize;
    prism_pars.pixelSize[0]/=prism_pars.imageSize[0];
    prism_pars.pixelSize[1]/=prism_pars.imageSize[1];
   try {
       prism_pars.atoms = PRISM::readAtoms(filename);
   }
   catch(const std::runtime_error& e){
	   cout << "PRISM: Error opening " << filename << endl;
	   cout << e.what();
       cout << "Terminating" << endl;
       return 1;
   }
   catch(const std::domain_error& e){
       cout << "PRISM: Error extracting atomic data from " << filename << "!" << endl;
	   cout << e.what();
       cout << "Terminating" << endl;
	   return 1;
   }

    cout << "prism_pars.pixelSize[0] = " << prism_pars.pixelSize[0] << endl;
    cout << "prism_pars.pixelSize[1] = " << prism_pars.pixelSize[1] << endl;
    cout << "test" << endl;
    cout << "fparams[0]" <<  fparams[0] << endl;
    cout << "fparams[10]" <<  fparams[10] << endl;
    cout << "fparams[20]" <<  fparams[20] << endl;





    Array2D u = PRISM::ones_ND<2, double>({118,1}) * 0.08;
    prism_pars.u = u;
    prism_pars.atoms[0].to_string();
    prism_pars.atoms[prism_pars.atoms.size()-1].to_string();
    PRISM::PRISM01(prism_pars);
    cout << "Writing potential stack to \"potential.mrc\"" << endl;
	prism_pars.pot.toMRC_f("potentials.mrc");
	//for (auto& i : u) std::cout << i << std::endl;
    return 0;
}