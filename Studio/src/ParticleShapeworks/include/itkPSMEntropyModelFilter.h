/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPSMEntropyModelFilter_h
#define __itkPSMEntropyModelFilter_h

#include "itkInPlaceImageFilter.h"
#include "itkPSMParticleSystem.h"
#include "itkPSMImplicitSurfaceDomain.h"
#include "itkPSMShapeEntropyFunction.h"
#include "itkPSMParticleEntropyFunction.h"
#include "itkPSMGradientDescentOptimizer.h"
#include "itkPSMContainerArrayAttribute.h"
//#include "itkPSMMeanCurvatureAttribute.h"
#include "itkPSMSurfaceNeighborhood.h"
#include "itkPSMTwoCostFunction.h"
#include "itkCommand.h"

namespace itk
{
  
/** \class PSMEntropyModelFilter
 *
 * \brief
 * 
 * This the most basic of all PSM model optimization filters.  This
 * filter assembles all of the necessary components to perform an
 * entropy-based correspondence optimization for constructing a
 * point-based shape model of shape surfaces.
 *
 * This class combines a PSMShapeEntropyFunction with a
 * PSMParticleEntropyFunction within a PSMTwoCostFunction to produce
 * an optimization of particle positions with respect to both their
 * positional entropy on shape surfaces and their entropy within a
 * higher-dimensional shape space (the entropy of the distributions of
 * shapes).  The relative weighting of these two terms may be adjusted
 * using the SetShapeEntropyWeighting method, which increases or
 * decreases the influence of the entropy of the shape model on the
 * optimization.  The default value for ShapeEntropyWeighting is 1.0.
 *
 *
 * NEED CITATION HERE
 * 
 * HOW DO YOU USE THIS FILTER?  First you need to specify the input
 * images.  Input images are in the form of distance transforms that
 * contain an implicit surface representation at the zero level set
 * within the image. The zero level set is the surface on which the
 * particles are constrained during the optimization.  You will
 * specify multiple input images, one for each shape in your shape
 * cohort.  To specify the inputs, use the method SetInput(i,image *),
 * where i is the number of the input and image * is an
 * itk::Image::Pointer to the distance transform image.
 *
 * This filter can be run in "initialization mode", which causes all
 * data structures to allocate and initialize, but does not run the
 * actual optimization.  To run in initialization mode, set the
 * Initializing flag to true.
 *
 * \ingroup PSM 
 * \ingroup PSMModelingFilters
 * \author Josh Cates
 */
template <class TImage, class TShapeMatrix = PSMShapeMatrixAttribute<double, TImage::ImageDimension> >
class ITK_EXPORT PSMEntropyModelFilter
  : public InPlaceImageFilter<TImage,TImage> 
{
 public:
  /** Standard class typedefs. */
  typedef PSMEntropyModelFilter  Self;
  typedef InPlaceImageFilter<TImage,TImage>  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, TImage::ImageDimension);
  
  /** Type of the particle system */
  typedef PSMParticleSystem<Dimension> ParticleSystemType;
  
