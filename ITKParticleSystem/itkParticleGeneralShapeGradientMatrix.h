#ifndef ITKPARTICLEGENERALSHAPEGRADIENTMATRIX_H
#define ITKPARTICLEGENERALSHAPEGRADIENTMATRIX_H

#include "itkParticleAttribute.h"
#include "itkDataObject.h"
#include "itkWeakPointer.h"
#include "itkParticleContainer.h"
#include "vnl/vnl_matrix.h"

#include "itkParticleImplicitSurfaceDomain.h"
#include "itkParticleImageDomainWithGradients.h"
#include "itkParticleImageDomainWithHessians.h"
#include "TriMesh.h"

namespace itk
{
/** \class ParticleGeneralShapeGradientMatrix
 *
 * \brief Each column describes a shape.  A shape may be composed of
 * m_DomainsPerShape domains (default 1). ALL DOMAINS ARE *NOT* ASSUMED TO HAVE THE
 * SAME NUMBER OF PARTICLES!
 *
 *
 * Each column represents a single shape.
 */
template <class T, unsigned int VDimension>
class ITK_EXPORT ParticleGeneralShapeGradientMatrix
        : public vnl_matrix<T>, public ParticleAttribute<VDimension>
{
public:
    /** Standard class typedefs */
    typedef T DataType;
    typedef ParticleGeneralShapeGradientMatrix Self;
    typedef ParticleAttribute<VDimension> Superclass;
    typedef SmartPointer<Self>  Pointer;
    typedef SmartPointer<const Self>  ConstPointer;
    typedef WeakPointer<const Self>  ConstWeakPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self)

    /** Run-time type information (and related methods). */
    itkTypeMacro(ParticleGeneralShapeGradientMatrix, ParticleAttribute)

    virtual void BeforeIteration() {}
    virtual void AfterIteration() {}

    /** Set/Get the number of domains per shape.  This can only be safely done
      before shapes are initialized with points! */
    void SetDomainsPerShape(int i)
    { m_DomainsPerShape = i;     }
    int GetDomainsPerShape() const
    { return m_DomainsPerShape;  }

    void SetAttributesPerDomain(const std::vector<int> &i)
    { m_AttributesPerDomain = i; }

    void SetAttributeScales( const std::vector<double> &s)
    { m_AttributeScales = s;     }

    void SetXYZ(int i, bool val)
    {
        if (m_use_xyz.size() != m_DomainsPerShape)
            m_use_xyz.resize(m_DomainsPerShape);
        m_use_xyz[i] = val;
    }
    void SetNormals(int i, bool val)
    {
        if (m_use_normals.size() != m_DomainsPerShape)
            m_use_normals.resize(m_DomainsPerShape);
        m_use_normals[i] = val;
    }

    virtual void SetMatrix(const vnl_matrix<T> &m)
    {
        vnl_matrix<T>::operator=(m);
    }

    virtual void ResizeMatrix(int rs, int cs)
    {
        vnl_matrix<T> tmp(*this); // copy existing  matrix

        // Create new column (shape)
        this->set_size(rs, cs);

        // Copy old data into new matrix.
        for (unsigned int c = 0; c < tmp.cols(); c++)
        {
            for (unsigned int r = 0; r < tmp.rows(); r++)
                this->operator()(r,c) = tmp(r,c);
        }
    }

    virtual void DomainAddEventCallback(Object *, const EventObject &e)
    {
        const itk::ParticleDomainAddEvent &event = dynamic_cast<const itk::ParticleDomainAddEvent &>(e);
        unsigned int d = event.GetDomainIndex();

        if ( d % m_DomainsPerShape  == 0 )
            this->ResizeMatrix(this->rows(), this->cols()+3); //3 columns for every shape
    }

