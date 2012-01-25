/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleSystem.h,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleSystem_h
#define __itkParticleSystem_h

#include "itkDataObject.h"
#include "itkObjectFactory.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include "itkParticleAttribute.h"
#include "itkParticleDomain.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"
#include "itkParticleContainer.h"
#include "itkParticleEvents.h"
#include "itkCommand.h"
#include "itkEventObject.h"
#include "itkParticleNeighborhood.h"
#include "vnl/vnl_inverse.h"
#include <map>
#include <vector>

namespace itk
{
/** \class ParticleSystem
 *  \brief  A facade class managing interactions with a particle system.
 *
 * A particle system consists of a set of particle locations and domains in
 * which those locations are defined.  For example, a particle system may
 * simply be a set of points in 3D Euclidean space that are constrained to a
 * specified bounding box.  A more complex example is a set of 3D points
 * constrained to a given surface.  The itkParticleSystem class is also
 * designed to manage multiple sets of points across multiple domains.  For
 * example, one may create a system of dozens of point sets that are each
 * defined in different spaces.  The itkParticleSystem class can also maintain
 * a set of transformations associated with each domain that map that
 * particular domain into another coordinate frame, for example, a common
 * coordinate frame.
 */
template <unsigned int VDimension=3>
class ITK_EXPORT ParticleSystem : public DataObject
{
public:
  /** Standard class typedefs */
  typedef ParticleSystem Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleSystem, DataObject);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Define the base domain type. */
  typedef ParticleDomain<VDimension> DomainType;
  
  /** Point type used to store particle locations. */
  typedef Point<double, VDimension> PointType;

  /** Class used to compute neighborhoods of points. One is associated with
      each domain.*/
  typedef ParticleNeighborhood<VDimension> NeighborhoodType;
  
  /** Point container type.  One is associated with each domain.*/
  typedef ParticleContainer<PointType> PointContainerType;

  typedef typename NeighborhoodType::PointVectorType PointVectorType;
  
  /** Defines a transform class type.  One is associated with each
      domain. Transforms are affine for simplicity.  This could be extended by
      using the itk::Transform base type so that a variety of transform types
      may be used. */
  //  typedef Transform<double, VDimension, VDimension> TransformType;
  typedef vnl_matrix_fixed<double, VDimension +1, VDimension +1> TransformType;
  typedef vnl_vector_fixed<double, VDimension> VectorType;

  /** Register an attribute object with this particle system.  This action adds
      the attribute object as an observer for the particle system so that it
      will receive published updates.  For example, a gradient attribute object
      may need to know when particle positions have changed so that it can
      update its internal state appropriately (e.g. precompute a gradient that
      corresponds to the new particle position).  Internally, this method
      manages registering the appropriate itk::Command and itk::Object as an
      observer of this itk::Object (AddObserver).  Relevant state information
      is passed to objects in particle events.
  */
  void RegisterAttribute( ParticleAttribute<VDimension> *);


  /** Invokes the set event on all particle positions, resetting them to their
      current value.  This method may be called to synchronize positional
      information among various observers which may have gone out of sync. */
  void SynchronizePositions()
  {
    for (unsigned int d = 0; d < this->GetNumberOfDomains(); d++)
      {
      for (unsigned int p = 0; p < this->GetNumberOfParticles(d); p++)
        {
        this->SetPosition(this->GetPosition(p,d), p, d);
        }
      } 
  }
  
  /** Returns the number of particles in domain k. */
  unsigned long int GetNumberOfParticles(unsigned int d = 0) const
  { return m_Positions[d]->GetSize(); }
  