  /** The type of the Shape Matrix, which defines optimization behavior. */
  typedef TShapeMatrix ShapeMatrixType;
  typedef typename ShapeMatrixType::Pointer ShapeMatrixPointerType;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMEntropyModelFilter, InPlaceImageFilter);
  
  /** Type of the input/output image. */
  typedef TImage ImageType;
  
  /** Expose the point type */
  typedef typename ImageType::PointType PointType;
  
  /** Type of the optimizer */
  typedef PSMGradientDescentOptimizer<typename ImageType::PixelType, Dimension> OptimizerType;

  /** Convenient type for cutting plane info */
  struct CuttingPlaneType
  {
    vnl_vector_fixed<double,Dimension> x;
    vnl_vector_fixed<double,Dimension> y;
    vnl_vector_fixed<double,Dimension> z;
    bool valid;

    CuttingPlaneType()
    { valid = false; }
    
  };
  
  /**
   * Override parent classes to expand input list on new inputs.  This
   * filter requires multiple input images.  Each input to this filter
   * represents a shape sample. Note that there may be hundreds of
   * inputs to this filter!
   */
  void SetInput(const std::string& s, itk::DataObject *o)
    {
      Superclass::SetInput(s,o);
    }
  void SetInput(const TImage *image)
  { 
    this->SetInput(0, image);  
  }  
  void SetInput( unsigned int index, const TImage * image ) 
    {
      if (this->GetNumberOfInputs() < index+1)
        {
          this->SetNumberOfRequiredInputs(index+1);
        }
      
      this->ProcessObject::SetNthInput(index, const_cast< TImage *>( image ) );
    }
  
  /** The number of scales for the optimization.  Starting with the
      input correspondence points for Scale 0, the algorithm will run a
      full optimization with the parameters given for that scale
      (RegularizationInitial, RegularizationFinal, etc.).  After
      optimization at each scale, the particle system size will be
      doubled by splitting each particle and optimization will proceed
      at the next scale. If no correspondence points are supplied on
      the inputs, then the system at Scale 0 will be initialized with a
      single correspondence point, placed at the surface position
      closest to the upper-left-hand corner of each of the input
      images.  
      
      If NumberOfScales = 1, then the filter will simply
      optimize the positions of the input correspondence points.  Note
      that you must supply parameters separately for each scale of the
      optimization. */
  void SetNumberOfScales(unsigned int n)
    {
      m_NumberOfScales = n;
      if (m_Tolerances.size() < n)
        {
          m_Tolerances.resize(n);
        }
      if (m_MaximumNumberOfIterations.size() < n)
        {
          m_MaximumNumberOfIterations.resize(n);
        }
      if (m_RegularizationInitial.size() < n)
        {
          m_RegularizationInitial.resize(n);
        }
      if (m_RegularizationFinal.size() < n)
        {
          m_RegularizationFinal.resize(n);
        }
      if (m_RegularizationDecaySpan.size() < n)
        {
          m_RegularizationDecaySpan.resize(n);
        }

    }
  itkGetMacro(NumberOfScales, unsigned int);
  itkSetMacro(CurrentScale, unsigned int);
  itkGetMacro(CurrentScale, unsigned int);
  
  /** This is a placeholder for a more formalized implementation of a
      point-based shape model. */
  //  void SetInputModel(const ParticleModel &model);
  
  /** Set the Nth list of correspondences for the input model.  These
      are the initial correspondence point positions for the Nth input
      surface (see SetInput). */
  void SetInputCorrespondencePoints(unsigned int index, const std::vector<PointType> &corr)
    {
      if (m_InputCorrespondencePoints.size() < (index+1))
        {
          m_InputCorrespondencePoints.resize(index+1);
        }
      m_InputCorrespondencePoints[index] = corr;
    }
  
  /** Returns the particle system used in the shape model computation. */
  itkGetObjectMacro(ParticleSystem, ParticleSystemType);
  itkGetConstObjectMacro(ParticleSystem, ParticleSystemType);
  
  /** Returns a pointer to the ParticleEntropyFunction. This is the
      cost function that will maximize positional entropy of particle
      distributions on shape surfaces. This is the cost function term
      that ensures good shape representations. */
  PSMParticleEntropyFunction<typename ImageType::PixelType, Dimension>
    *GetParticleEntropyFunction()
  {
    return m_ParticleEntropyFunction;
  }
  
  /** Returns a pointer to the ShapeEntropyFunction.  This is the cost
      function that will minimize the entropy of the shape vectors in
      high-dimensional shape space.  This is the cost function term
      that establishes correspondence across shapes. */
  PSMShapeEntropyFunction<Dimension> *ShapeEntropyFunction()
  {
    return m_ShapeEntropyFunction;
  }
  
  /** Return a pointer to the optimizer object. */
  itkGetObjectMacro(Optimizer, OptimizerType);
  itkGetConstObjectMacro(Optimizer, OptimizerType);
  
  /** Set / Get the maximum number of iterations that the solver is
      allowed to execute.  If set to 0, this parameter is ignored and
      the solver will iterate until any other convergence criteria are
      met. A maximum number of iterations must be specified for each
      scale (second, optional parameter if NumberOfScales > 1).*/
  void SetMaximumNumberOfIterations(const std::vector<unsigned int> &n)
    {
      m_MaximumNumberOfIterations = n;
    }
  void SetMaximumNumberOfIterations(unsigned int n, unsigned int i = 0)
    {
      // Resize the tolerances list if necessary
      if (m_MaximumNumberOfIterations.size() < i+1)
        {
          itkExceptionMacro("Specified scale does not exist.  Call SetNumberOfScales before SetMaximumNumberOfIterations");
        }
      m_MaximumNumberOfIterations[i] = n;
      
      if (m_CurrentScale == i)
        {
          m_Optimizer->SetMaximumNumberOfIterations(n);
        }
      
    }
  unsigned int GetMaximumNumberOfIterations(unsigned int i = 0) const
  {
    if (m_MaximumNumberOfIterations.size() < i+1)
      {
        itkExceptionMacro("Specified scale does not exist.");
      }
    return m_MaximumNumberOfIterations[i];
  }
  
  /** Return the number of elapsed iterations of the solver.  Note
      that this number is reset to zero with each call to
      GenerateData. */
  unsigned int GetNumberOfElapsedIterations() const
  {
    return m_Optimizer->GetNumberOfIterations();
  }

   /** The Initialized flag indicates whether GenerateData has
      previously been called and all the necessary temporary data
      structures have been allocated. */
  itkGetMacro(Initialized, bool);

  /** Get/Set the Initializing flag.  When set to true, this flag will
      prevent GenerateData from starting the optimization.  When
      m_Initializing is true, GenerateData will allocated all data
      structures and set necessary parameters, but return before
      running the optimization. */
  itkGetMacro(Initializing, bool);
  itkSetMacro(Initializing, bool);

  /** This method sets/gets the relative weighting of terms in the two-term
      cost function used for the optimization.  This parameter adjusts
      the weighting of the shape representation term with respect to
      the strength of the entropy minimization in shape space.  Higher
      values will favor a more compact representation in shape space
      and produce less regular samplings of shape surfaces. Default
      value is an equal weighting between the two terms (1.0). */
  void SetShapeEntropyWeighting(double w) 
  {
    m_CostFunction->SetRelativeGradientScaling(w);
  }
  double GetShapeEntropyWeighting() const
  {
    return m_CostFunction->GetRelativeGradientScaling();
  }

 /* /\** This method simply calls the appropriate method in */
 /*     PSMParticleEntropyFunction.  It is exposed here for */
 /*     convenience. The minimum radius of the neighborhood of points */
 /*     that are considered in the entropy calculation. The neighborhood */
 /*     is a spherical radius in 3D space. The actual radius used in a */
 /*     calculation may exceed this value, but will not exceed the */
 /*     MaximumNeighborhoodRadius. This parameter should ALWAYS be set by */
 /*     an application, because it must be scaled according to the */
 /*     spacing in the image.  A good value is typically 5x the spacing */
 /*     of the highest-resolution dimension (the dimension with the */
 /*     smallest spacing. *\/ */
 /*  void SetMinimumNeighborhoodRadius( double s) */
 /*  { this->m_ParticleEntropyFunction->SetMinimumNeighborhoodRadius(s); } */
 /*  double GetMinimumNeighborhoodRadius() const */
 /*  { return this->m_ParticleEntropyFunction->GetMinimumNeighborhoodRadius(); } */

 /*  /\** Maximum radius of the neighborhood of points that are considered */
 /*      in the calculation. The neighborhood is a spherical radius in 3D */
 /*      space. MaximumNeighborhoodRadius should be set to a value */
 /*      equal-to or less-than the length of the largest dimension of the */
 /*      image.  This parameter should ALWAYS be set by an application, */
 /*      since this class cannot know by default the size of input */
 /*      images. The radius value should be specified in real-world */
 /*      coordinates. *\/ */
 /*  void SetMaximumNeighborhoodRadius( double s) */
 /*  {  this->m_ParticleEntropyFunction->SetMaximumNeighborhoodRadius(s); } */
 /*  double GetMaximumNeighborhoodRadius() const */
 /*  { return  this->m_ParticleEntropyFunction->GetMaximumNeighborhoodRadius(); } */

  /** Returns a vector of the variances associated with the PCA
      decomposition of the shape space.  There will be only
      number_of_samples - 1 total valid PCA modes. This method simply
      calls the method of the same name in PSMShapeEntropyFunction. It
      is included here for convenience. */
   const std::vector<double> &GetShapePCAVariances() const
   { 
     return m_ShapeEntropyFunction->GetShapePCAVariances(); 
   }
  
