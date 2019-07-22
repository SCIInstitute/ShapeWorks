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
#ifndef __itkPSMPointTree_hxx
#define __itkPSMPointTree_hxx
#include "itkPSMPointTree.h"

#include <cmath>

namespace itk
{

  template <unsigned int VDimension>
    void PSMPointTreeNode<VDimension>
    ::PrintSelf(std::ostream& os, Indent indent) const
    {
      os << indent << "PSMPointTreeNode: ";
      os << "m_LowerBound = " << m_LowerBound;
      os << "\tm_UpperBound = " << m_UpperBound;
      os << "\tBranchesPerNode = " << BranchesPerNode;
      os << "\tIsLeaf() = " << this->IsLeaf() << std::endl;
      os << "\tm_List: ";
      for (typename PointListType::const_iterator it = m_List.begin();
          it != m_List.end(); it++)
      {   os << "\t (" << it->Index << ", " << it->Point << ")" << std::endl;    }
      os << std::endl;
      for (unsigned int i = 0; i < BranchesPerNode; i++)
      {
        if (m_Branches[i].GetPointer() != 0)  m_Branches[i]->PrintSelf(os, indent);
      }
    }

  template <unsigned int VDimension>
    bool  PSMPointTree<VDimension>
    ::Overlap(const NodePointerType &node, const PointType &lowerbound,
        const PointType &upperbound) const
    {
      // Check for overlap in all dimensions.  If any one dimension has no overlap,
      // these two regions do not overlap.
      for (unsigned int i = 0; i < Dimension; i++)
      {
        // Does this region contain either node endpoint or does the node region
        // contain both of this region's endpoints?
        if (!  ((node->GetLowerBound()[i] >= lowerbound[i] && node->GetLowerBound()[i] <= upperbound[i])
              || (node->GetUpperBound()[i] >= lowerbound[i] && node->GetUpperBound()[i] <= upperbound[i])
              || (node->GetLowerBound()[i] <= lowerbound[i] && node->GetUpperBound()[i] >= upperbound[i] ) ))
          return false;
      }
      return true;
    }

  template <unsigned int VDimension>
    typename PSMPointTree<VDimension>::PointIteratorListType
    PSMPointTree<VDimension>::
    FindPointsInRegion(const PointType &lowerbound,  const PointType &upperbound) const
    {
      PointIteratorListType pointlist;
      NodePointerType it = this->m_Root;

      // If no overlap with the root node exists, then return an empty list...
      if ( this->Overlap(it, lowerbound, upperbound) && ! it->IsLeaf() )
      {
        // otherwise, descend into the child nodes and compile the list.
        for (unsigned int i = 0; i < BranchesPerNode; i++)
        {
          this->FindOneNodeInRegion(it->GetBranch(i), lowerbound, upperbound, pointlist);
        }
      }

      return pointlist;
    }

  template <unsigned int VDimension>
    void
    PSMPointTree<VDimension>::
    FindOneNodeInRegion(const NodePointerType &it, const PointType &lowerbound,
        const PointType &upperbound,
        PointIteratorListType &pointlist) const
    {
      // If this node is a leaf node, then add to the list if there is overlap.
      if (it ->IsLeaf())
      {
        if ( this->Overlap(it, lowerbound, upperbound) )
        {
          // Add all node's points to the list that are within the given region.
          for (typename PointListType::const_iterator pit = it->GetList().begin();
              pit != it->GetList().end(); pit++)
          {
            if (this->RegionContains(pit->Point, lowerbound, upperbound) )
            {
              pointlist.push_back(pit);
            }
          }
        }
      }
      else   // Otherwise, call this method on each branch.
      {
        for (unsigned int i = 0; i < BranchesPerNode; i++)
        {
          this->FindOneNodeInRegion(it->GetBranch(i), lowerbound, upperbound, pointlist);
        }
      }
    }

  template <unsigned int VDimension>
    typename PSMPointTree<VDimension>::PointListType::iterator
    PSMPointTree<VDimension>::AddPoint(const PointType &point,
        unsigned int  idx,
        NodePointerType &node)
    {
      NodePointerType it = this->m_Root;

      if ( ! it->Contains(point))
      {
        itkExceptionMacro("Point " << point << " is not contained within tree domain " << it->GetLowerBound() << " - " << it->GetUpperBound());
      }

      while (! it->IsLeaf() )
      {
        for (unsigned int i = 0; i < BranchesPerNode; i++)
        {
          if (it->GetBranch(i)->Contains(point))
          {
            it = it->GetBranch(i);
            break;
          }
        }
      }
      node = it;
      return it->InsertElement( PSMPointIndexPair<VDimension>(point, idx) );
    }

  template  <unsigned int VDimension>
    void PSMPointTree<VDimension>::ConstructTree(const PointType &lowerbound,
        const PointType &upperbound, unsigned int depth)
    {
      m_Depth = depth;

      NodePointerType n = NodeType::New();
      n->SetLowerBound(lowerbound);
      n->SetUpperBound(upperbound);

      this->SetRoot(n);
      this->BranchNode(n, 1);
    }

  template  <unsigned int VDimension>
    void PSMPointTree<VDimension>::BranchNode(NodePointerType &parent,
        unsigned int level)
    {
      // Called from a leaf node.
      if (level > m_Depth) return;

      PointType midpoint, upper, lower;
      int thresh[VDimension];
      for (unsigned int dim = 0; dim < VDimension; dim++)
      {
        thresh[dim] = 1;
        midpoint[dim] = ( parent->GetLowerBound()[dim] + parent->GetUpperBound()[dim] ) /  2.0;
      }

      for (unsigned int b = 0; b <  BranchesPerNode; b++)
      {
        for (unsigned int dim = 0; dim <  VDimension; dim++)
        {
          // toggle use low/high lower and upper bounds
          if ( b % ( static_cast<unsigned int>(pow(2.0, (int)dim)) ) == 0 )
          {        thresh[dim] = 1 - thresh[dim];        }

          if (  thresh[dim] == 0 )
          {
            lower[dim] = parent->GetLowerBound()[dim];
            upper[dim] = midpoint[dim];
          }
          else
          {
            lower[dim] = midpoint[dim];
            upper[dim] = parent->GetUpperBound()[dim];
          }
        }

        NodePointerType newNode = NodeType::New();
        newNode->SetLowerBound(lower);
        newNode->SetUpperBound(upper);

        parent->SetBranch(b, newNode);

        this->BranchNode(newNode, level+1);
      }
    }

  template  <unsigned int VDimension>
    void PSMPointTree<VDimension>::PrintSelf(std::ostream& os, Indent indent) const
    {
      os << indent << "BranchesPerNode = " << BranchesPerNode << std::endl;
      os << indent << "m_Depth = " << m_Depth << std::endl;
      os << indent << "m_Root:  ";
      m_Root->PrintSelf(os, indent);
      Superclass::PrintSelf(os, indent);
    }

} // end namespace itk
#endif
