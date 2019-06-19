#ifndef ITKPARTICLEMESHBASEDGENERALENTROPYGRADIENTFUNCTION_H
#define ITKPARTICLEMESHBASEDGENERALENTROPYGRADIENTFUNCTION_H

/*
 * Author: Praful
 * Class for mesh based features/attributes for shape correspondence
 */

#include "itkParticleVectorFunction.h"
#include <vector>
#include <numeric>
#include "itkParticleGeneralShapeMatrix.h"
#include "itkParticleGeneralShapeGradientMatrix.h"

namespace itk
{

template <unsigned int VDimension>
class ParticleMeshBasedGeneralEntropyGradientFunction : public ParticleVectorFunction<VDimension>
{
public:
    /** Standard class typedefs. */
    typedef ParticleMeshBasedGeneralEntropyGradientFunction Self;
    typedef SmartPointer<Self>  Pointer;
    typedef SmartPointer<const Self>  ConstPointer;
    typedef ParticleVectorFunction<VDimension> Superclass;
    itkTypeMacro( ParticleMeshBasedGeneralEntropyGradientFunction, ParticleVectorFunction)

    /** Type of particle system. */
    typedef typename Superclass::ParticleSystemType ParticleSystemType;

    typedef ParticleGeneralShapeMatrix<double, VDimension> ShapeDataType;
    typedef ParticleGeneralShapeGradientMatrix<double, VDimension> ShapeGradientType;

    typedef typename ShapeDataType::DataType DataType;

    /** Vector & Point types. */
    typedef typename Superclass::VectorType VectorType;
    typedef typename ParticleSystemType::PointType PointType;
    typedef vnl_vector<DataType> vnl_vector_type;
    typedef vnl_matrix<DataType> vnl_matrix_type;

    /** Method for creation through the object factory. */
    itkNewMacro(Self)

    /** Dimensionality of the domain of the particle system. */
    itkStaticConstMacro(Dimension, unsigned int, VDimension);

    /**Access the shape matrix. */
    void SetShapeData( ShapeDataType *s)
    {    m_ShapeData = s;  }
    ShapeDataType *GetShapeData()
    {   return  m_ShapeData.GetPointer();  }
    const ShapeDataType *GetShapeData() const
    {   return  m_ShapeData.GetPointer();  }

    /**Access the shape gradient matrix. */
    void SetShapeGradient( ShapeGradientType *s)
    {    m_ShapeGradient = s;  }
    ShapeGradientType *GetShapeGradient()
    {   return  m_ShapeGradient.GetPointer();  }
    const ShapeGradientType *GetShapeGradient() const
    {   return  m_ShapeGradient.GetPointer();  }


    /** The first argument is a pointer to the particle system.  The second
         argument is the index of the domain within that particle system.  The
         third argument is the index of the particle location within the given
         domain. */

    virtual VectorType Evaluate(unsigned int, unsigned int,
                                const ParticleSystemType *, double &, double&) const;

    virtual VectorType Evaluate(unsigned int a,  unsigned int b,
                                const ParticleSystemType *c, double &d) const
    {
        double e;
        return this->Evaluate(a,b,c,d,e);
    }


    virtual double Energy(unsigned int a, unsigned int b,
                          const ParticleSystemType *c) const
    {
        double e, d;
        this->Evaluate(a,b,c,d,e);
        return e;
    }


    /** Called before each iteration of a solver. */
    virtual void BeforeIteration()
    {
        this->ComputeUpdates(this->m_ParticleSystem);
    }


    /** Called after each iteration of the solver. */
    virtual void AfterIteration()
    {
        // Update the annealing parameter.
        if (m_HoldMinimumVariance != true && !m_UseMeanEnergy)
        {
            m_Counter++;
            if (m_Counter >=  m_RecomputeCovarianceInterval)
            {
                m_Counter = 0;
                m_MinimumVariance *= m_MinimumVarianceDecayConstant;
            }
        }
    }
    void SetMinimumVarianceDecay(double initial_value, double final_value, double time_period)
    {
        m_MinimumVarianceDecayConstant =  exp(log(final_value / initial_value) / time_period);
        m_MinimumVariance = initial_value;
        m_HoldMinimumVariance = false;
    }

    void SetMinimumVariance( double d)
    { m_MinimumVariance = d;}
    double GetMinimumVariance() const
    { return m_MinimumVariance; }

    bool GetHoldMinimumVariance() const
    { return m_HoldMinimumVariance; }
    void SetHoldMinimumVariance(bool b)
    { m_HoldMinimumVariance = b; }

    void SetRecomputeCovarianceInterval(int i)
    { m_RecomputeCovarianceInterval = i; }
    int GetRecomputeCovarianceInterval() const
    { return m_RecomputeCovarianceInterval; }

    void SetAttributeScales( const std::vector<double> &s)
    { m_AttributeScales = s; }

    /** Set/Get the number of domains per shape. */
    void SetDomainsPerShape(int i)
    { m_DomainsPerShape = i; }
    int GetDomainsPerShape() const
    { return m_DomainsPerShape; }

