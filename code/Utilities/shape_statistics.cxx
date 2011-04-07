/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: shape_statistics.cxx,v $

  Date:      $Date: 2011/03/24 01:17:41 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "shape_statistics.h"



int shape_statistics::ComputeMedianShape(const int ID)

{

  int ret = -1;

  double min_L1 = 1.0e300;

  

  // Compile list of indices for groupIDs == ID

  std::vector<unsigned int> set;

  for (unsigned int i = 0; i < m_groupIDs.size(); i++)

    {

    if (m_groupIDs[i] == ID)

      { set.push_back(i); }

    }



  // Find min sum L1 norms

  for (unsigned int i = 0; i < set.size(); i++)

    {

    double sum = 0.0;

    std::cout << "i = " << i << std::endl;

    for (unsigned int j = 0; j < set.size(); j++)

      {      

      sum += this->L1Norm(i,j);

      std::cout << j << "\t" << this->L1Norm(i,j) << std::endl;

      }



    if (sum < min_L1)

      {

      min_L1 = sum;

      ret = static_cast<int>(i);

      }

    }

  std::cout << "min_L1 = " << min_L1 << std::endl;

  return ret; // if there has been some error ret == -1

}



double shape_statistics::L1Norm(unsigned int a, unsigned int b)

{

  double norm = 0.0;

  for (unsigned int i = 0; i < m_shapes.rows(); i++)

    {

    norm += fabs(m_shapes(i,a) - m_shapes(i,b));

    }

  return norm;

}



int shape_statistics::ReadPointFiles(param::parameterFile &pf)

{

  // Collect point file names and group id's

  std::vector< std::string > pointsfiles;

  bool ok = true;

  

  for (unsigned int i = 0; ok == true; i++)

    {

    std::string tmp;

    PARAMSET(pf, tmp, "point_files", i, ok, "");

    if (ok == true)

      {

      pointsfiles.push_back(tmp);

      }

    }



  //  ok = true;

  //  int NM;

  //  PARAMSET(pf, NM, "number_of_modes", 0, ok, 0);

  //  if (ok != true)

  //    {

  //    std::cerr << "Missing number_of_modes parameter" << std::endl;

  //    return 1;

  //    }

  

  PARAMSET(pf, m_domainsPerShape, "domains_per_shape", 0, ok, 1);



  // Read the point files.  Assumes all the same size.

  itk::ParticlePositionReader<3>::Pointer reader1

    = itk::ParticlePositionReader<3>::New();

  reader1->SetFileName( pointsfiles[0].c_str() );

  reader1->Update();



  m_numSamples1 = 0;

  m_numSamples2 = 0;

  m_numSamples = pointsfiles.size() / m_domainsPerShape;

  m_numDimensions = reader1->GetOutput().size() * 3 * m_domainsPerShape;



  // Read the group ids

  ok = true;

  for (unsigned int i = 0; i < m_numSamples; i++)

    {

    int g;

    PARAMSET(pf, g,   "group_ids", i, ok, 0);

    

    if (ok == true)

      {

      m_groupIDs.push_back(g);

      if (g == 1) m_numSamples1++;

      else m_numSamples2++;

      }

    }



  if (m_groupIDs.size() != m_numSamples) return 1;

  

  m_pointsMinusMean.set_size(m_numDimensions, m_numSamples);

  m_shapes.set_size(m_numSamples, m_numSamples);



  m_mean.set_size(m_numDimensions);

  m_mean.fill(0);



//   m_mean1.set_size(m_numDimensions);

//   m_mean1.fill(0);

//   m_mean2.set_size(m_numDimensions);

//   m_mean2.fill(0);



  // Compile the "meta shapes"

  for (unsigned int i = 0; i < m_numSamples; i++) 

    {

    for (unsigned int k = 0; k < m_domainsPerShape; k++) 

      {

      // read file

      itk::ParticlePositionReader<3>::Pointer reader

        = itk::ParticlePositionReader<3>::New();

      reader->SetFileName( pointsfiles[i*m_domainsPerShape + k].c_str() );

      reader->Update();

      unsigned int q = reader->GetOutput().size();  

      for (unsigned int j = 0; j < q; j++)

        {

        m_mean(q*k*3 +(3*j)+0) += m_pointsMinusMean(q*k*3 +(3*j)+0, i)

          = reader->GetOutput()[j][0];

        m_mean(q*k*3 +(3*j)+1) += m_pointsMinusMean(q*k*3 +(3*j)+1, i)

          = reader->GetOutput()[j][1];

        m_mean(q*k*3 +(3*j)+2) += m_pointsMinusMean(q*k*3 +(3*j)+2, i)

          = reader->GetOutput()[j][2];





//         if (m_groupIDs[i] == 1)

//           {

//           m_mean1(q*k*3 +(3*j)+0) += reader->GetOutput()[j][0];

//           m_mean1(q*k*3 +(3*j)+1) += reader->GetOutput()[j][1];

//           m_mean1(q*k*3 +(3*j)+2) += reader->GetOutput()[j][2];

//           }

//         else

//           {

//           m_mean2(q*k*3 +(3*j)+0) += reader->GetOutput()[j][0];

//           m_mean2(q*k*3 +(3*j)+1) += reader->GetOutput()[j][1];

//           m_mean2(q*k*3 +(3*j)+2) += reader->GetOutput()[j][2];

//           }

        

        //        m_shapes(q*k*3 +(3*j)+0) = reader->GetOutput()[j][0];

        //        m_shapes(q*k*3 +(3*j)+1) = reader->GetOutput()[j][1];

        //        m_shapes(q*k*3 +(3*j)+2) =  reader->GetOutput()[j][2];

                

        }

      }

    }



  for (unsigned int i = 0; i < m_numDimensions; i++)

    {

    m_mean(i)  /= (double)m_numSamples;

    //    m_mean1(i) /= (double)m_numSamples1;

    //    m_mean1(i) /= (double)m_numSamples2;

    }

  

  for (int j = 0; j < m_numDimensions; j++)

    {

    for (int i = 0; i < m_numSamples; i++)

      {

      m_pointsMinusMean(j, i) -= m_mean(j);

      }

    }

  return 0;

} // end ReadPointFiles





