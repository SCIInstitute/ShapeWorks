/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: TPSAnalysis.cxx,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include <iostream>
#include <vector>
#include <string>
#include "itkParticlePositionReader.h"
#include "itkParticlePositionWriter.h"
#include "itkParticleSystem.h"
#include "object_writer.h"
#include "object_reader.h"
#include "param.h"
#include <iostream>
#include <fstream>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_determinant.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

int main(int argc, char *argv[])
{
  typedef itk::ParticleSystem<3> ParticleSystemType;
  typedef vnl_vector<double> vectype;
  typedef vnl_matrix<double> matrixtype;

  if (argc != 4)
    {
    std::cerr << "Use: " << argv[0] << "input1 input2 output" << std::endl;
    return 1;
    }


  // Read the point data
  itk::ParticlePositionReader<3>::Pointer reader1 = itk::ParticlePositionReader<3>::New();
  itk::ParticlePositionReader<3>::Pointer reader2 = itk::ParticlePositionReader<3>::New();
  reader1->SetFileName(argv[1]);
  reader2->SetFileName(argv[2]);
  reader1->Update();
  reader2->Update();
  const unsigned int N = reader2->GetOutput().size();
  if (reader1->GetOutput().size() != N)
    {
    std::cerr << "Input files are not the same size" << std::endl;
    return 1;
    }
  
  // Transforms are from 1 to 2
  vectype b(N+4); // point data from file 2
  vectype x(N+4); // parameters to be solved for
  matrixtype A(N+4,N+4);
  matrixtype Phi(N,N);

  // Compute Phi values
  vectype Xi(3);
  vectype Xj(3);
  for (unsigned int i = 0; i < N; i++)
    {
    Xi[0] = (reader1->GetOutput()[i])[0];
    Xi[1] = (reader1->GetOutput()[i])[1];
    Xi[2] = (reader1->GetOutput()[i])[2];
    
    for (unsigned int j = 0; j < N; j++)
      {
      Xj[0] = (reader1->GetOutput()[j])[0];
      Xj[1] = (reader1->GetOutput()[j])[1];
      Xj[2] = (reader1->GetOutput()[j])[2];
      
      Phi(i,j) = (Xi - Xj).magnitude();
      // Phi(i,j) = dot_product(Xi - Xj,Xi - Xj) * log((Xi - Xj).magnitude() + 1.0e-6);
      }
    }
 
  // Construct the coefficient matrix A from file 1 and Phi's
  for (unsigned int i = 0; i < 4; i++)
    {
    for (unsigned int j = 0; j < N +4; j++)
      {
      if      (j >= N) A(i,j) = 0.0;
      else if (i == 3) A(i,j) = 1.0;
      else             A(i,j) = (reader1->GetOutput()[j])[i];
      }    
    }

  // Rest of the matrix A
  for (unsigned int i = 0; i < N; i++)
    {
    for (unsigned int j = 0; j < N +4; j++)
      {
      if      (j < N)    A(i+4,j) = Phi(i,j);
      else if (j == N+3) A(i+4,j) = 1.0;
      else               A(i+4,j) = (reader1->GetOutput()[i])[2-(j-N)];
      }
    }

  // To keep things simple, solve in each dimension separately
  matrixtype P(4,3);
  matrixtype C(N,3);
  for (unsigned int D = 0; D < 3; D++)
    {
    // Construct the position data vector b from file 2 
    // [0 0 0 0 x1' x2' x3' ... xN' ]^T   
    b[0] = b[1] = b[2] = b[3] = 0.0;
    for(unsigned int i = 0; i < N; i++)
      {
      b[i+4] = (reader2->GetOutput()[i])[D];
      }

    // Solve for parameters
    x = vnl_matrix_inverse<double>(A) * b;

    // Store results
    for (unsigned int i = 0; i < N; i++)
      {
      C(i,D) = x[i];
      }
    
    P(0,D) = x[N+3];
    P(1,D) = x[N+2];
    P(2,D) = x[N+1];
    P(3,D) = x[N];
    
    } // end D
  //  std::cout << P << std::endl << std::endl;
  // std::cout << C << std::endl;

  // DEBUG -- Check transforms
//   for (unsigned int i = 0; i < N; i++)
//     {
//     Xi[0] =  (reader1->GetOutput()[i])[0];
//     Xi[1] =  (reader1->GetOutput()[i])[1];
//     Xi[2] =  (reader1->GetOutput()[i])[2];
    
//     double sumX = 0;
//     double sumY = 0;
//     double sumZ = 0;        
//     for (unsigned int j = 0; j < N; j++)
//       {
//       Xj[0] =  (reader1->GetOutput()[j])[0];
//       Xj[1] =  (reader1->GetOutput()[j])[1];
//       Xj[2] =  (reader1->GetOutput()[j])[2];

//       sumX += C[j][0] * Phi(i,j);
//       sumY += C[j][1] * Phi(i,j);
//       sumZ += C[j][2] * Phi(i,j);
//       }

//     vectype xPrime(3);
//     xPrime[0] = sumX + P[0][0] + Xi[0]*P[1][0] + Xi[1]*P[2][0] + Xi[2]*P[3][0];
//     xPrime[1] = sumY + P[0][1] + Xi[0]*P[1][1] + Xi[1]*P[2][1] + Xi[2]*P[3][1];
//     xPrime[2] = sumZ + P[0][2] + Xi[0]*P[1][2] + Xi[1]*P[2][2] + Xi[2]*P[3][2]; 

//    //   std::cout << reader2->GetOutput()[i] << " = " << xPrime << std::endl;;   
//     }


  
  // COMPUTE FEATURES
  // Jacobian
  matrixtype J(3,3);
  std::vector<double> detJ;
  std::ofstream ellipsefile;
  ellipsefile.open("ellipses.txt");
  
  ellipsefile << "NUMBER_OF_POINTS = " << N << std::endl;
  ellipsefile << "DIMENSION = 9" << std::endl;
  ellipsefile << "TYPE = Ellipsoid" << std::endl;
  
  for (unsigned int i = 0; i < N; i++)  // for each point
    {
    Xi[0] =  (reader1->GetOutput()[i])[0];
    Xi[1] =  (reader1->GetOutput()[i])[1];
    Xi[2] =  (reader1->GetOutput()[i])[2];

    double sumXx = 0.0;
    double sumYx = 0.0;
    double sumZx = 0.0;
    double sumXy = 0.0;
    double sumYy = 0.0;
    double sumZy = 0.0;
    double sumXz = 0.0;
    double sumYz = 0.0;
    double sumZz = 0.0;
    
    for (unsigned int j = 0; j < N; j++) // summation
      {
       Xj[0] =  (reader1->GetOutput()[j])[0];
       Xj[1] =  (reader1->GetOutput()[j])[1];
       Xj[2] =  (reader1->GetOutput()[j])[2];

       double q = (Xi - Xj).magnitude() + 1.0e-6;

       sumXx += C[j][0] * ( (Xi[0] - Xj[0]) / q );
       sumXy += C[j][0] * ( (Xi[1] - Xj[1]) / q );
       sumXz += C[j][0] * ( (Xi[2] - Xj[2]) / q );
       
       sumYx += C[j][1] * ( (Xi[0] - Xj[0]) / q );
       sumYy += C[j][1] * ( (Xi[1] - Xj[1]) / q );
       sumYz += C[j][1] * ( (Xi[2] - Xj[2]) / q );
       
       sumZx += C[j][2] * ( (Xi[0] - Xj[0]) / q );
       sumZy += C[j][2] * ( (Xi[1] - Xj[1]) / q );
       sumZz += C[j][2] * ( (Xi[2] - Xj[2]) / q );
      }


    J(0,0) =  1 - ( sumXx + P[1][0]);
    J(0,1) =     - ( sumXy + P[2][0]);
    J(0,2) =     - ( sumXz + P[3][0]);

    J(1,0) =     - ( sumYx + P[1][1]);
    J(1,1) =  1 - ( sumYy + P[2][1]);
    J(1,2) =     - ( sumYz + P[3][1]);

    J(2,0) =     - ( sumZx + P[1][2]);
    J(2,1) =     - ( sumZy + P[2][2]);
    J(2,2) =  1 - ( sumZz + P[3][2]);

    //     std::cout << J << std::endl;


    // Compute strain tensor
    matrixtype E = 0.5 * (J + J.transpose() + J.transpose() * J);
    vnl_symmetric_eigensystem<double> sys(E);

    //    std::cout << E << std::endl;
    
    double mag = sys.get_eigenvalue(0) + sys.get_eigenvalue(1) + sys.get_eigenvalue(2);

    

    // Sort eigenvalues by magnitude
    unsigned int order[3];
    order[0] = 0; order[1] = 1; order[2] = 2;
    double e[3];    
    e[0] = fabs(sys.get_eigenvalue(2));
    e[1] = fabs(sys.get_eigenvalue(1));
    e[2] = fabs(sys.get_eigenvalue(0));

    bool done = false;
    while(! done)
      {
      done = true;
      for (unsigned int z = 1; z < 3; z++)
        {
        if ( e[z-1] < e[z] )
          {
          double tmpd = e[z-1];
          e[z-1] = e[z];
          e[z] = tmpd;
          
          unsigned int tmpi = order[z-1];
          order[z-1] = order[z];
          order[z] = tmpi;
            
          done = false;
          }
        }      
      }

    

    std::vector<vectype> evecs;
    evecs.push_back(sys.get_eigenvector(2) * sys.get_eigenvalue(2));
    evecs.push_back(sys.get_eigenvector(1) * sys.get_eigenvalue(1));
    evecs.push_back(sys.get_eigenvector(0) * sys.get_eigenvalue(0));

    //    std::cout << sys.get_eigenvalue(2) << " " << sys.get_eigenvalue(1)
    //              << " " << sys.get_eigenvalue(0);

    //    std::cout << " " << order[0] << " " << order[1] << " " << order[2] << std::endl;

    //    for (unsigned int y = 0; y < 3; y++)
    //      {      
    //      for (unsigned int z = 0; z < 3; z++)
    //        {
    //        ellipsefile << evecs[order[z]][y] << " ";
    //        }
    //      ellipsefile << std::endl;
    //      }
        
  //   ellipsefile << evecs[0][0] << " " << evecs[1][0] << " " << evecs[2][0] << " "
//                 << evecs[0][1] << " " << evecs[1][1] << " " << evecs[2][1] << " "
//                 << evecs[0][2] << " " << evecs[1][2] << " " << evecs[2][2] 
//                 << std::endl;

     ellipsefile << evecs[0][0] << " " << evecs[0][1] << " " << evecs[0][2] << " "
                 << evecs[1][0] << " " << evecs[1][1] << " " << evecs[1][2] << " "
                 << evecs[2][0] << " " << evecs[2][1] << " " << evecs[2][2] 
                 << std::endl;
    
    detJ.push_back(sys.get_eigenvalue(order[0]));
    } // end Features
  
  ellipsefile.close();
  
  std::ofstream outfile;
  outfile.open(argv[3]);

  outfile << "NUMBER_OF_POINTS = " << N << std::endl;
  outfile << "DIMENSION = 1" << std::endl;
  outfile << "TYPE = Scalar" << std::endl;
  
  for (unsigned int i = 0; i < N; i++)
    {
    outfile << detJ[i] << std::endl;    
    //    std::cout << detJ[i] << std::endl;
    }
  outfile.close();
  
  return 0;
}
