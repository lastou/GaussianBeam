#include <iostream>
#include <complex>
#include <cmath>
#define M_PI 3.14159265358979323846

class Beam
{
public:
	double wavelength_; // (nm)
	double ref_pos_; // the position(mm) at which q-parameter is evaluated
	std::complex<double> q_; // q-parameter(mm)

	Beam(double wavelength, double waist)
		: wavelength_(wavelength), ref_pos_(0), q_(std::complex<double>(0, get_rayleigh_range(waist)))
	{}

	Beam(double wavelength, double waist, double pos)
		: wavelength_(wavelength), ref_pos_(pos), q_(std::complex<double>(0, get_rayleigh_range(waist)))
	{}

	double get_rayleigh_range(double waist) const // (mm)
	{
		return pow(waist, 2) * M_PI / wavelength_;
	}

	double get_waist() const // (um)
	{
		return std::sqrt(wavelength_ * q_.imag() / M_PI);
	}

	double get_waist_pos() const // (mm)
	{
		return ref_pos_ - q_.real();
	}
};

class Lens
{
public:
	double f_; // (mm)
	double pos_; // (mm)

	Lens(double f, double pos)
		: f_(f), pos_(pos)
	{}

	Beam& transform(Beam& beam) const
	{
		beam.q_ = beam.q_ + (pos_ - beam.ref_pos_);
		beam.ref_pos_ = pos_;
		beam.q_ = 1.0 / (1.0 / (beam.q_) - 1 / f_);
		return beam;
	}
};

int main()
{
	Beam beam(1064, 35, 70);
	Lens lens1(-50, 20);
	Lens lens2(100, 70);
	lens1.transform(beam);
	lens2.transform(beam);
	std::cout << beam.get_waist_pos() << std::endl;
}