    virtual void PositionAddEventCallback(Object *o, const EventObject &e)
    {
        // update the size of matrix based on xyz, normals and number of attributes being used
        const itk::ParticlePositionAddEvent &event = dynamic_cast<const itk::ParticlePositionAddEvent &>(e);
        const itk::ParticleSystem<VDimension> *ps= dynamic_cast<const itk::ParticleSystem<VDimension> *>(o);
        const int d = event.GetDomainIndex();
        const unsigned int idx = event.GetPositionIndex();
//        const typename itk::ParticleSystem<VDimension>::PointType pos = ps->GetTransformedPosition(idx, d);
        const typename itk::ParticleSystem<VDimension>::PointType posLocal = ps->GetPosition(idx, d);

        int numRows = 0;
        for (int i = 0; i < m_DomainsPerShape; i++)
        {
            if (m_use_xyz[i])
                numRows += VDimension * ps->GetNumberOfParticles(i);
            if (m_use_normals[i])
                numRows += VDimension * ps->GetNumberOfParticles(i);
            numRows += m_AttributesPerDomain[i];
        }

        if (numRows > this->rows())
            this->ResizeMatrix(numRows, this->cols());

        //setting values now
        unsigned int k = 0;
        int dom = d % m_DomainsPerShape;
        int num = 0;
        for (int i = 0; i < dom; i++)
        {
            if (m_use_xyz[i])
            {
                k += VDimension * ps->GetNumberOfParticles(i);
                num += VDimension;
            }
            if (m_use_normals[i])
            {
                k += VDimension * ps->GetNumberOfParticles(i);
                num += VDimension;
            }
            k += m_AttributesPerDomain[i]* ps->GetNumberOfParticles(i);
            num += m_AttributesPerDomain[i];
        }
        if (m_use_xyz[dom])
            k += idx * VDimension;
        if (m_use_normals[dom])
            k += idx * VDimension;
        k += idx * m_AttributesPerDomain[dom];

        const ParticleImplicitSurfaceDomain<float, 3>* domain
                = static_cast<const ParticleImplicitSurfaceDomain<float ,3> *>(ps->GetDomain(d));

        const ParticleImageDomainWithGradients<float, 3> * domainWithGrad
                = static_cast<const ParticleImageDomainWithGradients<float ,3> *>(ps->GetDomain(d));

        const ParticleImageDomainWithHessians<float, 3> * domainWithHess
                = static_cast<const ParticleImageDomainWithHessians<float ,3> *>(ps->GetDomain(d));

        TriMesh *ptr = domain->GetMesh();

        int s = 0;
        if (m_use_xyz[dom])
        {
            for (unsigned int i = 0; i < VDimension; i++)
            {
                for (unsigned int j = 0; j < VDimension; j++)
                {
                    if (i == j)
                        this->operator()(i+k, j + 3* (d / m_DomainsPerShape)) = 1.0*m_AttributeScales[num+i];
                    else
                        this->operator()(i+k, j + 3* (d / m_DomainsPerShape)) = 0.0;
                }

            }
            k += VDimension;
            s += VDimension;
        }
        if (m_use_normals[dom])
        {

            typename ParticleImageDomainWithGradients<float,3>::VnlVectorType pG = domainWithGrad->SampleGradientVnl(posLocal);
            typename ParticleImageDomainWithGradients<float,3>::VnlVectorType pN = pG.normalize();
            float grad_mag = pG.magnitude();

            typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType pH = domainWithHess->SampleHessianVnl(posLocal);

            typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType mat1;
            mat1.set_identity();
            vnl_matrix<float> nrml(VDimension, 1);
            nrml.fill(0.0);
            nrml(0,0) = pN[0]; nrml(1,0) = pN[1]; nrml(2,0) = pN[2];
            typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType mat2 = nrml * nrml.transpose();

            for (unsigned int x1 = 0; x1 < VDimension; x1++)
            {
                for (unsigned int x2 = 0; x2 < VDimension; x2++)
                {
                    mat1(x1, x2) -= mat2(x1, x2);
                    pH(x1, x2)   /= grad_mag;
                }
            }
            // mat3 = H/|grad_f| * (I - n*n');
            typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType mat3 = pH * mat1;
            typename itk::ParticleSystem<VDimension>::VnlMatrixType tmp;
            tmp.set_size(VDimension, VDimension);
            tmp.fill(0.0);
            for (unsigned int c = 0; c<VDimension; c++)
            {
                for (unsigned vd = 0; vd<VDimension; vd++)
                    tmp(c,vd) = mat3(c,vd);
            }

            tmp = ps->TransformNormalDerivative(tmp, ps->GetTransform(d) * ps->GetPrefixTransform(d));

            for (unsigned int c = s; c < s+VDimension; c++)
            {
                for (unsigned int vd = 0; vd < VDimension; vd++)
                    this->operator()(c-s+k, vd + 3 * (d / m_DomainsPerShape)) = tmp(c-s, vd)*m_AttributeScales[num+c];
            }
            s += VDimension;
            k += VDimension;
        }

        if (m_AttributesPerDomain[dom] > 0)
        {
            point pt;
            pt.clear();
            pt[0] = posLocal[0];
            pt[1] = posLocal[1];
            pt[2] = posLocal[2];

            for (int aa = 0; aa < m_AttributesPerDomain[dom]; aa++)
            {
                point dc;
                dc.clear();
                dc = ptr->GetFeatureDerivative(pt, aa);
                for (unsigned int vd = 0; vd < VDimension; vd++)
                    this->operator()(aa+k, vd + 3 * (d / m_DomainsPerShape)) = dc[vd]*m_AttributeScales[num+aa+s];
            }
        }

    }

