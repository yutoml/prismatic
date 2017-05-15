// Copyright Alan (AJ) Pryor, Jr. 2017
// Transcribed from MATLAB code by Colin Ophus
// PRISM is distributed under the GNU General Public License (GPL)
// If you use PRISM, we ask that you cite the following papers:

#include "utility.h"
#include <complex>
#include "defines.h"
#include "configure.h"
namespace PRISM {
	std::pair<PRISM::Array2D<std::complex<PRISM_FLOAT_PRECISION> >, PRISM::Array2D<std::complex<PRISM_FLOAT_PRECISION> > >
	upsamplePRISMProbe(PRISM::Array2D<std::complex<PRISM_FLOAT_PRECISION> > probe,
	                   const long dimj, const long dimi, long ys, long xs) {
		Array2D<std::complex<PRISM_FLOAT_PRECISION> > realspace_probe;
		Array2D<std::complex<PRISM_FLOAT_PRECISION> > buffer_probe;
		Array2D<std::complex<PRISM_FLOAT_PRECISION> > kspace_probe;

		buffer_probe = zeros_ND<2, std::complex<PRISM_FLOAT_PRECISION> >({{(size_t)dimj, (size_t)dimi}});
//		std::cout << "dimj = " << dimj << std::endl;
		long ncy = probe.get_dimj() / 2;
		long ncx = probe.get_dimi() / 2;
		for (auto j = 0; j < probe.get_dimj(); ++j) {
			for (auto i = 0; i < probe.get_dimi(); ++i) {
				buffer_probe.at( (dimj + ((j - ncy + ys) % dimj)) % dimj,
				                 (dimi + ((i - ncx + xs) % dimi)) % dimi) = probe.at(j, i);
//				std::cout << "(dimj + ((j - ncy) % dimj)) % dimj= " << (dimj + ((j - ncy) % dimj)) % dimj<< std::endl;
//				std::cout << "(j - ncy)= " << (j - ncy) << std::endl;
//				std::cout << "(j - ncy) % dimj)= " << (j - ncy) % dimj<< std::endl;

//				buffer_probe.at( (dimj + ((j - ncy) % dimj)) % dimj,
//				                 (dimi + ((i - ncx) % dimi)) % dimi) = probe.at(j, i);
			}
		}
		std::unique_lock<std::mutex> gatekeeper(fftw_plan_lock);
		PRISM_FFTW_PLAN plan = PRISM_FFTW_PLAN_DFT_2D(buffer_probe.get_dimj(), buffer_probe.get_dimi(),
		                                              reinterpret_cast<PRISM_FFTW_COMPLEX *>(&buffer_probe[0]),
		                                              reinterpret_cast<PRISM_FFTW_COMPLEX *>(&buffer_probe[0]),
		                                              FFTW_FORWARD, FFTW_ESTIMATE);
		gatekeeper.unlock();
		realspace_probe = buffer_probe;
		PRISM_FFTW_EXECUTE(plan);
		kspace_probe = buffer_probe;
		gatekeeper.lock();
		PRISM_FFTW_DESTROY_PLAN(plan);
		gatekeeper.unlock();
		return std::make_pair(realspace_probe, kspace_probe);
	}

	PRISM_FLOAT_PRECISION computePearsonCorrelation(PRISM::Array2D<std::complex<PRISM_FLOAT_PRECISION> > left,
	                                                PRISM::Array2D<std::complex<PRISM_FLOAT_PRECISION> > right){
		PRISM_FLOAT_PRECISION m1, m2, sigma1, sigma2, R;
		m1=m2=sigma1=sigma2=R=0;

		for (auto &i:left) m1+=abs(i);
		for (auto &i:right)m2+=abs(i);

		m1 /= left.size();
		m2 /= right.size();

		for (auto &i:left)sigma1  += pow(std::abs(std::abs(i)-m1),2);
		for (auto &i:right)sigma2 += pow(std::abs(std::abs(i)-m2),2);

		sigma1 /= left.size();
		sigma2 /= right.size();

		sigma1 = sqrt(sigma1);
		sigma2 = sqrt(sigma2);

		for (auto i = 0; i < std::min(left.size(), right.size()); ++i){
			R += (std::abs(left[i]) - m1) * (std::abs(right[i]) - m2);
		}
		return R / (sigma1 * sigma2);
	}
	PRISM_FLOAT_PRECISION computeRfactor(PRISM::Array2D<std::complex<PRISM_FLOAT_PRECISION> > left,
	                                     PRISM::Array2D<std::complex<PRISM_FLOAT_PRECISION> > right){
		PRISM_FLOAT_PRECISION accum, diffs;
		accum = diffs = 0;
		for (auto i = 0; i < std::min(left.size(), right.size()); ++i){
			diffs += std::abs(left[i] - right[i]);
			accum += std::abs(left[i]);
		}
		return diffs / accum;
	}

}