/** A regularization constant is added to the diagonal elements of the
      covariance matrix of the particle positions in shape space
      during the optimization.  It is called "minimum variance" in the
      PSMShapeEntropyFunction class because it enforces a minimum
      variance in each of the eigen-modes of the shape space during
      the optimization. Higher MinimumVariance produces larger
      gradients, thus allowing shapes to move larger distances in
      shape space during optimization of positions with this function.
      Higher MinimumVariance values minimize the effects of smaller
      modes of variation in the covariance matrix.  Conversely,
      smaller MinimumVariance values increase the significance of the
      smaller modes of variation on the gradient calculation.  The
      parameters RegularizationInitial, RegularizationFinal, and
      RegularizationDecaySpan define an "annealing" type optimization,
      where the minimum variance starts at RegularizationInitial and
      exponentially decreases over RegularizationDecaySpan to the
      RegularizationFinal value.  If the RegularizationDecaySpan is
      set to 0, then only RegularizationConstant is used as the
      MinimumVariance parameter and is held fixed during the
      optimization.*/
   void SetRegularizationInitial(const std::vector<double> &v)
     {
       if (v.size() != m_RegularizationInitial.size()) 
         {
           itkExceptionMacro("Number of variables does not match number of scales.  Call SetNumberOfScales first.");
         }
       m_RegularizationInitial = v;
     }
   void SetRegularizationInitial(double t, unsigned int i=0)
     {
       // Resize the tolerances list if necessary
       if (m_RegularizationInitial.size() < i+1)
         {
           itkExceptionMacro("Specified scale does not exist. Call SetNumberOfScales before SetTolerance");
         }
       m_RegularizationInitial[i] = t;
       
       if (m_CurrentScale == i)
         {
           m_Optimizer->SetTolerance(t);
         }       
     }
   double GetRegularizationInitial(unsigned int i=0) const
     {
       if (m_RegularizationInitial.size() < i+1)
         {
           itkExceptionMacro("Specified scale does not exist");
         }
       else return m_RegularizationInitial[i];
     }
   void SetRegularizationFinal(const std::vector<double> &v)
     {
       if (v.size() != m_RegularizationFinal.size()) 
         {
           itkExceptionMacro("Number of variables does not match number of scales.  Call SetNumberOfScales first.");
         }
       m_RegularizationFinal = v;
     }
   void SetRegularizationFinal(double t, unsigned int i=0)
     {
       // Resize the tolerances list if necessary
       if (m_RegularizationFinal.size() < i+1)
         {
           itkExceptionMacro("Specified scale does not exist. Call SetNumberOfScales before SetTolerance");
         }
       m_RegularizationFinal[i] = t;
       
       if (m_CurrentScale == i)
         {
           m_Optimizer->SetTolerance(t);
         }       
     }
   double GetRegularizationFinal(unsigned int i=0) const
   {
     if (m_RegularizationFinal.size() < i+1)
       {
         itkExceptionMacro("Specified scale does not exist");
       }
     else return m_RegularizationFinal[i];
   }
   void SetRegularizationDecaySpan(const std::vector<double> &v)
     {
       if (v.size() != m_RegularizationDecaySpan.size()) 
         {
           itkExceptionMacro("Number of variables does not match number of scales.  Call SetNumberOfScales first.");
         }
       m_RegularizationDecaySpan = v;
     }
   void SetRegularizationDecaySpan(double t, unsigned int i=0)
     {
       // Resize the tolerances list if necessary
       if (m_RegularizationDecaySpan.size() < i+1)
         {
           itkExceptionMacro("Specified scale does not exist. Call SetNumberOfScales before SetTolerance");
         }
       m_RegularizationDecaySpan[i] = t;
       
       if (m_CurrentScale == i)
         {
           m_Optimizer->SetTolerance(t);
         }       
     }
   double GetRegularizationDecaySpan(unsigned int i=0) const
   {
     if (m_RegularizationDecaySpan.size() < i+1)
       {
         itkExceptionMacro("Specified scale does not exist");
       }
     else return m_RegularizationDecaySpan[i];
   }

   /** Directly set the regularization.  This bypasses all other
       methods, but will only be valid until the current scale is
       optimized. */
   void SetRegularizationConstant(double r)
   {
     m_ShapeEntropyFunction->SetMinimumVariance(r);
   }
   void SetMinimumVariance(double r)
   {
     m_ShapeEntropyFunction->SetMinimumVariance(r);
   }
   double GetRegularizationConstant() const
   {
     return m_ShapeEntropyFunction->GetMinimumVariance();
   }
   double GetMinimumVariance() const
   {
     return m_ShapeEntropyFunction->GetMinimumVariance();
   }
   
   /** Set the tolerance for the optimization, below which the
       optimization will be considered to have converged.  Tolerance
       is specified as a distance in the implicit surface images.  The
       solver will converge when no pixel has moved more than the
       specified tolerance in a given iteration. An optional argument
       specifies the scale to which you want this tolerance applied
       (see SetNumberOfScales). If NumberOfScales > 1, then you MUST
       supply a value for each scale. */
   void SetTolerance(const std::vector<double> &v)
     {
       if (v.size() != m_Tolerances.size()) 
         {
           itkExceptionMacro("Number of variables does not match number of scales.  Call SetNumberOfScales first.");
         }
       m_Tolerances = v;
     }
   void SetTolerance(double t, unsigned int i = 0)
     {
       // Resize the tolerances list if necessary
       if (m_Tolerances.size() < i+1)
         {
           itkExceptionMacro("Specified scale does not exist. Call SetNumberOfScales before SetTolerance");
         }
       m_Tolerances[i] = t;
       
       if (m_CurrentScale == i)
         {
           m_Optimizer->SetTolerance(t);
         }
     }
   double GetTolerance(unsigned int i = 0) const
     {
       if (m_Tolerances.size() > i)
         {
           return m_Tolerances[i];
         }
       else 
         {
           itkExceptionMacro("Specified scale does not exist.");
         }
     }
   
   /** Returns shape matrix.  Each column of the shape matrix is a
       different point-based representation of a shape in the cohort.
       There are several types of shape matrices, which may be defined
       differently for subclasses of the PSMEntropyModelFilter
       class. */
   itkGetMacro(ShapeMatrix, ShapeMatrixPointerType);
   const ShapeMatrixType *GetShapeMatrix() const
   {
     return m_ShapeMatrix;
   }
   
   /** Creates a single correspondence point to each domain that is
       placed at the surface position closest to the upper-left-hand
       corner of each of the input images.  */
   bool CreateSingleCorrespondence();

   /** Sets the name of the ith domain. This functionality is only for
       convenience, but required if you want to lookup a domain index
       by its name.*/
   void SetDomainName(const std::string &s, unsigned int i);

   /** Returns the index associated with the given name.  Throws an
       exception if the given name is not found in the domain name
       list. */
   unsigned int GetDomainIndexByName(const std::string &name);

   /** Adds cutting plane information for given domain i.  Currently,
       only one cutting plane per domain is supported.  */
   void AddCuttingPlane(const vnl_vector_fixed<double,3> &x,
			const vnl_vector_fixed<double,3> &y,
			const vnl_vector_fixed<double,3> &z,
			unsigned int domain);
   void AddCuttingPlane(const vnl_vector_fixed<double,3> &x,
			const vnl_vector_fixed<double,3> &y,
			const vnl_vector_fixed<double,3> &z,
			const std::string &s)
   {
     this->AddCuttingPlane(x,y,z,this->GetDomainIndexByName(s));
   }

 protected:
   PSMEntropyModelFilter();
   virtual ~PSMEntropyModelFilter() {};
   
   void PrintSelf(std::ostream& os, Indent indent) const
     {
       Superclass::PrintSelf(os, indent);
     }
   
   /** This method does the work of the optimization. */
   void GenerateData();
   
   /** This method sets up temporary data structures needed for the
       optimization.  It is called during the first run of
       GenerateData. */
   virtual void AllocateDataCaches();
   
   /** This method allocates working images needed during the
       optimization.  It is called during the first run of
       GenerateData. */
   virtual void AllocateWorkingImages();
   
   /** This method allocates data structures necessary to keep track of
       image domains and image neighborhoods.  It is called during the
       first run of GenerateData. */
   virtual void AllocateDomainsAndNeighborhoods();
   
   /** This method copies the correspondence point lists provided by
       SetInputCorrespondencePoints into the initialized particle system.
       It is called during the first run of GenerateData. */
   void InitializeCorrespondences();
   
   /** The Initialized flag indicates whether GenerateData has
      previously been called and all the necessary temporary data
      structures have been allocated. */
   itkSetMacro(Initialized, bool);
   
   /** A callback method that will be called after each iteration of
       the Solver. */
   void OptimizerIterateCallback(Object *, const EventObject &);

   /** Returns a pointer to the domain that is referenced by the given
       name*/
   typename PSMImplicitSurfaceDomain<
     typename ImageType::PixelType, Dimension>::Pointer
     &GetDomainByName(const std::string &name)
   {
     return this->GetDomain(this->GetDomainIndexByName(name));
   }

   /** Returns a pointer to the ith domain in the domain list. */
   typename PSMImplicitSurfaceDomain<
     typename ImageType::PixelType, Dimension>::Pointer
     &GetDomain(unsigned int i)
   {
     if (i >= m_DomainList.size())
       { itkExceptionMacro("Requested domain not available"); }
     return m_DomainList[i];
   }
   
 private:
   PSMEntropyModelFilter(const Self&); //purposely not implemented
   void operator=(const Self&);        //purposely not implemented
   
   std::vector<typename TImage::Pointer> m_WorkingImages;
   typename OptimizerType::Pointer       m_Optimizer;
   
   /** Maximum number of iterations for each of the scales. */
   std::vector<unsigned int> m_MaximumNumberOfIterations;
   
   /** Parameters that control the annealing rate for the
       regularization on the shape covariance matrix. See
       corresponding Set/Get method documentation for more
       information. */
   std::vector<double> m_RegularizationInitial;
   std::vector<double> m_RegularizationFinal;
   std::vector<double> m_RegularizationDecaySpan;
   
  /** When set to true, this flag will prevent GenerateData from
      starting the optimization.  When m_Initializing is true,
      GenerateData will allocated all data structures and set
      necessary parameters, but return before running the
      optimization. */
   bool m_Initializing;
   
   /** This flag is set to true if GenerateData has previously been
       called and all the necessary temporary data structures have been
       allocated. */
  bool m_Initialized;
  
  /** This method sets some numerical parameters in the optimization
      functions based on the given image inputs.  It is called in the
      first call of GenerateData. */
  virtual void InitializeOptimizationFunctions();
  
  /** Data structure used by the optimization metric to store local
      entropy estimation parameters. */
  typename PSMContainerArrayAttribute<double, Dimension>::Pointer m_SigmaCache;
  //  typename PSMMeanCurvatureAttribute<typename ImageType::PixelType, Dimension>
  //    ::Pointer m_MeanCurvatureCache;
  
  /** The entropy optimization metric in shape space.. */
  typename PSMShapeEntropyFunction<Dimension>::Pointer m_ShapeEntropyFunction;
  
  /** The entropy optimization on surfaces. */
  typename PSMParticleEntropyFunction<typename TImage::PixelType, Dimension>::Pointer
    m_ParticleEntropyFunction;
  
  /** The optimization metric that combined the shape-space and
      surface entropy optimimization metrics. */
  typename PSMTwoCostFunction<Dimension>::Pointer m_CostFunction;
  
  /** The matrix of all point positions across all shapes that is used
      by the ShapeEntropyFunction. */
  ShapeMatrixPointerType m_ShapeMatrix;
  
  /** Callback function object that will be invoked after each
      iteration of the solver. */
  typename itk::MemberCommand< PSMEntropyModelFilter<TImage,TShapeMatrix> >
    ::Pointer m_IterateCallback;
  
  /** The particle system. */
  typename ParticleSystemType::Pointer m_ParticleSystem;
  
  /** The filter can be initialized with correspondence points.  This
      is a vector of correspondence point lists.  It should be the
      same length as the number of input images. */
  std::vector<std::vector<PointType> > m_InputCorrespondencePoints;

  /** The set of names for the domains. Same order as the
      m_DomainList*/
  std::vector<std::string> m_DomainListNames;
 
  /** The set of all shape domains.*/
  std::vector<typename PSMImplicitSurfaceDomain<
  typename ImageType::PixelType, Dimension>::Pointer> m_DomainList;
 
 /** Set of neighborhood objects that tracks point positions on each
      of the surface domains.  */
  std::vector<typename PSMSurfaceNeighborhood<ImageType>::Pointer> m_NeighborhoodList;
  
  /** The number of scales for the optimization. See
      Set/GetNumberOfScales for more information. */
  unsigned int m_NumberOfScales;
  
  /** Keeps track of the current scale being processed.  This is used
      to enable interactive stopping and starting of the filter at
      different scales. */
  unsigned int m_CurrentScale;
  
  /** The optimization tolerance for each scale.  It is resized
      on-the-fly as values are added.*/
  std::vector<double> m_Tolerances;

  /** List of cutting planes. One per domain. */
  std::vector<CuttingPlaneType> m_CuttingPlanes;
};
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPSMEntropyModelFilter.hxx"
#endif

#endif