  /**  Add/Set/Remove a single particle position.  The actual position added or
      set will be returned.  If, for example, the domain imposes any
      constraints on this location (e.g. out-of-bounds, projection onto a
      surface), then the method returns the location after constraints are
      applied.  Both the Add and Set methods require a domain index.  If
      unspecified, the domain index is zero.  The Set method requires a
      specific particle index.  If the given index does not exist, an exception
      is thrown.  Set is intended to be used for moving particles. The
      SetTransformedPosition sets the position using a Point transformed by the
      m_Transform associated with the given domain.*/
  const PointType &AddPosition( const PointType &, unsigned int d=0, int threadId=0);
  const PointType &SetPosition( const PointType &,  unsigned long int k,  unsigned int d=0, int threadId=0);

  //  inline const PointType &SetTransformedPosition(const PointType &p,
  //                                                 unsigned long int k,  unsigned int d=0, int threadId=0)
  //  {
  //    this->SetPosition( this->TransformPoint(p, m_InverseTransform[d]), k, d, threadId );
  //  }

  void RemovePosition(unsigned long int k, unsigned int d=0,  int threadId=0);
  
  /** Return a position with index k from domain d.  Note the order in which the 2
      integers must be specified!   The domain number is specified second and
      the position index within the domain is specified first.  Note that the
      domain index may be omitted if the particle system only contains a single
      domain.  GetTransformedPosition returns the position premultiplied by the
      transform matrix for the given domain.*/
  PointType &GetPosition(unsigned long int k, unsigned int d=0)
  {    return m_Positions[d]->operator[](k);  }
  const PointType &GetPosition(unsigned long int k, unsigned int d=0) const
  {    return m_Positions[d]->operator[](k);  }
  PointType GetTransformedPosition(unsigned long int k, unsigned int d=0) const
  {    return this->TransformPoint(m_Positions[d]->operator[](k),
                                  m_Transforms[d] * m_PrefixTransforms[d]);  }

  /** Doubles the number of particles of the system by
      splitting each particle into 2 particles.  Each new particle position is
      added to the system at a random epsilon distance on the surface from the
      original particle.  The new particles are added using the AddPosition
      method, so all appropriate callbacks are invoked. */
  void SplitAllParticles(double epsilon, int threadId=0);
  void SplitAllParticlesInDomain(const vnl_vector_fixed<double, VDimension> &,
                                 double epsilon, unsigned int d=0, int threadId=0);
  void SplitParticle(double epsilon, unsigned int idx,  unsigned int d=0, int threadId=0);
  
  /** Set/Get the neighborhood object associated with domain k. */
  void SetNeighborhood(unsigned int,  NeighborhoodType *, int threadId=0);
  void SetNeighborhood(NeighborhoodType *n, int threadId=0)
  { this->SetNeighborhood(0, n, threadId); }
  typename NeighborhoodType::ConstPointer GetNeighborhood(unsigned int k) const
  { return m_Neighborhoods[k]; }

  /** Return the neighborhood of points with radius r around point p in domain
      k.  This is just a convenience method to avoid exposing the underlying
      Neighborhood objects. FindTransformedNeighborhoodPoints returns the list
      with all points transformed by the transform associated with the given
      domain.*/
  inline PointVectorType FindNeighborhoodPoints(const PointType &p,
                                                double r, unsigned int d = 0) const
  {  return m_Neighborhoods[d]->FindNeighborhoodPoints(p, r); }
  inline PointVectorType FindNeighborhoodPoints(const PointType &p,
                                                std::vector<double> &w,
                                                double r, unsigned int d = 0) const
  {  return m_Neighborhoods[d]->FindNeighborhoodPoints(p,w,r); }
  inline PointVectorType FindNeighborhoodPoints(unsigned int idx,
                                                double r, unsigned int d = 0) const
  {  return m_Neighborhoods[d]->FindNeighborhoodPoints(this->GetPosition(idx,d), r); }
  inline PointVectorType FindNeighborhoodPoints(unsigned int idx,
                                                std::vector<double> &w,
                                                double r, unsigned int d = 0) const
  {  return m_Neighborhoods[d]->FindNeighborhoodPoints(this->GetPosition(idx,d),w, r); }