int shape_statistics::ComputeModes()

{

  // COMPUTE MODES

  vnl_matrix<double> A = m_pointsMinusMean.transpose()

    * m_pointsMinusMean * (1.0/((double)(m_numSamples-1)));



  vnl_symmetric_eigensystem<double> symEigen(A);

  m_eigenvectors = m_pointsMinusMean * symEigen.V;

  m_eigenvalues.set_size(m_numSamples);



  // normalize those eigenvectors

  for (int i = 0; i < m_numSamples; i++)

    {

    double total = 0.0f;

    for (int j = 0; j < m_numDimensions; j++)

      {

      total += m_eigenvectors(j, i) * m_eigenvectors(j, i);

      }

    total = sqrt(total);

    

    for (int j = 0; j < m_numDimensions; j++)

      {

      m_eigenvectors(j, i) = m_eigenvectors(j, i) / (total + 1.0e-15);

      }



    m_eigenvalues(i) = symEigen.D(i, i);

    }

  

  std::cout << "Eigens" << std::endl;

  float sum = 0.0;

  for (unsigned int n = 0; n < m_numSamples; n++)

    {

    std::cout << m_eigenvalues[(m_numSamples-1)-n] << std::endl;

    sum += m_eigenvalues[(m_numSamples-1)-n];

    }



  //  std::cout << "total variance = " << sum << std::endl;

  m_top95 = 0;

  float sum2 = 0.0;

  bool found= false;

  for (unsigned int n = 0; n < m_numSamples; n++)

    {

    sum2 += m_eigenvalues[(m_numSamples-1)-n];

    m_percentVarByMode.push_back(sum2 / sum);

    //    std::cout << "SUM2 = " << sum2 << std::endl;

    if ((sum2 / sum) >= 0.95 && found==false)

      {      

      m_top95 = n;

      found=true;

      }

    }



  return 0;

}  // end ComputeModes();





int shape_statistics::PrincipalComponentProjections()

{

  // Now print the projection of each shape

  m_principals.set_size(m_numSamples, m_numSamples);

  

  for (unsigned int n = 0; n < m_numSamples; n++)

    {

    for (unsigned int s = 0; s < m_numSamples; s++)

      {

      double p = dot_product<double>(m_eigenvectors.get_column((m_numSamples-1)-n),

                                     m_pointsMinusMean.get_column(s));



      m_principals(s, n) = p; // each row is a sample, columns index PC



      }

    }



  //  std::cout << m_principals << std::endl;

  

  // DEBUG

  //  std::cout << "Samples projected to first " << top95

  //            << " principal components." << std::endl;

  //  for (unsigned int i = 0; i < m_numSamples; i++)

  //    {

  //    std::cout << i << "\t" << samp.get_column(i) << std::endl;

  //    }

  

  return 0;

}

int shape_statistics::FisherLinearDiscriminant(int numModes)

