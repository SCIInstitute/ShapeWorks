#pragma once

#include "Constraint.h"	


//class PlanarConstraint : public Constraint
//{
//public:
//	typedef Constraint<3>::PointType PointType;
//
//
//	PlanarConstraint(PointType point1, PointType point2, PointType point3) {
//		this->point1 = point1;
//		this->point2 = point2;
//		this->point3 = point3;
//	}
//	~PlanarConstraint();
//
//	void printConstraints() {
//		std::cout << "point 1: " << point1 << endl << "point 2: " << point2 << endl << "point 3: " << point3 << endl;
//	}
//
//	void printConstraintExists() {
//		std::cout << std::endl << " Farshad Constraint Exists! " << endl;
//	}
//	// gneerate the plane normal
//	//typedef itk::FixedArray<T, VDimension> VectorType;
//
//
//	// Apply constraint to satisfy constraint and update the point location 
//	void Apply(PointType& point){
//		// project the point to the constraint plane
//	}
//private:
//	PointType point1, point2, point3;
//	//VectorType planeNormal
//
//
//
//};

//template <unsigned int VDimension = 3>
//class feri : public Farshad<VDimension>
//{
//public:
//	feri();
//	~feri();
//
//	void printFeri() { ; }
//
//};