    virtual void PositionSetEventCallback(Object *o, const EventObject &e)
    {
        // update xyz, normals and number of attributes being used
        const itk::ParticlePositionSetEvent &event = dynamic_cast<const itk::ParticlePositionSetEvent &>(e);
        const itk::ParticleSystem<VDimension> *ps= dynamic_cast<const itk::ParticleSystem<VDimension> *>(o);
        const int d = event.GetDomainIndex();
        const unsigned int idx = event.GetPositionIndex();
//        const typename itk::ParticleSystem<VDimension>::PointType pos = ps->GetTransformedPosition(idx, d);
        const typename itk::ParticleSystem<VDimension>::PointType posLocal = ps->GetPosition(idx, d);

        unsigned int k = 0;
        int dom = d % m_DomainsPerShape;
        int num = 0;
        for (int i = 0; i < dom; i++)
        {
            if (m_use_xyz[i])
            {
                k += VDimension * ps->GetNumberOfParticles(i);
                num += VDimension;
            }
            if (m_use_normals[i])
            {
                k += VDimension * ps->GetNumberOfParticles(i);
                num += VDimension;
            }
            k += m_AttributesPerDomain[i]* ps->GetNumberOfParticles(i);
            num += m_AttributesPerDomain[i];
        }
        if (m_use_xyz[dom])
            k += idx * VDimension;
        if (m_use_normals[dom])
            k += idx * VDimension;
        k += idx * m_AttributesPerDomain[dom];

        const ParticleImplicitSurfaceDomain<float, 3>* domain
                = static_cast<const ParticleImplicitSurfaceDomain<float ,3> *>(ps->GetDomain(d));

        const ParticleImageDomainWithGradients<float, 3> * domainWithGrad
                = static_cast<const ParticleImageDomainWithGradients<float ,3> *>(ps->GetDomain(d));

        const ParticleImageDomainWithHessians<float, 3> * domainWithHess
                = static_cast<const ParticleImageDomainWithHessians<float ,3> *>(ps->GetDomain(d));

        TriMesh *ptr = domain->GetMesh();

        int s = 0;
        if (m_use_xyz[dom])
        {
            for (unsigned int i = 0; i < VDimension; i++)
            {
                for (unsigned int j = 0; j < VDimension; j++)
                {
                    if (i == j)
                        this->operator()(i+k, j + 3* (d / m_DomainsPerShape)) = 1.0*m_AttributeScales[num+i];
                    else
                        this->operator()(i+k, j + 3* (d / m_DomainsPerShape)) = 0.0;
                }

            }
            k += VDimension;
            s += VDimension;
        }
        if (m_use_normals[dom])
        {

            typename ParticleImageDomainWithGradients<float,3>::VnlVectorType pG = domainWithGrad->SampleGradientVnl(posLocal);
            typename ParticleImageDomainWithGradients<float,3>::VnlVectorType pN = pG.normalize();
            float grad_mag = pG.magnitude();

            typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType pH = domainWithHess->SampleHessianVnl(posLocal);

            typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType mat1;
            mat1.set_identity();
            vnl_matrix<float> nrml(VDimension, 1);
            nrml.fill(0.0);
            nrml(0,0) = pN[0]; nrml(1,0) = pN[1]; nrml(2,0) = pN[2];
            typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType mat2 = nrml * nrml.transpose();

            for (unsigned int x1 = 0; x1 < VDimension; x1++)
            {
                for (unsigned int x2 = 0; x2 < VDimension; x2++)
                {
                    mat1(x1, x2) -= mat2(x1, x2);
                    pH(x1, x2)   /= grad_mag;
                }
            }

            // mat3 = H/|grad_f| * (I - n*n');
            typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType mat3 = pH * mat1;
            typename itk::ParticleSystem<VDimension>::VnlMatrixType tmp;
            tmp.set_size(VDimension, VDimension);
            tmp.fill(0.0);
            for (unsigned int c = 0; c<VDimension; c++)
            {
                for (unsigned vd = 0; vd<VDimension; vd++)
                    tmp(c,vd) = mat3(c,vd);
            }

            tmp = ps->TransformNormalDerivative(tmp, ps->GetTransform(d) * ps->GetPrefixTransform(d));

            for (unsigned int c = s; c < s+VDimension; c++)
            {
                for (unsigned int vd = 0; vd < VDimension; vd++)
                    this->operator()(c-s+k, vd + 3 * (d / m_DomainsPerShape)) = tmp(c-s, vd)*m_AttributeScales[num+c];
            }
            s += VDimension;
            k += VDimension;
        }


        if (m_AttributesPerDomain[dom] > 0)
        {
            point pt;
            pt.clear();
            pt[0] = posLocal[0];
            pt[1] = posLocal[1];
            pt[2] = posLocal[2];

            for (int aa = 0; aa < m_AttributesPerDomain[dom]; aa++)
            {
                point dc;
                dc.clear();
                dc = ptr->GetFeatureDerivative(pt, aa);
                for (unsigned int vd = 0; vd < VDimension; vd++)
                    this->operator()(aa+k, vd + 3 * (d / m_DomainsPerShape)) = dc[vd]*m_AttributeScales[num+aa+s];
            }
        }
    }

    virtual void PositionRemoveEventCallback(Object *, const EventObject &)
    {
        // NEED TO IMPLEMENT THIS
    }

protected:
    ParticleGeneralShapeGradientMatrix()
    {
        m_DomainsPerShape = 1;

        this->m_DefinedCallbacks.DomainAddEvent = true;
        this->m_DefinedCallbacks.PositionAddEvent = true;
        this->m_DefinedCallbacks.PositionSetEvent = true;
        this->m_DefinedCallbacks.PositionRemoveEvent = true;
    }
    virtual ~ParticleGeneralShapeGradientMatrix() {}

    void PrintSelf(std::ostream& os, Indent indent) const
    {  Superclass::PrintSelf(os,indent);  }

    int m_DomainsPerShape;
private:

    ParticleGeneralShapeGradientMatrix(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    std::vector<bool> m_use_xyz;
    std::vector<bool> m_use_normals;
    std::vector<int> m_AttributesPerDomain;
    std::vector<double> m_AttributeScales;

}; // end class

} // end namespace



#endif // ITKPARTICLEGENERALSHAPEGRADIENTMATRIX_H
