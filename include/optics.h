#pragma once
#include <complex>

class Beam
{
public:
	double wavelength_; // (nm)
	//double ref_pos_; // the position(mm) at which q-parameter is evaluated
	double waist_; // (um)
	double waist_pos_; // (mm)

	Beam(double wavelength, double waist);
	Beam(double wavelength, double waist, double waist_pos);
	Beam(Beam& other);

	//double get_waist() const; // (um)
	//double get_waist_pos() const; // (mm)

	double CalWaist(double rayleigh_range) const; // (um)
	double rayleigh_range() const; // (mm)
	double CalRayleighRange(double waist) const; // (mm)

	//void WavelengthChanged();
	//void WaistChanged();
	//void WaistPosChanged();


	void Update(Beam& other);
};

class Lens
{
public:
	double f_; // (mm)
	double pos_; // (mm)

	Lens();
	Lens(double f, double pos);

	Beam& Transform(Beam& beam) const;
};