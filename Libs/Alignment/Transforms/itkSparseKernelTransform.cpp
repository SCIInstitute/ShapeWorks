/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSparseKernelTransform.txx,v $
  Language:  C++
  Date:      $Date: 2006-11-28 14:22:18 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkSparseKernelTransform_txx
#define _itkSparseKernelTransform_txx
#include "itkSparseKernelTransform.h"

// Report timings
#include <itkTimeProbe.h>
#include <itkTimeProbesCollectorBase.h>

#include <itkeigen/Eigen/Sparse>
#include <itkeigen/Eigen/SparseLU>

#include <vector>

namespace itk
{


/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
SparseKernelTransform<TScalarType, NDimensions>::
SparseKernelTransform()
  : Transform<TScalarType, NDimensions,NDimensions>(1)
//:Superclass(
                         //   NDimensions,
                           // NDimensions )
  // the second NDimensions is associated is provided as
  // a tentative number for initializing the Jacobian.
  // The matrix can be resized at run time so this number
  // here is irrelevant. The correct size of the Jacobian
  // will be NDimension X NDimension.NumberOfLandMarks.
{

    // m_I.set_identity();
    m_I               = IMatrixType::Identity();
    m_SourceLandmarks = PointSetType::New();
    m_TargetLandmarks = PointSetType::New();
    m_Displacements   = VectorSetType::New();
    m_WMatrixComputed = false;

    m_LMatrixComputed  = false;
    m_LInverseComputed = false;

    m_Stiffness = 0.0;

    Eigen::setNbThreads(8);
}

/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
SparseKernelTransform<TScalarType, NDimensions>::
~SparseKernelTransform()
{
}

template<class TScalarType, unsigned int NDimensions>
inline void SparseKernelTransform<TScalarType,
  NDimensions>::ComputeJacobianWithRespectToParameters(
    const InputPointType & in, JacobianType & jacobian) const {}

template <class TScalarType, unsigned int NDimensions>
void
SparseKernelTransform<TScalarType, NDimensions>::
SetSourceLandmarks(PointSetType * landmarks)
{
    itkDebugMacro("setting SourceLandmarks to " << landmarks );
    if (this->m_SourceLandmarks != landmarks)
    {
        this->m_SourceLandmarks = landmarks;
        this->UpdateParameters();
        this->Modified();

        // these are invalidated when the source lms change
        m_WMatrixComputed  = false;
        m_LMatrixComputed  = false;
        m_LInverseComputed = false;

        // you must recompute L and Linv - this does not require the targ lms
        // Linverse is only needed ofr Jacobian computation, I will defer this in case Jacobian is needed
        // we will assume by default that this transform is used only for warping, if it is a part of optimization
        // the GetJacobian will feel that the inverse is not computed and will compute it
        //this->ComputeLInverse();
        this->ComputeL();

    }
}


/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void
SparseKernelTransform<TScalarType, NDimensions>::
SetTargetLandmarks(PointSetType * landmarks)
{
    itkDebugMacro("setting TargetLandmarks to " << landmarks );
    if (this->m_TargetLandmarks != landmarks)
    {
        this->m_TargetLandmarks = landmarks;
        // this is invalidated when the target lms change
        m_WMatrixComputed=false;
        this->ComputeWMatrix();
        this->UpdateParameters();
        this->Modified();
    }

}



/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
const typename SparseKernelTransform<TScalarType, NDimensions>::GMatrixType &
SparseKernelTransform<TScalarType, NDimensions>::
ComputeG( const InputVectorType & ) const
{
    //
    // Should an Exception be thrown here  ?
    //
    itkWarningMacro(<< "ComputeG() should be reimplemented in the subclass !!");
    return m_GMatrix;
}

/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
const typename SparseKernelTransform<TScalarType, NDimensions>::GMatrixType &
SparseKernelTransform<TScalarType, NDimensions>::
ComputeReflexiveG( PointsIterator ) const
{
    m_GMatrix = GMatrixType::Zero();
    for(unsigned d = 0; d < NDimensions; d++)
        m_GMatrix(d,d) = m_Stiffness;

    //m_GMatrix.fill( NumericTraits< TScalarType >::Zero );
    //m_GMatrix.fill_diagonal( m_Stiffness );

    return m_GMatrix;
}




/**
 * Default implementation of the the method. This can be overloaded
 * in transforms whose kernel produce diagonal G matrices.
 */
template <class TScalarType, unsigned int NDimensions>
void
SparseKernelTransform<TScalarType, NDimensions>::
ComputeDeformationContribution( const InputPointType  & thisPoint,
                                OutputPointType & result     ) const
{

    unsigned long numberOfLandmarks = m_SourceLandmarks->GetNumberOfPoints();

    PointsIterator sp  = m_SourceLandmarks->GetPoints()->Begin();

    for(unsigned int lnd=0; lnd < numberOfLandmarks; lnd++ )
    {
        const GMatrixType & Gmatrix = ComputeG( thisPoint - sp->Value() );
        for(unsigned int dim=0; dim < NDimensions; dim++ )
        {
            for(unsigned int odim=0; odim < NDimensions; odim++ )
            {
                result[ odim ] += Gmatrix(dim, odim ) * m_DMatrix(dim,lnd);
            }
        }
        ++sp;
    }

}


/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void SparseKernelTransform<TScalarType, NDimensions>
::ComputeD(void) const
{
    unsigned long numberOfLandmarks = m_SourceLandmarks->GetNumberOfPoints();

    PointsIterator sp  = m_SourceLandmarks->GetPoints()->Begin();
    PointsIterator tp  = m_TargetLandmarks->GetPoints()->Begin();
    PointsIterator end = m_SourceLandmarks->GetPoints()->End();

    m_Displacements->Reserve( numberOfLandmarks );
    typename VectorSetType::Iterator vt = m_Displacements->Begin();

    while( sp != end )
    {
        vt->Value() = tp->Value() - sp->Value();
        vt++;
        sp++;
        tp++;
    }
    //	std::cout<<" Computed displacements "<<m_Displacements<<std::endl;
}

/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void SparseKernelTransform<TScalarType, NDimensions>
::ComputeWMatrix(void) const
{
    itk::TimeProbe clock;

    //	std::cout<<"Computing W matrix"<<std::endl;

    //typedef vnl_svd<TScalarType>  SVDSolverType;

    if(!m_LMatrixComputed) {
        this->ComputeL();
    }
    this->ComputeY();

    //SVDSolverType svd( m_LMatrix, 1e-8 );
    //m_WMatrix = svd.solve( m_YMatrix );

    clock.Start();

    //Eigen::BiCGSTAB<LMatrixType>  solver;
    Eigen::BiCGSTAB<LMatrixType, Eigen::IncompleteLUT<double> >  solver;
    solver.preconditioner().setDroptol(1e-10);
    solver.preconditioner().setFillfactor(1000);

    //    Eigen::SparseLU<LMatrixType> solver;
    solver.compute(m_LMatrix);

    if(solver.info()!= Eigen::Success) {
        // decomposition failed
        std::cerr << "LMatrix failed to decompose ...!" << std::endl;
        return;
    }

    unsigned long numberOfLandmarks = m_SourceLandmarks->GetNumberOfPoints();
    m_WMatrix = WMatrixType::Zero(NDimensions*(numberOfLandmarks+NDimensions+1), 1);
    m_WMatrix = solver.solve(m_YMatrix);

    //    m_WMatrix = WMatrixType::Random(NDimensions*(numberOfLandmarks+NDimensions+1), 1);
    //    solver.setMaxIterations(1);
    //    int i = 0;
    //    do {
    //        m_WMatrix = solver.solveWithGuess(m_YMatrix,m_WMatrix);
    //        std::cout << "#iteration: " << i << " " << "estimated error: " << solver.error() << std::endl;
    //        ++i;
    //    } while (solver.info()!= Eigen::Success && i<100);

    std::cout << "#iterations: " << solver.iterations() << std::endl;
    std::cout << "estimated error: " << solver.error() << std::endl;

    std::cout  << solver.error() << std::endl;
    if(solver.info() != Eigen::Success) {
        // solving failed
        std::cerr << "solving sparse system failed ...!" << std::endl;
        return;
    }

    clock.Stop();
    std::cout << "Computing Wmatrix:" << std::endl;
    std::cout << "Mean: " << clock.GetMean() << std::endl;
    std::cout << "Total: " << clock.GetTotal() << std::endl;

    this->ReorganizeW();
    m_WMatrixComputed=true;
}

/**
 * postponed till needing the jacobian for this class
 */
//template <class TScalarType, unsigned int NDimensions>
//void SparseKernelTransform<TScalarType, NDimensions>::
//ComputeLInverse(void) const
//{
//    // Assumes that L has already been computed
//    // Necessary for the jacobian
//    if(!m_LMatrixComputed) {
//        this->ComputeL();
//    }
//    //std::cout<<"LMatrix is:"<<std::endl;
//    //std::cout<<m_LMatrix<<std::endl;

//    itk::TimeProbesCollectorBase timeCollector;
//    if (0){
//        timeCollector.Start( "ComputeLInverse" );
//        m_LMatrixInverse=vnl_matrix_inverse<TScalarType> (m_LMatrix);
//        timeCollector.Stop( "ComputeLInverse" );
//    }

//    // Convert to sparse matrix
//    // Because of the special storage scheme of a SparseMatrix, special care has to be taken when adding new nonzero entries.
//    // For instance, the cost of a single purely random insertion into a SparseMatrix is O(nnz),
//    // where nnz is the current number of non-zero coefficients.
//    // The simplest way to create a sparse matrix while guaranteeing good performance is thus to first build a list of
//    // so-called triplets, and then convert it to a SparseMatrix.
//    typedef Eigen::SparseMatrix<ScalarType> SpMat; // declares a column-major sparse matrix type of double
//    typedef Eigen::Triplet<ScalarType> Triplet;
//    SpMat A(m_LMatrix.rows(),m_LMatrix.cols());

//    std::vector<Triplet> tripletList;
//    for ( unsigned int r = 0; r < m_LMatrix.rows(); r++ )
//        for ( unsigned int c = 0; c < m_LMatrix.cols(); c++ )
//        {
//            ScalarType val = m_LMatrix.get( r, c );
//            if ( val != 0 )
//                tripletList.push_back(Triplet(r,c,val));
//        }
//    A.setFromTriplets(tripletList.begin(), tripletList.end());
//    timeCollector.Stop( "ConvertLToSparse" );

//    // Method 4: LU Decomposition
//    // Depends on local ITK vnl_sparse_lu modification
//    timeCollector.Start( "ComputeLSparseInverse" );
//    unsigned long numberOfLandmarks = m_SourceLandmarks->GetNumberOfPoints();
//    //    vnl_sparse_symmetric_eigensystem eigSys;
//    //    eigSys.CalculateNPairs(lSparseMatrix,NDimensions*(numberOfLandmarks+NDimensions+1), false);
//    //    //LSparseMatrixType lMatrixInverse = vnl_sparse_lu( lSparseMatrix ).inverse();
//    //    timeCollector.Stop( "ComputeLSparseInverse" );


//    m_LInverseComputed=true;
//    //std::cout<<"LMatrix inverse is:"<<std::endl;
//    //std::cout<<m_LMatrixInverse<<std::endl;
//}


/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void SparseKernelTransform<TScalarType, NDimensions>::
ComputeL(void) const
{
    unsigned long numberOfLandmarks = m_SourceLandmarks->GetNumberOfPoints();
    //vnl_matrix<TScalarType> O2(NDimensions*(NDimensions+1),
    //                           NDimensions*(NDimensions+1), 0);

    this->ComputeP();
    this->ComputeK();

    //m_LMatrix.set_size( NDimensions*(numberOfLandmarks+NDimensions+1),
    //                    NDimensions*(numberOfLandmarks+NDimensions+1) );
    //m_LMatrix.fill( 0.0 );
    m_LMatrix = LMatrixType( NDimensions*(numberOfLandmarks+NDimensions+1),
                             NDimensions*(numberOfLandmarks+NDimensions+1) );

    //std::vector<TripletType> tripletList;

    // putting KMATRIX
    //m_LMatrix.update( m_KMatrix, 0, 0 );
    //it.value();
    //it.row(); // row index
    //it.col(); // col index (here it is equal to k)
    //it.index(); // inner index, here it is equal to it.row()
    for (int k = 0; k < m_KMatrix.outerSize(); ++k) // column index
        for (typename KMatrixType::InnerIterator it(m_KMatrix,k); it; ++it)
            m_LMatrix.insert(it.row(), it.col()) = it.value() ;
    //tripletList.push_back( TripletType ( it.row(), it.col(), it.value() ) );

    // putting PMATRIX - will keep the lower only ??
    //m_LMatrix.update( m_PMatrix, 0, m_KMatrix.columns() );
    //m_LMatrix.update( m_PMatrix.transpose(), m_KMatrix.rows(), 0);
    for (int p = 0; p < m_PMatrix.outerSize(); ++p) // column index
        for (typename PMatrixType::InnerIterator it(m_PMatrix,p); it; ++it)
        {
            // fill P -> upper
            m_LMatrix.insert(it.row(), NDimensions*numberOfLandmarks + it.col()) = it.value() ;
            //tripletList.push_back( TripletType ( it.row(), NDimensions*numberOfLandmarks + it.col(), it.value() ) );

            // fill P.transpose -> lower
            m_LMatrix.insert(NDimensions*numberOfLandmarks + it.col(),  it.row()) = it.value() ;
            //tripletList.push_back( TripletType ( NDimensions*numberOfLandmarks + it.col(),  it.row(), it.value() ) );
        }
    //m_LMatrix.update( O2, m_KMatrix.rows(), m_KMatrix.columns());

    //    // shireen: for debugging let's make sure that the L matrix is sparse (based on gaussian basis)
    //    std::ofstream ofs;
    //    ofs.open("Lsparse.csv");
    //    for (int k = 0; k < m_LMatrix.outerSize(); ++k) // column index
    //        for (typename LMatrixType::InnerIterator it(m_LMatrix,k); it; ++it)
    //            ofs << it.row()  << ", " << it.col()  << ", " << it.value() << std::endl;
    //    ofs.close();


    std::cout << "Lmatrix - nnz = "  << m_LMatrix.nonZeros() << std::endl;
    m_LMatrix.makeCompressed();

    //m_LMatrix.setFromTriplets(tripletList.begin(), tripletList.end());
    m_LMatrixComputed=1;
}


/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void SparseKernelTransform<TScalarType, NDimensions>::
ComputeK(void) const
{
    unsigned long numberOfLandmarks = m_SourceLandmarks->GetNumberOfPoints();
    GMatrixType G;
    //std::vector<TripletType> tripletList;

    m_KMatrix = KMatrixType( NDimensions * numberOfLandmarks,
                             NDimensions * numberOfLandmarks );
    //m_KMatrix.set_size( NDimensions * numberOfLandmarks,
    //                    NDimensions * numberOfLandmarks );

    //m_KMatrix.fill( 0.0 );

    PointsIterator p1  = m_SourceLandmarks->GetPoints()->Begin();
    PointsIterator end = m_SourceLandmarks->GetPoints()->End();

    // K matrix is symmetric, so only evaluate the upper triangle and
    // store the values in both the upper and lower triangle
    unsigned int i = 0;
    while( p1 != end )
    {
        PointsIterator p2 = p1; // start at the diagonal element
        unsigned int j = i;

        // Compute the block diagonal element, i.e. kernel for pi->pi
        //G = ComputeReflexiveG(p1);

        // force to compute the basis on the diagonal
        const InputVectorType s = p1.Value() - p1.Value();
        G = ComputeG(s); // the basis

        //m_KMatrix.update(G, i*NDimensions, i*NDimensions);
        for(unsigned int d = 0; d < NDimensions; d++)
        {
            if(G(d,d) != 0 )
                m_KMatrix.insert(i*NDimensions+d ,i*NDimensions+d) = G(d,d) + m_Stiffness; // this is as a regularizer
            //tripletList.push_back(TripletType(i*NDimensions+d ,i*NDimensions+d, m_GMatrix(d,d)));
        }

        p2++;
        j++;

        // Compute the upper (and copy into lower) triangular part of K
        // only save the lower part, don't need it
        while( p2 != end )
        {
            const InputVectorType s = p1.Value() - p2.Value();
            G = ComputeG(s); // the basis

            // write value in upper and lower triangle of matrix
            for(unsigned int ii = 0 ; ii < NDimensions; ii++)
                for(unsigned int jj = 0 ; jj < NDimensions; jj++)
                {
                    if (G(ii,jj) != 0)
                    {
                        m_KMatrix.insert(i*NDimensions+ii ,j*NDimensions+jj) = G(ii,jj); // upper
                        m_KMatrix.insert(j*NDimensions+ii ,i*NDimensions+jj) = G(ii,jj); // lower
                    }

                    //tripletList.push_back(TripletType(i*NDimensions+ii ,j*NDimensions+jj, G(ii,jj)));
                    //tripletList.push_back(TripletType(j*NDimensions+ii ,i*NDimensions+jj, G(ii,jj)));
                }

            // m_KMatrix.update(G, i*NDimensions, j*NDimensions);
            // m_KMatrix.update(G, j*NDimensions, i*NDimensions);
            p2++;
            j++;
        }
        p1++;
        i++;
    }
    //std::cout<<"K matrix: "<<std::endl;
    //std::cout<<m_KMatrix<<std::endl;

    //    // shireen: for debugging let's make sure that the L matrix is sparse (based on gaussian basis)
    //    std::ofstream ofs;
    //    ofs.open("Ksparse.csv");
    //    for (int k = 0; k < m_KMatrix.outerSize(); ++k) // column index
    //        for (typename KMatrixType::InnerIterator it(m_KMatrix,k); it; ++it)
    //            ofs << it.row()  << ", " << it.col()  << ", " << it.value() << std::endl;
    //    ofs.close();

    std::cout << "Kmatrix - nnz = "  << m_KMatrix.nonZeros() << std::endl;
    m_KMatrix.makeCompressed();

    //m_KMatrix.setFromTriplets(tripletList.begin(), tripletList.end());
}



/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void SparseKernelTransform<TScalarType, NDimensions>::
ComputeP() const
{
    unsigned long numberOfLandmarks = m_SourceLandmarks->GetNumberOfPoints();

    //IMatrixType I = IMatrixType::Identity();
    //IMatrixType temp;
    InputPointType p;

    //I.set_identity();

    //std::vector<TripletType> tripletList;

    //m_PMatrix.set_size( NDimensions*numberOfLandmarks,
    //                    NDimensions*(NDimensions+1) );
    //m_PMatrix.fill( 0.0 );
    m_PMatrix = PMatrixType(NDimensions*numberOfLandmarks,
                            NDimensions*(NDimensions+1) );
    for (unsigned int i = 0; i < numberOfLandmarks; i++)
    {
        m_SourceLandmarks->GetPoint(i, &p);
        for (unsigned int j = 0; j < NDimensions; j++)
        {
            //temp = I * p[j];
            for(unsigned int d = 0 ; d < NDimensions; d++)
            {
                m_PMatrix.insert(i*NDimensions + d, j*NDimensions + d) = p[j];
                //tripletList.push_back( TripletType( i*NDimensions + d, j*NDimensions + d, p[j] ) );
            }
            //m_PMatrix.update(temp, i*NDimensions, j*NDimensions);
        }

        for(unsigned int d = 0 ; d < NDimensions; d++)
        {
            m_PMatrix.insert(i*NDimensions + d, NDimensions*NDimensions + d) = 1;
            //tripletList.push_back( TripletType( i*NDimensions + d, NDimensions*NDimensions + d, 1 ) );
        }
        //m_PMatrix.update(I, i*NDimensions, NDimensions*NDimensions);
    }

    std::cout << "Pmatrix - nnz = "  << m_PMatrix.nonZeros() << std::endl;
    m_PMatrix.makeCompressed();
    //m_PMatrix.setFromTriplets(tripletList.begin(), tripletList.end());
}



/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void SparseKernelTransform<TScalarType, NDimensions>::
ComputeY(void) const
{
    unsigned long numberOfLandmarks = m_SourceLandmarks->GetNumberOfPoints();

    this->ComputeD();

    typename VectorSetType::ConstIterator displacement =
            m_Displacements->Begin();

    //m_YMatrix.set_size( NDimensions*(numberOfLandmarks+NDimensions+1), 1);
    //m_YMatrix.fill( 0.0 );
    m_YMatrix = YMatrixType::Zero(NDimensions*(numberOfLandmarks+NDimensions+1), 1);
    
    for (unsigned int i = 0; i < numberOfLandmarks; i++)
    {
        for (unsigned int j = 0; j < NDimensions; j++)
        {
            m_YMatrix(i*NDimensions+j, 0) = displacement.Value()[j];
            //m_YMatrix.put(i*NDimensions+j, 0, displacement.Value()[j]);
        }
        displacement++;
    }

    for (unsigned int i = 0; i < NDimensions*(NDimensions+1); i++)
    {
        m_YMatrix(numberOfLandmarks*NDimensions+i, 0) = 0;
        //m_YMatrix.put(numberOfLandmarks*NDimensions+i, 0, 0);
    }
}


/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
void
SparseKernelTransform<TScalarType, NDimensions>
::ReorganizeW(void) const
{
    unsigned long numberOfLandmarks = m_SourceLandmarks->GetNumberOfPoints();

    // The deformable (non-affine) part of the registration goes here
    m_DMatrix = DMatrixType::Zero(NDimensions,numberOfLandmarks);
    //m_DMatrix.set_size(NDimensions,numberOfLandmarks);

    unsigned int ci = 0;
    for(unsigned int lnd=0; lnd < numberOfLandmarks; lnd++ )
    {
        for(unsigned int dim=0; dim < NDimensions; dim++ )
        {
            //std::cout << m_WMatrix(ci,0) << std::endl;
            m_DMatrix(dim,lnd) = m_WMatrix(ci++,0);
        }
    }

    // This matrix holds the rotational part of the Affine component
    m_AMatrix = AMatrixType::Zero(NDimensions, NDimensions);
    for(unsigned int j=0; j < NDimensions; j++ )
    {
        for(unsigned int i=0; i < NDimensions; i++ )
        {
            m_AMatrix(i,j) = m_WMatrix(ci++,0);
        }
    }

    // This vector holds the translational part of the Affine component
    m_BVector = BMatrixType::Zero(NDimensions,1);
    for(unsigned int k=0; k < NDimensions; k++ )
    {
        m_BVector(k) = m_WMatrix(ci++,0);
    }

    // release WMatrix memory by assigning a small one.
    m_WMatrix = WMatrixType(1,1);

    m_WMatrixComputed=1;

    //    std::ofstream ofs;
    //    ofs.open("D.csv");
    //    for (int k = 0; k < m_DMatrix.outerSize(); ++k) // column index
    //        for (typename DMatrixType::InnerIterator it(m_DMatrix,k); it; ++it)
    //            ofs << it.row()  << ", " << it.col()  << ", " << it.value() << std::endl;
    //    ofs.close();
}



/**
 *
 */
template <class TScalarType, unsigned int NDimensions>
typename SparseKernelTransform<TScalarType, NDimensions>::OutputPointType
SparseKernelTransform<TScalarType, NDimensions>
::TransformPoint(const InputPointType& thisPoint) const
{

    OutputPointType result;

    typedef typename OutputPointType::ValueType ValueType;

    result.Fill( NumericTraits< ValueType >::Zero );

    this->ComputeDeformationContribution( thisPoint, result );

    // Add the rotational part of the Affine component
    for(unsigned int j=0; j < NDimensions; j++ )
    {
        for(unsigned int i=0; i < NDimensions; i++ )
        {
            result[i] += m_AMatrix(i,j) * thisPoint[j];
        }
    }



    // This vector holds the translational part of the Affine component
    for(unsigned int k=0; k < NDimensions; k++ )
    {
        result[k] += m_BVector(k) + thisPoint[k];
    }

    return result;

}




//// Compute the Jacobian in one position - POSTPONED
//template <class TScalarType, unsigned int NDimensions>
//const typename SparseKernelTransform<TScalarType,NDimensions>::JacobianType &
//SparseKernelTransform< TScalarType,NDimensions>::
//GetJacobian( const InputPointType & thisPoint) const
//{
//    if(!m_LInverseComputed) {
//        this->ComputeLInverse();
//    }
//    unsigned long numberOfLandmarks = m_SourceLandmarks->GetNumberOfPoints();
//    Superclass::m_Jacobian.SetSize(NDimensions, numberOfLandmarks*NDimensions);
//    Superclass::m_Jacobian.Fill( 0.0 );

//    PointsIterator sp  = m_SourceLandmarks->GetPoints()->Begin();
//    for(unsigned int lnd=0; lnd < numberOfLandmarks; lnd++ )
//    {
//        const GMatrixType & Gmatrix = ComputeG( thisPoint - sp->Value() );
//        ///std::cout<<"G for landmark "<<lnd<<std::endl<<Gmatrix<<std::endl;
//        for(unsigned int dim=0; dim < NDimensions; dim++ )
//        {
//            for(unsigned int odim=0; odim < NDimensions; odim++ )
//            {
//                for(unsigned int lidx=0; lidx < numberOfLandmarks*NDimensions; lidx++ )
//                {
//                    Superclass::m_Jacobian[ odim ] [lidx] += Gmatrix(dim, odim ) *
//                            m_LMatrixInverse[lnd*NDimensions+dim][lidx];
//                }
//            }
//        }
//        ++sp;

//    }
//    for(unsigned int odim=0; odim < NDimensions; odim++ )
//    {
//        for(unsigned int lidx=0; lidx < numberOfLandmarks*NDimensions; lidx++ )
//        {
//            for(unsigned int dim=0; dim < NDimensions; dim++ )
//            {
//                Superclass::m_Jacobian[ odim ] [lidx] += thisPoint[dim] *
//                        m_LMatrixInverse[(numberOfLandmarks+dim)*NDimensions+odim][lidx];
//            }

//            Superclass::m_Jacobian[ odim ] [lidx] += m_LMatrixInverse[(numberOfLandmarks+NDimensions)*NDimensions+odim][lidx];
//        }
//    }

//    return Superclass::m_Jacobian;

//}

// Set to the identity transform - ie make the  Source and target lm the same
template <class TScalarType, unsigned int NDimensions>
void
SparseKernelTransform<TScalarType, NDimensions>::
SetIdentity()
{
    this->SetParameters(this->GetFixedParameters());
}

// Set the parameters
// NOTE that in this transformation both the Source and Target
// landmarks could be considered as parameters. It is assumed
// here that the Target landmarks are provided by the user and
// are not changed during the optimization process required for
// registration.
template <class TScalarType, unsigned int NDimensions>
void
SparseKernelTransform<TScalarType, NDimensions>::
SetParameters( const ParametersType & parameters )
{
    //	std::cout<<"Setting parameters to "<<parameters<<std::endl;
    typename PointsContainer::Pointer landmarks = PointsContainer::New();
    const unsigned int numberOfLandmarks =  parameters.Size() / NDimensions;
    landmarks->Reserve( numberOfLandmarks );

    PointsIterator itr = landmarks->Begin();
    PointsIterator end = landmarks->End();

    InputPointType  landMark;

    unsigned int pcounter = 0;
    while( itr != end )
    {
        for(unsigned int dim=0; dim<NDimensions; dim++)
        {
            landMark[ dim ] = parameters[ pcounter ];
            pcounter++;
        }
        itr.Value() = landMark;
        itr++;
    }

    // m_SourceLandmarks->SetPoints( landmarks );
    m_TargetLandmarks->SetPoints( landmarks );

    // W MUST be recomputed if the target lms are set
    this->ComputeWMatrix();

    //  if(!m_LInverseComputed) {
    //  this->ComputeLInverse();
    //  }

    // Modified is always called since we just have a pointer to the
    // parameters and cannot know if the parameters have changed.
    this->Modified();

}

// Set the fixed parameters
// Since the API of the SetParameters() function sets the
// source landmarks, this function was added to support the
// setting of the target landmarks, and allowing the Transform
// I/O mechanism to be supported.
template <class TScalarType, unsigned int NDimensions>
void
SparseKernelTransform<TScalarType, NDimensions>::
SetFixedParameters( const ParametersType & parameters )
{
    typename PointsContainer::Pointer landmarks = PointsContainer::New();
    const unsigned int numberOfLandmarks =  parameters.Size() / NDimensions;

    landmarks->Reserve( numberOfLandmarks );

    PointsIterator itr = landmarks->Begin();
    PointsIterator end = landmarks->End();

    InputPointType  landMark;

    unsigned int pcounter = 0;
    while( itr != end )
    {
        for(unsigned int dim=0; dim<NDimensions; dim++)
        {
            landMark[ dim ] = parameters[ pcounter ];
            pcounter++;
        }
        itr.Value() = landMark;
        itr++;
    }

    //  m_TargetLandmarks->SetPoints( landmarks );
    m_SourceLandmarks->SetPoints( landmarks );

    // these are invalidated when the source lms change
    m_WMatrixComputed=false;
    m_LMatrixComputed=false;
    m_LInverseComputed=false;

    // you must recompute L and Linv - this does not require the targ lms
    //this->ComputeLInverse();
    this->ComputeL();

}


// Update parameters array
// They are the components of all the landmarks in the source space
template <class TScalarType, unsigned int NDimensions>
void
SparseKernelTransform<TScalarType, NDimensions>::
UpdateParameters( void ) const
{
    this->m_Parameters = ParametersType( m_TargetLandmarks->GetNumberOfPoints() * NDimensions );

    PointsIterator itr = m_TargetLandmarks->GetPoints()->Begin();
    PointsIterator end = m_TargetLandmarks->GetPoints()->End();

    unsigned int pcounter = 0;
    while( itr != end )
    {
        InputPointType  landmark = itr.Value();
        for(unsigned int dim=0; dim<NDimensions; dim++)
        {
            this->m_Parameters[ pcounter ] = landmark[ dim ];
            pcounter++;
        }
        itr++;
    }
}




// Get the parameters
// They are the components of all the landmarks in the source space
template <class TScalarType, unsigned int NDimensions>
const typename SparseKernelTransform<TScalarType, NDimensions>::ParametersType &
SparseKernelTransform<TScalarType, NDimensions>::
GetParameters( void ) const
{
    this->UpdateParameters();
    return this->m_Parameters;

}


// Get the fixed parameters
// This returns the target landmark locations
// This was added to support the Transform Reader/Writer mechanism
template <class TScalarType, unsigned int NDimensions>
const typename SparseKernelTransform<TScalarType, NDimensions>::ParametersType &
SparseKernelTransform<TScalarType, NDimensions>::
GetFixedParameters( void ) const
{
    this->m_FixedParameters = ParametersType( m_SourceLandmarks->GetNumberOfPoints() * NDimensions );

    PointsIterator itr = m_SourceLandmarks->GetPoints()->Begin();
    PointsIterator end = m_SourceLandmarks->GetPoints()->End();

    unsigned int pcounter = 0;
    while( itr != end )
    {
        InputPointType  landmark = itr.Value();
        for(unsigned int dim=0; dim<NDimensions; dim++)
        {
            this->m_FixedParameters[ pcounter ] = landmark[ dim ];
            pcounter++;
        }
        itr++;
    }

    return this->m_FixedParameters;

}



template <class TScalarType, unsigned int NDimensions>
void
SparseKernelTransform<TScalarType, NDimensions>::
PrintSelf(std::ostream& os, Indent indent) const
{
    Superclass::PrintSelf(os,indent);
    if (m_SourceLandmarks)
    {
        os << indent << "SourceLandmarks: " << std::endl;
        m_SourceLandmarks->Print(os,indent.GetNextIndent());
    }
    if (m_TargetLandmarks)
    {
        os << indent << "TargetLandmarks: " << std::endl;
        m_TargetLandmarks->Print(os,indent.GetNextIndent());
    }
    if (m_Displacements)
    {
        os << indent << "Displacements: " << std::endl;
        m_Displacements->Print(os,indent.GetNextIndent());
    }
    os << indent << "Stiffness: " << m_Stiffness << std::endl;
}
} // namespace itk

#endif
