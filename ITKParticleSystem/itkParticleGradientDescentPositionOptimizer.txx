/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleGradientDescentPositionOptimizer.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleGradientDescentPositionOptimizer_txx
#define __itkParticleGradientDescentPositionOptimizer_txx

#ifdef SW_USE_OPENMP
#include <omp.h>
const int global_iteration = 1;
#else /* SW_USE_OPENMP */
const int global_iteration = 1;
#endif /* SW_USE_OPENMP */

#include <algorithm>
#include <ctime>
#include <time.h>
#include <string>
#include "itkParticleImageDomainWithGradients.h"
#include <vector>
#include <fstream>
#include <sstream>
namespace itk
{
template <class TGradientNumericType, unsigned int VDimension>
ParticleGradientDescentPositionOptimizer<TGradientNumericType, VDimension>
::ParticleGradientDescentPositionOptimizer()
{
    m_StopOptimization = false;
    m_NumberOfIterations = 0;
    m_MaximumNumberOfIterations = 0;
    m_Tolerance = 0.0;
    m_TimeStep = 1.0;
    m_OptimizationMode = 0;
}

/*** ADAPTIVE GAUSS SEIDEL ***/
template <class TGradientNumericType, unsigned int VDimension>
void
ParticleGradientDescentPositionOptimizer<TGradientNumericType, VDimension>
::StartAdaptiveGaussSeidelOptimization()
{
//    std::cout << "Starting Adaptive Gauss Seidel Optimization ..... \n" << std::flush;
    const double factor = 1.1;//1.1;
    //  const double epsilon = 1.0e-4;

    // NOTE: THIS METHOD WILL NOT WORK AS WRITTEN IF PARTICLES ARE
    // ADDED TO THE SYSTEM DURING OPTIMIZATION.
    m_StopOptimization = false;
    m_GradientFunction->SetParticleSystem(m_ParticleSystem);

    typedef typename DomainType::VnlVectorType NormalType;

    bool reset = false;
    // Make sure the time step vector is the right size
    while (m_TimeSteps.size() != m_ParticleSystem->GetNumberOfDomains() )
    {
        reset = true;
std::vector<double> tmp;
        std::vector<double> tmp1;
        m_TimeSteps.push_back( tmp );
        if(m_TimeStepsDx.size() != m_ParticleSystem->GetNumberOfDomains())//Added by Anupama
            m_TimeStepsDx.push_back( tmp1 ); //Added by Anupama
    }

    for (unsigned int i = 0; i < m_ParticleSystem->GetNumberOfDomains(); i++)
    {
        unsigned int np = m_ParticleSystem->GetPositions(i)->GetSize();
        if (m_TimeSteps[i].size() != np)
        {
            // resize and initialize everything to 1.0
            m_TimeSteps[i].resize(np);
        }
        if (m_TimeStepsDx[i].size() != np)//Added by Anupama
        {
            // resize and initialize everything to 1.0
            m_TimeStepsDx[i].resize(np); //Added by Anupama
        }
        for (unsigned int j = 0; j < np; j++)
        {
            m_TimeSteps[i][j] = 1.0;
            m_TimeStepsDx[i][j] = 0.000001;//Added by Anupama
        }
        reset = true;
    }

    const double pi = std::acos(-1.0);
    unsigned int numdomains = m_ParticleSystem->GetNumberOfDomains();
    std::vector<double> meantime(numdomains);
    std::vector<double> maxtime(numdomains);
    std::vector<double> mintime(numdomains);

    std::vector<double> meantimeDx(numdomains); //Added by Anupama
    std::vector<double> maxtimeDx(numdomains); //Added by Anupama
    std::vector<double> mintimeDx(numdomains); //Added by Anupama

    unsigned int counter = 0;

    for (unsigned int q = 0; q < numdomains; q++)
    {
        meantime[q] = 0.0;
        maxtime[q]  = 1.0e30;
        mintime[q]  = 1.0;
        meantimeDx[q] = 0.0; //Added by Anupama
        maxtimeDx[q]  = 1.0e30; //Added by Anupama
        mintimeDx[q]  = 1.0; //Added by Anupama
    }
    time_t timerBefore, timerAfter;

    double maxchange = 0.0;
    while (m_StopOptimization == false) // iterations loop
    {
        timerBefore = time(NULL);
        if (counter % global_iteration == 0)
            m_GradientFunction->BeforeIteration();
        counter++;

        std::vector<std::vector<VectorType> > particleGradients(m_ParticleSystem->GetNumberOfDomains()); //Added by Anupama

        for (unsigned int d = 0; d < m_ParticleSystem->GetNumberOfDomains(); d++)
            particleGradients[d].resize(m_ParticleSystem->GetPositions(d)->GetSize()); //Added by Anupama

/*#pragma omp parallel
        {
            // Iterate over each domain
#pragma omp for*/
            for (int dom = 0; dom < numdomains; dom++)
            {
                int tid = 1;
                int num_threads = 1;

/*#ifdef SW_USE_OPENMP
                tid = omp_get_thread_num() + 1;
                num_threads = omp_get_num_threads();
#endif*/ /* SW_USE_OPENMP */

                meantime[dom] = 0.0;
                // skip any flagged domains
                if (m_ParticleSystem->GetDomainFlag(dom) == false)
                {
                    double maxdt;

                    VectorType gradient;
                    VectorType newGradient; //Added by ANupama
                    VectorType original_gradient;
                    PointType newpoint;

                    const DomainType * domain = static_cast<const DomainType *>(m_ParticleSystem->GetDomain(dom));

                    typename GradientFunctionType::Pointer localGradientFunction;

                    localGradientFunction = m_GradientFunction;

/*#ifdef SW_USE_OPENMP
                    localGradientFunction = m_GradientFunction->Clone();
#endif*/ /* SW_USE_OPENMP */


                    // Tell function which domain we are working on.
                    localGradientFunction->SetDomainNumber(dom);

                    // Iterate over each particle position
                    unsigned int k = 0;
                    maxchange = 0.0;
                    typename ParticleSystemType::PointContainerType::ConstIterator endit =
                            m_ParticleSystem->GetPositions(dom)->GetEnd();
                    for (typename ParticleSystemType::PointContainerType::ConstIterator it
                         = m_ParticleSystem->GetPositions(dom)->GetBegin(); it != endit; it++, k++)
                    {
                        bool done = false;

                        // Compute gradient update.
                        double energy = 0.0;
                        localGradientFunction->BeforeEvaluate(it.GetIndex(), dom, m_ParticleSystem);
                        original_gradient = localGradientFunction->Evaluate(it.GetIndex(), dom, m_ParticleSystem, maxdt, energy);
                        //double offset_smoothness = localGradientFunction->OffsetSmoothness(it.GetIndex(), dom, m_ParticleSystem);

                        /*Below block Added by Anupama*/

                        PointType pos = m_ParticleSystem->GetPosition(it.GetIndex(), dom);
                        PointType prev_pos = m_ParticleSystem->GetPrevPosition(it.GetIndex(), dom);
                        double prev_offset = m_ParticleSystem->GetPrevOffset(it.GetIndex(), dom);
                        double offset = m_ParticleSystem->GetOffset(it.GetIndex(), dom);
                        VectorType gradDx;
                        VectorType gradDxSmoothness;
                        double epsilonval=1.0e-6;
                        double alpha =1.0e6;
                        double value1=(1/(1+exp(-alpha*offset)));
                        double value2=(1/(1+exp(alpha*offset)));
                        double diffXtilda[3] ={(pos[0]-prev_pos[0]), (pos[1]-prev_pos[1]), (pos[2]-prev_pos[2])};
                        double diffDeltaX = offset - prev_offset;

                        for (int i=0; i<=2; i++)
                          {
                              if(diffXtilda[i]==0)
                                 diffXtilda[i]=epsilonval;
                          }

                        for (int i=0; i<=2; i++)
                            gradDx[i]=(diffDeltaX/diffXtilda[i])*(value1-value2);

                        /*for (int i=0; i<=2; i++)
                            gradDxSmoothness[i]=(diffDeltaX/diffXtilda[i])*(offset_smoothness);*/

                        for (int i=0; i<=2; i++)
                            newGradient[i]= original_gradient[i]+gradDx[i];//+gradDxSmoothness[i];

                           /*Above block Added by Anupama*/


                        unsigned int idx = it.GetIndex();
                        PointType pt = *it;
                        NormalType ptNormalOld = domain->SampleNormalVnl(pt);

                       // double dotPdt = original_gradient[0]*ptNormalOld[0] + original_gradient[1]*ptNormalOld[1] + original_gradient[2]*ptNormalOld[2];
                       // VectorType original_gradient_projectedOntoTangentSpace;
                        //original_gradient_projectedOntoTangentSpace[0] = original_gradient[0] - dotPdt*ptNormalOld[0] ;
                       // original_gradient_projectedOntoTangentSpace[1] = original_gradient[1] - dotPdt*ptNormalOld[1] ;
                       // original_gradient_projectedOntoTangentSpace[2] = original_gradient[2] - dotPdt*ptNormalOld[2] ;

                        /*Block below modified by Anupama*/
                        double dotPdt = newGradient[0]*ptNormalOld[0] + newGradient[1]*ptNormalOld[1] + newGradient[2]*ptNormalOld[2];
                        VectorType original_gradient_projectedOntoTangentSpace;
                        original_gradient_projectedOntoTangentSpace[0] = newGradient[0] - dotPdt*ptNormalOld[0] ;
                        original_gradient_projectedOntoTangentSpace[1] = newGradient[1] - dotPdt*ptNormalOld[1] ;
                        original_gradient_projectedOntoTangentSpace[2] = newGradient[2] - dotPdt*ptNormalOld[2] ;
                        /*Block above modified by Anupama*/

                        double newenergy, gradmag;
                        while ( !done )
                        {
                            VectorType null_gradient;
                            for(int i=0; i<VDimension; i++)
                                null_gradient[i]=0.0;
                            particleGradients[dom][it.GetIndex()]=null_gradient;//Added by Anupama
                            gradient = original_gradient_projectedOntoTangentSpace * m_TimeSteps[dom][k];

                            dynamic_cast<DomainType *>(m_ParticleSystem->GetDomain(dom))->ApplyVectorConstraints(gradient, m_ParticleSystem->GetPosition(it.GetIndex(), dom), maxdt);

                            gradmag = gradient.magnitude();
                            if (gradmag > maxdt)
                            {
                                m_TimeSteps[dom][k] /= factor;
                            }
                            else // Move is not too large
                            {
                                // Make a move and compute new energy
                                for (unsigned int i = 0; i < VDimension; i++)
                                {    newpoint[i] = pt[i] - gradient[i];
                                }


                                dynamic_cast<DomainType *>(m_ParticleSystem->GetDomain(dom))->ApplyConstraints(newpoint);

                                 particleGradients[dom][it.GetIndex()]=original_gradient;//Added by Anupama
                                 //m_ParticleSystem->SetPosition(newpoint, it.GetIndex(), dom);
                                 m_ParticleSystem->SetPositionUpdatePrev(newpoint, it.GetIndex(), pt, dom); //Modified by Anupama

                                newenergy = localGradientFunction->Energy(it.GetIndex(), dom, m_ParticleSystem);

                                if (newenergy < energy) // good move, increase timestep for next time
                                {
                                    meantime[dom] += m_TimeSteps[dom][k];
                                    m_TimeSteps[dom][k] *= factor;
                                    if (m_TimeSteps[dom][k] > maxtime[dom]) m_TimeSteps[dom][k] = maxtime[dom];
                                    if (gradmag > maxchange) maxchange = gradmag;
                                    done = true;
                                }
                                else
                                {// bad move, reset point position and back off on timestep
                                    particleGradients[dom][it.GetIndex()]=null_gradient;//Added by Anupama
                                    if (m_TimeSteps[dom][k] > mintime[dom])
                                    {
                                        dynamic_cast<DomainType *>(m_ParticleSystem->GetDomain(dom))->ApplyConstraints(pt);
                                        m_ParticleSystem->SetPositionUpdatePrev(pt, it.GetIndex(), prev_pos, dom); //Modified by Anupama
                                        //m_ParticleSystem->SetPosition(pt, it.GetIndex(), dom);
                                        m_TimeSteps[dom][k] /= factor;

                                    }
                                    else // keep the move with timestep 1.0 anyway
                                    {
                                        done = true;
                                    }
                                }
                            } //gradmag check
                        } // end while not done

                    } // for each particle

                    // Compute mean time step
                    meantime[dom] /= static_cast<double>(k);

                    if (meantime[dom] < 1.0) meantime[dom] = 1.0;
//                    //        std::cout << "meantime = " << meantime[dom] << std::endl;
                    maxtime[dom] = meantime[dom] + meantime[dom] * 0.2;
                    mintime[dom] = meantime[dom] - meantime[dom] * 0.1;
                } // if not flagged
            }// for each domain
     //   }

           /*Below block Added by Anupama*/
       double maxdtDx=0.1;
       double mintimeDxval=0.0000001;
       double maxtimeDxval=1.0e30;
       const double factorDx = 1.1;//1.1;

       for (int dom = 0; dom < numdomains; dom++)
        {
            meantimeDx[dom]=0.0;

            unsigned int k = 0;
            typename ParticleSystemType::PointContainerType::ConstIterator endit =
                    m_ParticleSystem->GetPositions(dom)->GetEnd();
            for (typename ParticleSystemType::PointContainerType::ConstIterator it
                 = m_ParticleSystem->GetPositions(dom)->GetBegin(); it != endit; it++, k++)
            {
               bool doneDx=false;
               VectorType gradXtilda1=particleGradients[dom][it.GetIndex()];
               if(gradXtilda1[0]!=0 || gradXtilda1[1]!=0 || gradXtilda1[2]!=0)
                {
               double maxdt;
               double energy=0.0;
               typename GradientFunctionType::Pointer localGradientFunctionDx;
               localGradientFunctionDx = m_GradientFunction;
               localGradientFunctionDx->SetDomainNumber(dom);
               localGradientFunctionDx->BeforeEvaluate(it.GetIndex(), dom, m_ParticleSystem);
               VectorType gradXtilda=localGradientFunctionDx->Evaluate(it.GetIndex(), dom, m_ParticleSystem, maxdt, energy);
               //localGradientFunctionDx->RecomputeCovMatrix();
               PointType pos = m_ParticleSystem->GetPosition(it.GetIndex(), dom);
               PointType prev_pos = m_ParticleSystem->GetPrevPosition(it.GetIndex(), dom);
               //energy = localGradientFunctionDx->Energy(it.GetIndex(), dom, m_ParticleSystem);
               double prev_offset = m_ParticleSystem->GetPrevOffset(it.GetIndex(), dom);
               double offset = m_ParticleSystem->GetOffset(it.GetIndex(), dom);
               double newoffset=0.0;
               double gradmagDx=0.0;
               gradmagDx =this->calculateOffsetGradient(pos, prev_pos, gradXtilda, offset, prev_offset);
               //double offset_smoothness = localGradientFunctionDx->OffsetSmoothness(it.GetIndex(), dom, m_ParticleSystem);
               //gradmagDx=gradmagDx;//+offset_smoothness;


               while(!doneDx)
               {
                        newoffset=offset-(gradmagDx* m_TimeStepsDx[dom][it.GetIndex()]);
                        m_ParticleSystem->setOffset(it.GetIndex(), dom, newoffset);
                        m_ParticleSystem->setPrevOffset(it.GetIndex(), dom, offset);
                        double newenergyDx = localGradientFunctionDx->Energy(it.GetIndex(), dom, m_ParticleSystem);

                        if(newenergyDx < energy)
                        {
                            meantimeDx[dom] += m_TimeStepsDx[dom][k];
                            m_TimeStepsDx[dom][k]*=factorDx;
                            if(m_TimeStepsDx[dom][k] > maxtimeDxval)
                                m_TimeStepsDx[dom][k]=maxtimeDxval;
                            doneDx=true;
                        }

                        else
                        {
                            m_ParticleSystem->setOffset(it.GetIndex(), dom, offset);
                            m_ParticleSystem->setPrevOffset(it.GetIndex(), dom, prev_offset);
                            if(m_TimeStepsDx[dom][k] > mintimeDxval)
                            {
                                m_TimeStepsDx[dom][k] /= factorDx;
                            }

                            else
                            {
                                doneDx=true;
                            }
                        }

               }//end of while
               }
            } //end of particle iterations

           /* meantimeDx[dom] /= static_cast<double>(k);

             if (meantimeDx[dom] < 1.0) meantimeDx[dom] = 1.0;
                maxtimeDx[dom] = meantimeDx[dom] + meantimeDx[dom] * 0.2;
                mintimeDx[dom] = meantimeDx[dom] - meantimeDx[dom] * 0.1;*/

        }//end of domains
        /* Block above Added by Anupama*/
         // m_GradientFunction->AfterIteration();

       m_NumberOfIterations++;
       m_GradientFunction->AfterIteration();

       timerAfter = time(NULL);
       double seconds = difftime(timerAfter, timerBefore);

       if (m_verbosity > 2)
       {
           std::cout << m_NumberOfIterations << ". " << seconds << " seconds.. ";
           std::cout.flush();
       }

       this->InvokeEvent(itk::IterationEvent());
       // Check for convergence.  Optimization is considered to have converged if
       // max number of iterations is reached or maximum distance moved by any
       // particle is less than the specified precision.
       //    std::cout << "maxchange = " << maxchange << std::endl;

       if ((m_NumberOfIterations >= m_MaximumNumberOfIterations)
               || (m_Tolerance > 0.0 &&  maxchange <  m_Tolerance))
       {
           m_StopOptimization = true;
       }

    } // end while stop optimization
}

template <class TGradientNumericType, unsigned int VDimension>
double ParticleGradientDescentPositionOptimizer<TGradientNumericType, VDimension>
::calculateOffsetGradient(PointType pos, PointType prev_pos, VectorType gradXtilda, double offset, double prev_offset)
{
VectorType gradDx;
double epsilonval=1.0e-6;
double alpha =1.0e6;
double prodGradients=0.0;
double diffXtilda[3] ={(pos[0]-prev_pos[0]), (pos[1]-prev_pos[1]), (pos[2]-prev_pos[2])};
double diffDeltaX = offset - prev_offset;
double value1=(1/(1+exp(-alpha*offset)));
double value2=(1/(1+exp(alpha*offset)));
double gradmagDx=0.0;

if(diffDeltaX==0)
  diffDeltaX=epsilonval;

for (int i=0; i<=2; i++)
     gradDx[i]=(diffXtilda[i]/diffDeltaX);

for (int i=0; i<=2; i++)
     prodGradients=prodGradients+(gradDx[i]*gradXtilda[i]);

gradmagDx=(value1-value2)+prodGradients;
return gradmagDx;
}
/*** GAUSS SEIDEL ***/
template <class TGradientNumericType, unsigned int VDimension>
void
ParticleGradientDescentPositionOptimizer<TGradientNumericType, VDimension>
::StartGaussSeidelOptimization()
{
//    std::cout << "Starting Gauss Seidel Optimization ..... \n" << std::flush;
    // NOTE: THIS METHOD WILL NOT WORK AS WRITTEN IF PARTICLES ARE
    // ADDED TO THE SYSTEM DURING OPTIMIZATION.
    m_StopOptimization = false;
    m_GradientFunction->SetParticleSystem(m_ParticleSystem);

    VectorType gradient;
    PointType newpoint;

    time_t timerBefore, timerAfter;

    while (m_StopOptimization == false)
    {
        timerBefore = time(NULL);

        m_GradientFunction->BeforeIteration();
        double maxdt;

        // Iterate over each domain
        for (unsigned int dom = 0; dom < m_ParticleSystem->GetNumberOfDomains(); dom++)
        {
            // skip any flagged domains
            if (m_ParticleSystem->GetDomainFlag(dom) == false)
            {
                // Tell function which domain we are working on.
                m_GradientFunction->SetDomainNumber(dom);

                // Iterate over each particle position
                unsigned int k = 0;
                typename ParticleSystemType::PointContainerType::ConstIterator endit =
                        m_ParticleSystem->GetPositions(dom)->GetEnd();
                for (typename ParticleSystemType::PointContainerType::ConstIterator it
                     = m_ParticleSystem->GetPositions(dom)->GetBegin(); it != endit; it++, k++)
                {
                    // Compute gradient update.
                    m_GradientFunction->BeforeEvaluate(it.GetIndex(), dom, m_ParticleSystem);
                    gradient = m_GradientFunction->Evaluate(it.GetIndex(), dom, m_ParticleSystem,
                                                            maxdt);

                    // May modify gradient
                    dynamic_cast<DomainType *>(m_ParticleSystem->GetDomain(dom))
                            ->ApplyVectorConstraints(gradient,
                                                     m_ParticleSystem->GetPosition(it.GetIndex(), dom), maxdt);

                    // Hack to avoid blowing up under certain conditions.
                    if (gradient.magnitude() > maxdt)
                    { gradient = (gradient / gradient.magnitude()) * maxdt; }

                    // Compute particle move based on update.
                    for (unsigned int i = 0; i < VDimension; i++)
                    {
                        newpoint[i] = (*it)[i] - gradient[i] * m_TimeStep;
                    }

                    // Apply update
                    dynamic_cast<DomainType *>(m_ParticleSystem->GetDomain(dom))->ApplyConstraints(newpoint);
                    m_ParticleSystem->SetPosition(newpoint, it.GetIndex(), dom);
                } // for each particle
            } // if not flagged
        }// for each domain

        m_NumberOfIterations++;
        m_GradientFunction->AfterIteration();


        timerAfter = time(NULL);
        double seconds = difftime(timerAfter, timerBefore);

        if (m_verbosity > 2)
        {
            std::cout << m_NumberOfIterations << ". " << seconds << " seconds.. ";
            std::cout.flush();
        }

        this->InvokeEvent(itk::IterationEvent());

        // Check for convergence.  Optimization is considered to have converged if
        // max number of iterations is reached or maximum distance moved by any
        // particle is less than the specified precision.
        if (m_NumberOfIterations >= m_MaximumNumberOfIterations)
        {
            m_StopOptimization = true;
        }

    } // end while

}

template <class TGradientNumericType, unsigned int VDimension>
void
ParticleGradientDescentPositionOptimizer<TGradientNumericType, VDimension>
::StartJacobiOptimization()
{
//    std::cout << "Starting Jacobi Optimization ..... \n" << std::flush;

    // NOTE: THIS METHOD WILL NOT WORK AS WRITTEN IF PARTICLES ARE
    // ADDED TO THE SYSTEM DURING OPTIMIZATION.
    m_StopOptimization = false;

    m_GradientFunction->SetParticleSystem(m_ParticleSystem);

    // Vector for storing updates.
    std::vector<std::vector<PointType> > updates(m_ParticleSystem->GetNumberOfDomains());
    std::vector<std::vector<PointType> > oldupdates(m_ParticleSystem->GetNumberOfDomains()); //Added by Anupama
    std::vector<std::vector<VectorType> > particleGradients(m_ParticleSystem->GetNumberOfDomains()); //Added by Anupama

    for (unsigned int d = 0; d < m_ParticleSystem->GetNumberOfDomains(); d++)
    {
        updates[d].resize(m_ParticleSystem->GetPositions(d)->GetSize());
        oldupdates[d].resize(m_ParticleSystem->GetPositions(d)->GetSize()); //Added by Anupama
        particleGradients[d].resize(m_ParticleSystem->GetPositions(d)->GetSize()); //Added by Anupama
    }
    VectorType gradient;
    VectorType newGradient;
    PointType  newpoint;
    PointType  oldpoint; //Added by Anupama
    time_t timerBefore, timerAfter;

    while (m_StopOptimization == false)
    {
        timerBefore = time(NULL);

        m_GradientFunction->BeforeIteration();
        double maxdt;

        // Iterate over each domain
        for (unsigned int dom = 0; dom < m_ParticleSystem->GetNumberOfDomains(); dom++)
        {
            // skip any flagged domains
            if (m_ParticleSystem->GetDomainFlag(dom) == false)
            {
                // Tell function which domain we are working on.
                m_GradientFunction->SetDomainNumber(dom);

                // Iterate over each particle position
                unsigned int k = 0;
                typename ParticleSystemType::PointContainerType::ConstIterator endit =
                        m_ParticleSystem->GetPositions(dom)->GetEnd();
                for (typename ParticleSystemType::PointContainerType::ConstIterator it
                     = m_ParticleSystem->GetPositions(dom)->GetBegin(); it != endit; it++, k++)
                {
                    m_GradientFunction->BeforeEvaluate(it.GetIndex(), dom, m_ParticleSystem);
                    gradient = m_GradientFunction->Evaluate(it.GetIndex(), dom, m_ParticleSystem,
                                                            maxdt);

                    /*Below block Added by Anupama*/

                    PointType pos = m_ParticleSystem->GetPosition(it.GetIndex(), dom);
                    PointType prev_pos = m_ParticleSystem->GetPrevPosition(it.GetIndex(), dom);
                    double prev_offset = m_ParticleSystem->GetPrevOffset(it.GetIndex(), dom);
                    double offset = m_ParticleSystem->GetOffset(it.GetIndex(), dom);
                    VectorType gradDx;
                    double epsilonval=1.0e-6;
                    double value1=(1/(1+exp(-1000000*offset)));
                    double value2=(1/(1+exp(1000000*offset)));
                    double diffXtilda[3] ={(pos[0]-prev_pos[0]), (pos[1]-prev_pos[1]), (pos[2]-prev_pos[2])};
                    double diffDeltaX = offset - prev_offset;

                    for (int i=0; i<=2; i++)
                      {
                          if(diffXtilda[i]==0)
                             diffXtilda[i]=epsilonval;
                      }

                    for (int i=0; i<=2; i++)
                        gradDx[i]=(diffDeltaX/diffXtilda[i])*(value1-value2);

                    for (int i=0; i<=2; i++)
                        newGradient[i]=gradient[i]+gradDx[i];

                       /*Above block Added by Anupama*/


                    // May modify gradient
                    dynamic_cast<DomainType *>(m_ParticleSystem->GetDomain(dom))
                            ->ApplyVectorConstraints(newGradient, m_ParticleSystem->GetPosition(it.GetIndex(), dom), maxdt); //Modified by Anupama

                    // Hack to avoid blowing up under certain conditions.
                    if (gradient.magnitude() > maxdt)
                    {
                        gradient = (gradient / gradient.magnitude()) * maxdt;
                    }
                    if (newGradient.magnitude() > maxdt) //Added by Anupama
                          newGradient = (newGradient / newGradient.magnitude()) * maxdt; //Added by Anupama

                    // Compute particle move based on update.
                    for (unsigned int i = 0; i < VDimension; i++)
                    {
                        oldpoint[i]=(*it)[i]; //Added by Anupama
                        newpoint[i] = (*it)[i] - newGradient[i] * m_TimeStep;

                    }

                    // Store update
                    updates[dom][k] = newpoint;
                    oldupdates[dom][k]=oldpoint;//Added by Anupama
                    particleGradients[dom][k]=gradient;//Added by Anupama

                } // for each particle
            } // if not flagged
        }// for each domain

        for (unsigned int dom = 0; dom < m_ParticleSystem->GetNumberOfDomains(); dom++)
        {
            // skip any flagged domains
            if (m_ParticleSystem->GetDomainFlag(dom) == false)
            {
                unsigned int k = 0;
                typename ParticleSystemType::PointContainerType::ConstIterator endit =
                        m_ParticleSystem->GetPositions(dom)->GetEnd();
                for (typename ParticleSystemType::PointContainerType::ConstIterator it
                     = m_ParticleSystem->GetPositions(dom)->GetBegin();  it != endit; it++, k++)
                {

                    dynamic_cast<DomainType *>(m_ParticleSystem->GetDomain(dom))->ApplyConstraints(updates[dom][k]);
                    m_ParticleSystem->SetPositionUpdatePrev(updates[dom][k], it.GetIndex(), oldupdates[dom][k], dom); //Modified by Anupama

                    /*Below block Added by Anupama*/
                   PointType pos = m_ParticleSystem->GetPosition(k, dom);
                   PointType prev_pos = m_ParticleSystem->GetPrevPosition(k, dom);
                   VectorType gradDx;
                   VectorType gradXtilda=particleGradients[dom][k];
                   double prev_offset = m_ParticleSystem->GetPrevOffset(k, dom);
                   double offset = m_ParticleSystem->GetOffset(k, dom);
                   double epsilonval=1.0e-6                                                                         ;
                   double prodGradients=0.0;
                   double diffXtilda[3] ={(pos[0]-prev_pos[0]), (pos[1]-prev_pos[1]), (pos[2]-prev_pos[2])};
                   double diffDeltaX = offset - prev_offset;
                   double value1=(1/(1+exp(-1000000*offset)));
                   double value2=(1/(1+exp(1000000*offset)));
                   double newoffset=0.0;

                   if(diffDeltaX==0)
                     diffDeltaX=epsilonval;

                   for (int i=0; i<=2; i++)
                        gradDx[i]=(diffXtilda[i]/diffDeltaX);

                   for (int i=0; i<=2; i++)
                        prodGradients=prodGradients+(gradDx[i]*gradXtilda[i]);

                      newoffset=offset-(((value1-value2)+prodGradients)*m_TimeStep);
                      m_ParticleSystem->setOffset(k, dom, newoffset);
                      m_ParticleSystem->setPrevOffset(k, dom, offset);

                    /* Block above Added by Anupama*/

                } // for each particle
            } // if not flagged
        } // for each domain

        m_NumberOfIterations++;
        m_GradientFunction->AfterIteration();


        timerAfter = time(NULL);
        double seconds = difftime(timerAfter, timerBefore);

        if (m_verbosity > 2)
        {
            std::cout << m_NumberOfIterations << " ";
            std::cout.flush();
        }

        this->InvokeEvent(itk::IterationEvent());

        // Check for convergence.  Optimization is considered to have converged if
        // max number of iterations is reached or maximum distance moved by any
        // particle is less than the specified precision.
        if (m_NumberOfIterations >= m_MaximumNumberOfIterations)
        {
            m_StopOptimization = true;
        }

    } // end while

}

} // end namespace

#endif