  //  inline int FindNeighborhoodPoints(const PointType &p,  double r, PointVectorType &vec, unsigned int d = 0) const
  //  {  return m_Neighborhoods[d]->FindNeighborhoodPoints(p, r, vec); }
  
  //   PointVectorType FindTransformedNeighborhoodPoints(const PointType &p, double r, unsigned int d = 0) const
  //   {
  //     PointVectorType ans = m_Neighborhoods[d]
  //       ->FindNeighborhoodPoints(this->TransformPoint(p, InverseTransform[d]), r);
  //     for (unsigned int i = 0; i < ans.size(); i++)
  //       {
  //       ans.Point[i] = this->TransformPoint(ans.Point[i], m_Transform[d]);
  //       }
  //     return ans;
  //   }

  /** Add a domain to the particle system.  This method also allocates space
      for a list of positions that are contained within the domain, and a
      default neighborhood calculator.  The final, optional argument indicates
      the calling thread id.*/
  void AddDomain( DomainType *, int threadId =0);
  
  /** Return an iterator that points to the first element of the list of the
      domains. */
  typename std::vector< typename DomainType::Pointer >::const_iterator GetDomainsBegin() const
  { return m_Domains.begin(); }

  /** Return an iterator that points one position past the last element of the list of the
      domains. */
  typename std::vector< typename DomainType::Pointer >::const_iterator GetDomainsEnd() const
  { return m_Domains.end(); }

  /** Return the i'th domain object. */
  DomainType * GetDomain(unsigned int i)
  { return m_Domains[i].GetPointer(); }
  
  /** API for the single domain case. */
  DomainType * GetDomain()
  {return m_Domains[0].GetPointer(); }
  
  /** Return the i'th domain object. */
  const DomainType *GetDomain(unsigned int i) const
  { return m_Domains[i].GetPointer(); }
  
  /** API for the single domain case. */
  const DomainType *GetDomain() const
  {return m_Domains[0].GetPointer(); }

  /** Returns the number of domains contained in the particle system. */
  unsigned int GetNumberOfDomains() const
  { return m_Domains.size(); }
  
  /** Set the transform associated with domain i. This method will also compute
      and set the corresponding inverse transform if possible.  If the inverse
      transform cannot be computed, the exception is quietly handled by this
      method and an identity transform is set in its place.  The calling
      program is responsible for knowing when an inverse transform cannot be
      computed. */
  void SetTransform(unsigned int i, const TransformType &, int threadId=0);
  void SetTransform(const TransformType &p, int threadId=0)
  { this->SetTransform(0, p, threadId); }
  void SetPrefixTransform(unsigned int i, const TransformType &, int threadId=0);
  void SetPrefixTransform(const TransformType &p, int threadId=0)
  { this->SetPrefixTransform(0, p, threadId); }

  /** Return an iterator that points to the first element of the list of the
      transforms. */
  typename std::vector< TransformType >::const_iterator
  GetTransformsBegin() const  { return m_Transforms.begin(); }

  /** Return an iterator that points one position past the last element of the list of the
      transforms. */
  typename std::vector< TransformType >::const_iterator
  GetTransformsEnd() const  { return m_Transforms.end(); }
  
  /** Return the i'th transform object. */
  const TransformType &GetTransform(unsigned int i) const
  { return m_Transforms[i]; }
  
  /** API for the single transform case. */
  const TransformType &GetTransform() const
  {return m_Transforms[0]; }

  /** Return the i'th transform object. */
  TransformType GetTransform(unsigned int i)
  { return m_Transforms[i]; }
  
  /** API for the single transform case. */
  TransformType GetTransform()
  {return m_Transforms[0]; }

  /** Return the i'th transform object. */
  const TransformType &GetPrefixTransform(unsigned int i) const
  { return m_PrefixTransforms[i]; }
  
  /** API for the single transform case. */
  const TransformType &GetPrefixTransform() const
  {return m_PrefixTransforms[0]; }

  /** Return the i'th transform object. */
  TransformType GetPrefixTransform(unsigned int i)
  { return m_PrefixTransforms[i]; }
  