{

  m_projectedMean1.set_size(numModes);

  m_projectedMean2.set_size(numModes);

  m_projectedMean1.fill(0.0);

  m_projectedMean2.fill(0.0);

  

  m_projectedPMM1.set_size(numModes, m_numSamples1);

  m_projectedPMM2.set_size(numModes, m_numSamples2);

  

  unsigned int s1 = 0;

  unsigned int s2 = 0;

  for (unsigned int n = 0; n < numModes; n++)

    {

    s1 = 0;

    s2 = 0;

    for (unsigned int s = 0; s < m_numSamples; s++)

      {

      double p = dot_product<double>(m_eigenvectors.get_column((m_numSamples-1)-n),

                                     m_pointsMinusMean.get_column(s));

      

      if (m_groupIDs[s] == 1)

        {

        m_projectedPMM1(n,s1) = p;

        m_projectedMean1[n] += p;

        s1++;

        }

      else

        {

        m_projectedPMM2(n,s2) = p;

        m_projectedMean2[n] += p;

        s2++;

        }

      }

    }

  

  // Compute means and covariance matrices for each group

  m_projectedMean1 /= static_cast<double>(m_numSamples1);

  m_projectedMean2 /= static_cast<double>(m_numSamples2);

  

  m_fishersProjection.resize(m_numSamples);

  for (unsigned int i = 0; i < m_numSamples; i++) m_fishersProjection[i]=0.0;

  

  

  for (unsigned int i = 0; i < numModes; i++) // modes

    {

     for (unsigned int j = 0; j < m_numSamples1; j++) // samples

       {

       m_projectedPMM1(i,j) -= m_projectedMean1(i);

       }

     for (unsigned int j = 0; j < m_numSamples2; j++) // samples

       {

       m_projectedPMM2(i,j) -= m_projectedMean2(i);

       }

     }

 

  

  vnl_matrix<double> cov1 = (m_projectedPMM1 * m_projectedPMM1.transpose()) / ((double)(m_numSamples1)-1.0);

  vnl_matrix<double> cov2 = (m_projectedPMM2 * m_projectedPMM2.transpose()) / ((double)(m_numSamples2)-1.0);



  vnl_vector<double> mdiff = m_projectedMean1 - m_projectedMean2;

  vnl_matrix<double> covsuminv = vnl_matrix_inverse<double>(cov1 + cov2);

  

  // w is fishers linear discriminant (normal to the hyperplane)

   vnl_vector<double> w = covsuminv * mdiff;



   //x   m_fishersLD = w;



   // Normalize to distance between means

   double mag = mdiff.magnitude();

   m_fishersLD = (w * mag)/ sqrt(dot_product<double>(w,w));



   std::cout << std::endl << "fishers ld =  " << m_fishersLD << std::endl;





   

   vnl_vector<double> wext(m_numSamples);

   for (unsigned int i = 0; i < m_numSamples; i++)

     {

     if (i >= numModes) wext[i] = 0.0;

     else wext[i] = m_fishersLD[i];// * m_eigenvalues[(m_numSamples - 1) - i];

     }

   std::cout << "wext = " << wext << std::endl;



   std::cout << "wext size= " << m_numSamples << std::endl;

   std::cout << "covar size = " << m_eigenvectors.rows() << " x " << m_eigenvectors.cols() << std::endl;



   // Rotate the LD back into the full dimensional space

   // Rearrange the eigenvectors:

     

   vnl_matrix<double> tmpeigs = m_eigenvectors;

   tmpeigs.fliplr();

   

   vnl_vector<double> bigLD = wext.post_multiply(tmpeigs.transpose());



   // Create a file of vectors in the 3D space from bigLD that KWMeshvisu can

   // read







   

 // Open the output file.

  std::ofstream out("LinearDiscriminantsVectors.txt");

 

  out << "NUMBER_OF_POINTS = " << m_numDimensions << std::endl;

  out << "DIMENSION = 3" << std::endl;

  out << "TYPE = Vector" << std::endl;

  

  // Write points.

  for (unsigned int i = 0; i < m_numDimensions; )

    {

    for (unsigned int j = 0; j < 3; j++)

      {

      out << -bigLD[i] << " ";

      i++;

      }

    out << std::endl;

    }



  out.close();









//   // Open the output file for magnitude

//   std::ofstream out("LinearDiscriminantsMagnitudes.txt");

 

//   out << "NUMBER_OF_POINTS = " << m_numDimensions << std::endl;

//   out << "DIMENSION = 3" << std::endl;

//   out << "TYPE = Vector" << std::endl;

  

//   // Write points.

//   for (unsigned int i = 0; i < m_numDimensions; )

//     {

//     for (unsigned int j = 0; j < 3; j++)

//       {

//       out << bigLD[i] << " ";

//       i++;

//       }

//     out << std::endl;

//     }



//    out.close();

   



   





   

//    for (unsigned int i = 0; i < m_numSamples; i++)

//      {

//      m_fishersProjection.push_back(dot_product(w, m_principals.get_column(i)));

//      std::cout << dot_product(w,m_principals.get_column(i)) << std::endl;

//      }



  // //   std::cout << "Distance btwn means = " << meandiff.magnitude() << std::endl;

// //   std::cout << "Variance in group 1 = " << dot_product(meandiff.post_multiply(groupcov1), meandiff) << std::endl;

// //   std::cout << "Variance in group 2 = " << dot_product(meandiff.post_multiply(groupcov2), meandiff) << std::endl;



  return 0;

}





