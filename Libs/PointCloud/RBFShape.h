#pragma once

#include <stddef.h>
#include "Shapeworks.h"
//#include "ImageUtils.h"
#include <itkTranslationTransform.h>
#include <Eigen/Core>
#include "evaluators/CPURBFEvaluator.h"
#include "solvers/EigenRBFSolver.h"

#include "kernels/ThinPlateKernel.h"
#include "kernels/GaussianKernel.h"

namespace shapeworks {

class RBFShape
{
public:
      RBFShape(Eigen::Vector4d coeff = Eigen::Vector4d(1.,1.,1.,1.),
                       Eigen::Vector3d pows = Eigen::Vector3d(1.,1.,1.),
                       Eigen::MatrixXd points = Eigen::MatrixXd());

      /** This function is the deconstructor of the class. It does nothing in this base class.
          It is a virtual function and hence it can be redefined in the derived class(s). */
      virtual ~RBFShape();

      /*******************************************************************************************************************/
      /** Access functions: This type of functions are used to provide set/get access to the class member variables. */
      /*******************************************************************************************************************/

      /** This function retrieves the coefficient (x,y,z,value) of the implicit function.
          It is a virtual function and hence it can be redefined in the derived class(s). */
      virtual Eigen::Vector4d coeff(){return coeff_;}

      /** This function sets the ID of the implicit function.
          It is a virtual function and hence it can be redefined in the derived class(s). */
      virtual void setID(size_t id);

      /** This function retrieves the ID of the implicit function.
          It is a virtual function and hence it can be redefined in the derived class(s). */
      virtual size_t getID();

      /** This function sets the name of the implicit function.
          It is a virtual function and hence it can be redefined in the derived class(s). */
      virtual void setName(std::string s){name_ = s;}

      /** This function retrieves the name of the implicit function.
          It is a virtual function and hence it can be redefined in the derived class(s). */
      virtual std::string name(){return name_;}

      /** This function retrieves the vector of 3D points that were used to construct the implicit function.
          It is a virtual function and hence it can be redefined in the derived class(s).*/
      virtual Eigen::MatrixXd getPoints(){return points_;}

      /*******************************************************************************************************************/
      /** Flag functions: This type of functions are used to perform boolean queries for specific characteristics of the class instance. */
      /*******************************************************************************************************************/
      /** This function returns a flag that indicate whether the implicit function has another implicit function that clips it.
          In this base class, the default is retrieved which is false. That is by default an implicit function does not have any clips.
          This default complies with whatever basic operation that this base class defines. Hence, this class doesn't define any operations that allow for clippling.
          It is a virtual function and hence it can be redefined in the derived class(s). */
      virtual bool hasClip(){return false;}

      /** This function returns a flag that indicate whether the implicit function is a fault.
          In this base class, the default is retrieved which is true. That is by default an implicit function is a fault.
          This default complies with whatever basic operation that this base class defines. Hence, this class doesn't define any operations for a horizon being split by a fault.
          It is a virtual function and hence it can be redefined in the derived class(s). */
      virtual bool isFault(){return isFault_;}
      //virtual bool isEmpty();
      virtual bool isClip() { return false; }

      /*******************************************************************************************************************/
      /** Processing functions: This type of functions define the functionality (i.e., the processing blocks) of the class. */
      /*******************************************************************************************************************/

      /** This function evaluates the implicit function value (i.e. scalar) at a given 3D point.
          \param point A 3D spatial point that is defined by xyz coordinates.
          The implicit function \f$f(\mathbf{x})\f$ at the point \f$\mathbf{x}\f$ is evaluated as \f$ f(\mathbf{x}) = \mathbf{c}^T\mathbf{x}^\mathbf{p} + c_v \f$
          where \f$ \mathbf{c} \f$ is the coefficient vector of the implicit function associated with \f$ c_v \f$ as a value
          and  \f$ \mathbf{p} \f$ is the exponent of each spatial coordinate.
          It is a virtual function and hence it can be redefined in the derived class(s). */
      virtual double evaluate(const Eigen::Vector3d & point);