  /** API for the single transform case. */
  TransformType GetPrefixTransform()
  {return m_PrefixTransforms[0]; }
  
  /** Return an iterator that points to the first element of the list of the
      inverse transforms. */
  typename std::vector< TransformType >::const_iterator
  GetInverseTransformsBegin() const
  { return m_InverseTransforms.begin(); }

  /** Return an iterator that points one position past the last element of the list of the
      transforms. */
  typename std::vector< TransformType >::const_iterator
  GetInverseTransformsEnd() const
  { return m_InverseTransforms.end(); }
  
  /** Return the i'th transform object. */
  const TransformType &GetInverseTransform(unsigned int i) const
  { return m_InverseTransforms[i]; }
  
  /** API for the single transform case. */
  const TransformType &GetInverseTransform() const
  {return m_InverseTransforms[0]; }

    /** Return the i'th transform object. */
  const TransformType &GetInversePrefixTransform(unsigned int i) const
  { return m_InversePrefixTransforms[i]; }
  
  /** API for the single transform case. */
  const TransformType &GetInversePrefixTransform() const
  {return m_InversePrefixTransforms[0]; }

  /** Return the array of particle positions. */
  const  std::vector<typename  PointContainerType::Pointer> &  GetPositions() const
  { return m_Positions; }
  const  typename  PointContainerType::Pointer & GetPositions(unsigned int d) const
  { return m_Positions[d]; }

  /** Adds a list of points to the specified domain.  The arguments are the
     std::vector of points and the domain number. */
  void AddPositionList(const std::vector<PointType> &, unsigned int d = 0, int threadId = 0);

  /** Transforms a point using the given transform. NOTE: Scaling is not
      currently implemented. (This method may be converted to virtual and
      overridden if tranform type is generalized.)*/
  PointType TransformPoint(const PointType &, const TransformType &) const;

  /** Transforms a vector using the given transform.   Only the rotational part
      of the transform is applied. NOTE: Scaling is not currently supported.*/
  VectorType TransformVector(const VectorType &, const TransformType &) const;

  /** Returns the inverse of a transformation matrix.*/
  inline TransformType InvertTransform(const TransformType &T) const
  {
    // Note, vnl_inverse is optimized for small matrices 1x1 - 4x4
    return vnl_inverse(T);
  }

  /** Flag/Unflag a domain.  Flagging a domain has different meanings according
      to the application using this particle system. */
  void FlagDomain(unsigned int i)
  {std::cout<<"in flag domain  "<<i<<" "<<m_DomainFlags.size()<<std::endl; m_DomainFlags[i] = true;std::cout<<"end flad domain"<<std::endl; }
  void UnflagDomain(unsigned int i)
  { m_DomainFlags[i] = false; }
  bool GetDomainFlag(unsigned int i) const
  { return m_DomainFlags[i]; }
  //  bool &GetDomainFlag(unsigned int i)
  //  { return m_DomainFlags[i]; }
  const std::vector<bool> &GetDomainFlags() const
  { return m_DomainFlags; }
  void SetDomainFlags()
  { for (unsigned int i = 0; i < m_DomainFlags.size(); i++)  { m_DomainFlags[i] = true; }  }
  void ResetDomainFlags()
  { for (unsigned int i = 0; i < m_DomainFlags.size(); i++)  { m_DomainFlags[i] = false; }  }