// int shape_statistics::FisherLinearDiscriminant(int numModes)

// {

//   m_projectedMean1.set_size(m_numSamples);

//   m_projectedMean2.set_size(m_numSamples);

//   m_projectedMean1.fill(0.0);

//   m_projectedMean2.fill(0.0);

  

//   m_projectedPMM1.set_size(m_numSamples, m_numSamples);

//   m_projectedPMM2.set_size(m_numSamples, m_numSamples);

  

//   unsigned int s1 = 0;

//   unsigned int s2 = 0;



//   vnl_matrix<double> modevectors = m_eigenvectors;

//   for (unsigned int i = 0; i < m_numSamples; i++)

//     for (unsigned int j = numModes; j< m_numSamples; j++)

//       { modevectors[j][i] = 0.0; }

  

//   for (unsigned int n = 0; n < m_numSamples; n++)

//     {

//     s1 = 0;

//     s2 = 0;

//     for (unsigned int s = 0; s < m_numSamples; s++)

//       {

//       double p = dot_product<double>(modevectors.get_column((m_numSamples-1)-n),

//                                      m_pointsMinusMean.get_column(s));

      

//       if (m_groupIDs[s] == 1)

//         {

//         m_projectedPMM1(n,s1) = p;

//         m_projectedMean1[n] += p;

//         s1++;

//         }

//       else

//         {

//         m_projectedPMM2(n,s2) = p;

//         m_projectedMean2[n] += p;

//         s2++;

//         }

//       }

//     }

  

//   // Compute means and covariance matrices for each group

//   m_projectedMean1 /= static_cast<double>(m_numSamples1);

//   m_projectedMean2 /= static_cast<double>(m_numSamples2);

  

  

//   m_fishersProjection.resize(m_numSamples);

//   for (unsigned int i = 0; i < m_numSamples; i++) m_fishersProjection[i]=0.0;

  

//   for (unsigned int i = numModes; i < m_numSamples; i++)

//     {

//     m_projectedMean1[i] = 0.0;

//     m_projectedMean2[i] = 0.0;

//     }

  

  

//   for (unsigned int i = 0; i < m_numSamples; i++) // modes

//     {

//      for (unsigned int j = 0; j < s1; j++) // samples

//        {

//        m_projectedPMM1(i,j) -= m_projectedMean1(i);

//        }

//      for (unsigned int j = 0; j < s2; j++) // samples

//        {

//        m_projectedPMM2(i,j) -= m_projectedMean2(i);

//        }

//      }

 

  

//   vnl_matrix<double> cov1 = (m_projectedPMM1 * m_projectedPMM1.transpose()) / (double)m_numSamples1;

//   vnl_matrix<double> cov2 = (m_projectedPMM2 * m_projectedPMM2.transpose()) / (double)m_numSamples2;



//   vnl_vector<double> mdiff = m_projectedMean1 - m_projectedMean2;

//   vnl_matrix<double> covsuminv = vnl_matrix_inverse<double>(cov1 + cov2);

  

//   // w is fishers linear discriminant (normal to the hyperplane)

//    vnl_vector<double> w = covsuminv * mdiff;



//    m_fishersLD = w;

  

//    std::cout << std::endl << "fishers ld =  " << w << std::endl;



//    //   // Project all samples onto ld

// //    for (unsigned int i = 0; i < m_numSamples; i++)

// //      {

// //      m_fishersProjection.push_back(dot_product(w, m_principals.get_column(i)));

// //      std::cout << dot_product(w,m_principals.get_column(i)) << std::endl;

// //      }



//   // //   std::cout << "Distance btwn means = " << meandiff.magnitude() << std::endl;

// // //   std::cout << "Variance in group 1 = " << dot_product(meandiff.post_multiply(groupcov1), meandiff) << std::endl;

// // //   std::cout << "Variance in group 2 = " << dot_product(meandiff.post_multiply(groupcov2), meandiff) << std::endl;

//

//  return 0;

//}



int shape_statistics::WriteCSVFile(const char *fn)

{

  // Write csv file

  std::ofstream outfile;

  outfile.open(fn);



  outfile << "Group,LDA,PV";

  for (unsigned int i = 0; i < m_numSamples; i++)

    {

    outfile << ",P" << i;

    }

  outfile << std::endl;



  for (unsigned int r = 0; r < m_numSamples; r++)

    {

    outfile << m_groupIDs[r] << ",";

    outfile << m_fishersProjection[r] << ",";

    outfile << m_percentVarByMode[r];

    for (unsigned int c = 0; c < m_numSamples; c++)

      {

      outfile << "," << m_principals(r,c);

      }

    outfile << std::endl;

    }



  

  outfile.close();

  return 0;

}

