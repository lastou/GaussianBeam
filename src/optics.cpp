#include <cmath> 
#include <complex>
#include <iostream>
#include <optics.h>
constexpr double kPi = 3.14159265358979323846;

// Beam
Beam::Beam(double wavelength, double waist)
	: wavelength_(wavelength), waist_(waist), waist_pos_(0)
{}
Beam::Beam(double wavelength, double waist, double waist_pos)
	: wavelength_(wavelength), waist_(waist), waist_pos_(waist_pos)
{}
Beam::Beam(Beam& other)
	: wavelength_(other.wavelength_), waist_(other.waist_), waist_pos_(other.waist_pos_)
{}

double Beam::CalWaist(double rayleigh_range) const
{
	return sqrt(wavelength_ * rayleigh_range / kPi);
}

double Beam::rayleigh_range() const
{
	return CalRayleighRange(waist_);
}
double Beam::CalRayleighRange(double waist) const
{
	return pow(waist, 2) * kPi / wavelength_;
}

//double Beam::get_waist() const
//{
//	return std::sqrt(wavelength_ * q_.imag() / kPi);
//}

//double Beam::get_waist_pos() const
//{
//	return ref_pos_ - q_.real();
//}

void Beam::Update(Beam& other)
{
	wavelength_ = other.wavelength_;
	waist_ = other.waist_;
	waist_pos_ = other.waist_pos_;
}

// Lens
Lens::Lens()
	:f_(50), pos_(50)
{}
Lens::Lens(double f, double pos)
	: f_(f), pos_(pos)
{}

Beam& Lens::Transform(Beam& beam) const
{
	std::complex<double> q(pos_ - beam.waist_pos_, beam.rayleigh_range()); // q-parameter(mm) of input beam evaluated at pos_
	q = 1.0 / (1.0 / q - 1 / f_); // q-parameter(mm) of output beam evaluated at pos_

	beam.waist_ = beam.CalWaist(q.imag());
	beam.waist_pos_ = pos_ - q.real();
	return beam;
}

#if 0
int main()
{
	Beam beam(1064, 35, 70);
	Lens lens1(-50, 20);
	Lens lens2(100, 70);
	lens1.transform(beam);
	lens2.transform(beam);
	std::cout << beam.get_waist_pos() << std::endl;
}
#endif