    void SetAttributesPerDomain(const std::vector<int> &i)
    { m_AttributesPerDomain = i; }

    void UseMeanEnergy()
    { m_UseMeanEnergy = true; }
    void UseEntropy()
    { m_UseMeanEnergy = false; }

    void SetXYZ(int i, bool val)
    {
        if (m_UseXYZ.size() != m_DomainsPerShape)
            m_UseXYZ.resize(m_DomainsPerShape);
        m_UseXYZ[i] = val;
    }
    void SetNormals(int i, bool val)
    {
        if (m_UseNormals.size() != m_DomainsPerShape)
            m_UseNormals.resize(m_DomainsPerShape);
        m_UseNormals[i] = val;
    }

    bool CheckForNans(vnl_matrix_type mat)
    {
        bool flag = false;
        for (int i = 0; i < mat.rows(); i++)
        {
            for (int j = 0; j < mat.cols(); j++)
            {
                if (isnan(mat(i,j)))
                {
                    flag = true;
                    break;
                }
            }
        }
        return flag;
    }

    virtual typename ParticleVectorFunction<VDimension>::Pointer Clone()
    {
        typename ParticleMeshBasedGeneralEntropyGradientFunction<VDimension>::Pointer copy =
                ParticleMeshBasedGeneralEntropyGradientFunction<VDimension>::New();

        // from itkParticleVectorFunction
        copy->m_DomainNumber = this->m_DomainNumber;
        copy->m_ParticleSystem = this->m_ParticleSystem;

        // local
        copy->m_AttributeScales = this->m_AttributeScales;
        copy->m_Counter = this->m_Counter;
        copy->m_CurrentEnergy = this->m_CurrentEnergy;
        copy->m_HoldMinimumVariance = this->m_HoldMinimumVariance;
        copy->m_MinimumEigenValue  = this->m_MinimumEigenValue;
        copy->m_MinimumVariance = this->m_MinimumVariance;
        copy->m_MinimumVarianceDecayConstant = this->m_MinimumVarianceDecayConstant;
        copy->m_PointsUpdate = this->m_PointsUpdate;
        copy->m_RecomputeCovarianceInterval = this->m_RecomputeCovarianceInterval;
        copy->m_AttributesPerDomain = this->m_AttributesPerDomain;
        copy->m_DomainsPerShape = this->m_DomainsPerShape;
        copy->m_UseMeanEnergy = this->m_UseMeanEnergy;
        copy->m_mean = this->m_mean;
        copy->m_UseNormals = this->m_UseNormals;
        copy->m_UseXYZ = this->m_UseXYZ;
        copy->m_InverseCovMatrix = this->m_InverseCovMatrix;

        copy->m_ShapeData = this->m_ShapeData;
        copy->m_ShapeGradient = this->m_ShapeGradient;

        return (typename ParticleVectorFunction<VDimension>::Pointer)copy;
    }

protected:
    ParticleMeshBasedGeneralEntropyGradientFunction()
    {
        // m_MinimumVarianceBase = 1.0;//exp(log(1.0e-5)/10000.0);
        m_HoldMinimumVariance = true;
        m_MinimumVariance = 1.0e-5;
        m_MinimumEigenValue = 0.0;
        m_MinimumVarianceDecayConstant = 1.0; //log(2.0) / 50000.0;
        m_RecomputeCovarianceInterval = 1;
        m_Counter = 0;
        m_PointsUpdate.clear();
        m_UseMeanEnergy = true;
        m_UseNormals.clear();
        m_UseXYZ.clear();
        m_mean.clear();
        num_dims = 0;
        num_samples = 0;
    }
    virtual ~ParticleMeshBasedGeneralEntropyGradientFunction() {}
    void operator=(const ParticleMeshBasedGeneralEntropyGradientFunction &);
    ParticleMeshBasedGeneralEntropyGradientFunction(const ParticleMeshBasedGeneralEntropyGradientFunction &);

    typename ShapeDataType::Pointer m_ShapeData;
    typename ShapeGradientType::Pointer m_ShapeGradient;

    virtual void ComputeUpdates(const ParticleSystemType *c);
    vnl_matrix_type m_PointsUpdate;

    double m_MinimumVariance;
    double m_MinimumEigenValue;
    //  double m_MinimumVarianceBase;
    bool m_HoldMinimumVariance;
    int m_RecomputeCovarianceInterval;
    double m_MinimumVarianceDecayConstant;
    int m_Counter;
    std::vector<double> m_AttributeScales; //size \sum_i n_i
    int m_DomainsPerShape;
    std::vector<int> m_AttributesPerDomain; // n
    double m_CurrentEnergy;
    bool m_UseMeanEnergy;
    std::vector<bool> m_UseXYZ;
    std::vector<bool> m_UseNormals;
    vnl_matrix_type m_mean;
    vnl_matrix_type m_InverseCovMatrix;
    int num_dims, num_samples;

};
} // end namespace

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkParticleMeshBasedGeneralEntropyGradientFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkParticleMeshBasedGeneralEntropyGradientFunction.txx"
#endif

#endif // ITKPARTICLEMESHBASEDGENERALENTROPYGRADIENTFUNCTION_H