      /** This function evaluates the gradient vector of the implicit function at a given 3D point.
          \param point A 3D spatial point that is defined by xyz coordinates.
          The gradient of the implicit function \f$f(\mathbf{x})\f$ at the point \f$\mathbf{x}\f$ is evaluated as
          \f$ \frac{\partial f(\mathbf{x})}{\partial \mathbf{x}} = \mathbf{p} \times \mathbf{x}^{\mathbf{p}-1} \times \mathbf{c} \f$
          where \f$ \mathbf{c} \f$ is the coefficient vector of the implicit function associated with \f$ c_v \f$ as a value
          and  \f$ \mathbf{p} \f$ is the exponent of each spatial coordinate. The operations here are element-wise.
          It is a virtual function and hence it can be redefined in the derived class(s). */
      virtual Eigen::Vector3d gradient(const Eigen::Vector3d &point);

      /** This function solves the RBF system*/

      virtual void solve_system(const Eigen::MatrixXd & points);

      /** This function applies a transformation matrix (4x4) to the implicit function.
          \param mtx A 4x4 transfromation matrix to be applied to the implicit function.
          This operation updates the coefficient (coeff_) and the points (points_) member variables based on the input transformation matrix.
          It is a virtual function and hence it can be redefined in the derived class(s). */
      //virtual void applyMatrix(std::array<std::array<double, 4>, 4> mtx);

      /** This function returns a pointer to the implicit function that clips this implicit function.
          Since this is a base class for implicit functions, it doesn't provided any clippping operations and hence it returns a null pointer.
          It is a virtual function and hence it can be redefined in the derived class(s). */

  	  bool getFault() const { return isFault_; }
      void setFault(bool isFault) { isFault_ = isFault; }

      bool isEmpty() const { return isEmpty_; }
      void setEmpty(bool isEmpty) { isEmpty_ = isEmpty; }
      std::string getName() const { return name_; }

      RBFShape* func_;

      /** A flag to indicate whether the implicit function is empty to allow empty horizon compartments to be added to the tree. */
      bool isEmpty_; // to allow empty horizon compartments to be added to the tree

      virtual std::string ToDebugString();

  protected:

      /*******************************************************************************************************************/
      /** Protected member variables that are only accesible by derived classes. */
      /*******************************************************************************************************************/

      /** The coefficient (x,y,z,value) that defines the implicit function. This is the linear part of the rbf-based implicit. pows_ > 1 is used to define it as a polynomial with a degree > 1.
          This coefficient defines the background interpolation function of the implicit function, i.e. trends in the function to be interpolated that can not be captured by the rbfs. */
      Eigen::Vector4d              coeff_;

      /** The exponent (power) of each spatial coordinate (x, y, and z) that defines the mathematical form of the basis function.
          That is, each point will contribute to the definition of the basis function by (x^pows_.x, y^pows_.y, z^pows_.z)
          This member variable doesnot have get/set access functions, it is set once using the class constructor with default ones.
          pows_ > 1 is used to define the background interpolation function as a polynomial with a degree > 1. */
      Eigen::Vector3d              pows_;

      /** Identifier of the implicit function. This can be used by derived classes to refer to certain implicited using their identifiers.
          It can also be used to link different parts of the same "horizon" that has been divided by a fault. Constructor default is 0. */
      size_t            id_;

      /** Material type of the implicit function. This might be used to link this implicit with a specific material that might affect subsequent steps such as meshing and visualization.
          Constructor default is 0. */
      size_t            material_;

      /** The name of the implicit function, derived from the input EV file. The name can refer to a horizon or a fault or a part of a horizon when a fault divides it.*/
      std::string       name_;

      /** A vector of 3D points (defined using xyz spatial coordinates) that construct the implicit function. Constructor default is empty vector.
          In the RBFImplicit derived class, rbf point values are the tps coefficients used to construct the rbf.
          These values are initialized by zeros (on surface) and +/- offsetScale/rbfOffset (for dipole points, if we move more cells off the surface, the initial rbf point coefficient is decreased). */
      Eigen::MatrixXd points_;

      RBFKernel * kernel;

      bool isFault_;

      std::vector<double> PointSolutions;
      std::vector<double> TPSWeights;
      std::vector<double> CSRBFWeights;
};

} // shapeworks