  /** The following methods provide functionality for specifying particle
      indices that are fixed landmarks.  SetPosition() calls to these particle
      indices will silently fail. For simplicity, only one list of indices is      
      maintained for all dimensions.  If particle index n is flagged, for
      example, then particle index n in all domains is fixed.*/
  void SetFixedParticleFlag(unsigned int i)
  { if (i < m_FixedParticleFlags.size())  m_FixedParticleFlags[i] = true;  }
  void ResetFixedParticleFlag(unsigned int i)
  { if (i < m_FixedParticleFlags.size()) m_FixedParticleFlags[i] = false;}
  bool GetFixedParticleFlag(unsigned int i) const
  {
    if (i < m_FixedParticleFlags.size()) return m_FixedParticleFlags[i];
    else return false;
  }
  void ResetFixedParticleFlags()
  {
    for (unsigned int i = 0; i < m_FixedParticleFlags.size(); i++)
      { m_FixedParticleFlags[i] = false; }
  }
  
protected:
  ParticleSystem();
  void PrintSelf(std::ostream& os, Indent indent) const;
  virtual ~ParticleSystem() {};

  /** Set the number of domains.  This method modifies the size of the
      m_Domains, m_Positions, and m_Transform lists. */
  void SetNumberOfDomains( unsigned int );

  /** Return an iterator that points to the first element of the list of the
      domains. */
  typename std::vector< typename DomainType::Pointer >::iterator GetDomainsBegin()
  { return m_Domains.begin(); }

  /** Return an iterator that points one position past the last element of the list of the
      domains. */
  typename std::vector< typename DomainType::Pointer >::iterator GetDomainsEnd()
  { return m_Domains.end(); }
  
  /** Return an iterator that points to the first element of the list of the
      transforms. */
  typename std::vector< TransformType >::iterator GetTransformsBegin()
  { return m_Transforms.begin(); }

  /** Return an iterator that points one position past the last element of the list of the
      transforms. */
  typename std::vector< TransformType >::iterator GetTransformsEnd()
  { return m_Transforms.end(); }
  
  /** Return an iterator that points to the first element of the list of the
      inverse transforms. */
  typename std::vector< TransformType >::iterator
  GetInverseTransformsBegin()
  { return m_InverseTransforms.begin(); }

  /** Return an iterator that points one position past the last element of the list of the
      transforms. */
  typename std::vector< TransformType >::iterator
  GetInverseTransformsEnd()
  { return m_InverseTransforms.end(); }
  
  /** Return the i'th transform object. */
  TransformType &GetInverseTransform(unsigned int i)
  { return m_InverseTransforms[i]; }
  
  /** API for the single transform case. */
  TransformType &GetInverseTransform()
  {return m_InverseTransforms[0]; }

  /** Return the i'th transform object. */
  TransformType &GetInversePrefixTransform(unsigned int i)
  { return m_InversePrefixTransforms[i]; }
  
  /** API for the single transform case. */
  TransformType &GetInversePrefixTransform()
  {return m_InversePrefixTransforms[0]; }

private:
  ParticleSystem(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  /** The 2D array of particle positions.  1st array axis is the domain number.
      These values may only be modified by the ParticleSystem class itself. */
  std::vector<typename  PointContainerType::Pointer>  m_Positions;

  /** The set of particle domain definitions. */
  std::vector< typename DomainType::Pointer > m_Domains;

  /** The set of domain neighborhood objects. */
  std::vector< typename NeighborhoodType::Pointer > m_Neighborhoods;
               
  /** The set of domain transform objects */
  std::vector< TransformType > m_Transforms;

  /** The set of inverse transform objects */
  std::vector< TransformType > m_InverseTransforms;

  /** The set of domain transform objects */
  std::vector< TransformType > m_PrefixTransforms;

  /** The set of domain transform objects */
  std::vector< TransformType > m_InversePrefixTransforms;

  /** A counter used to assign indicies for new particle locations. */
  std::vector< unsigned long int> m_IndexCounters;

  /** An array that indicates which domains are flagged.  A domain may be
      flagged for various purposes depending on the application.  This array
      was created to flag which domains are "fixed" during optimization, for
      example. */
  std::vector<bool> m_DomainFlags;

  /** An array that indicates which particle indices (in any domain) will
      not respond to SetPosition. */
  std::vector<bool> m_FixedParticleFlags;
};

} // end namespace itk

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkParticleSystem+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkParticleSystem.txx"
#endif

#endif
