---
title: FEMesh

---

# FEMesh





Inherits from [FECoreMesh](../Classes/classFECoreMesh.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FEMesh](../Classes/classFEMesh.md#function-femesh)**() |
| | **[FEMesh](../Classes/classFEMesh.md#function-femesh)**([FEMesh](../Classes/classFEMesh.md) & m) |
| virtual | **[~FEMesh](../Classes/classFEMesh.md#function-~femesh)**() |
| virtual void | **[Create](../Classes/classFEMesh.md#function-create)**(int nodes, int elems, int faces =0, int edges =0)<br>allocate space for mesh  |
| void | **[ShallowCopy](../Classes/classFEMesh.md#function-shallowcopy)**([FEMesh](../Classes/classFEMesh.md) * pm) |
| void | **[Clear](../Classes/classFEMesh.md#function-clear)**()<br>&mdash; C L E A N U P &mdash; |
| void | **[DeleteEdges](../Classes/classFEMesh.md#function-deleteedges)**() |
| virtual int | **[Elements](../Classes/classFEMesh.md#function-elements)**() const<br>return number of elements  |
| [FEElement](../Classes/classFEElement.md) & | **[Element](../Classes/classFEMesh.md#function-element)**(int n)<br>return element  |
| virtual [FEElement_](../Classes/classFEElement__.md) & | **[ElementRef](../Classes/classFEMesh.md#function-elementref)**(int n)<br>return reference to element  |
| virtual [FEElement](../Classes/classFEElement.md) * | **[ElementPtr](../Classes/classFEMesh.md#function-elementptr)**(int n =0)<br>return pointer to element  |
| int | **[FindFace](../Classes/classFEMesh.md#function-findface)**([FEElement](../Classes/classFEElement.md) * pe, [FEFace](../Classes/classFEFace.md) & f, [FEFace](../Classes/classFEFace.md) & fe) |
| void | **[FindNodesFromPart](../Classes/classFEMesh.md#function-findnodesfrompart)**(int gid, vector< int > & node) |
| std::vector< [FENode](../Classes/classFENode.md) > & | **[NodeArray](../Classes/classFEMesh.md#function-nodearray)**() |
| std::vector< [FEFace](../Classes/classFEFace.md) > & | **[FaceArray](../Classes/classFEMesh.md#function-facearray)**() |
| std::vector< [FEElement](../Classes/classFEElement.md) > & | **[ElementArray](../Classes/classFEMesh.md#function-elementarray)**() |
| double | **[GetElementValue](../Classes/classFEMesh.md#function-getelementvalue)**(int n) |
| void | **[SetElementValue](../Classes/classFEMesh.md#function-setelementvalue)**(int n, double v) |
| void | **[UpdateValueRange](../Classes/classFEMesh.md#function-updatevaluerange)**() |
| void | **[GetValueRange](../Classes/classFEMesh.md#function-getvaluerange)**(double & vmin, double & vmax) |
| [vec3d](../Classes/classvec3d.md) | **[ProjectToSurface](../Classes/classFEMesh.md#function-projecttosurface)**([vec3d](../Classes/classvec3d.md) r, [vec3d](../Classes/classvec3d.md) t) |
| [vec3d](../Classes/classvec3d.md) | **[ProjectToFace](../Classes/classFEMesh.md#function-projecttoface)**([vec3d](../Classes/classvec3d.md) p, [FEFace](../Classes/classFEFace.md) & f, double & r, double & s) |
| [vec3d](../Classes/classvec3d.md) | **[ProjectToEdge](../Classes/classFEMesh.md#function-projecttoedge)**([vec3d](../Classes/classvec3d.md) e1, [vec3d](../Classes/classvec3d.md) e2, [vec3d](../Classes/classvec3d.md) p, double & r) |
| bool | **[FindIntersection](../Classes/classFEMesh.md#function-findintersection)**([FEFace](../Classes/classFEFace.md) & f, [vec3d](../Classes/classvec3d.md) x, [vec3d](../Classes/classvec3d.md) n, [vec3d](../Classes/classvec3d.md) & q, double & g) |
| void | **[Update](../Classes/classFEMesh.md#function-update)**() |
| void | **[UpdateElementNeighbors](../Classes/classFEMesh.md#function-updateelementneighbors)**() |
| void | **[UpdateFaces](../Classes/classFEMesh.md#function-updatefaces)**() |
| void | **[UpdateEdges](../Classes/classFEMesh.md#function-updateedges)**() |
| void | **[UpdateNodes](../Classes/classFEMesh.md#function-updatenodes)**() |
| void | **[AutoSmooth](../Classes/classFEMesh.md#function-autosmooth)**(double w) |
| void | **[UpdateNormals](../Classes/classFEMesh.md#function-updatenormals)**() |
| void | **[PartitionSelection](../Classes/classFEMesh.md#function-partitionselection)**() |
| void | **[RemoveIsolatedNodes](../Classes/classFEMesh.md#function-removeisolatednodes)**() |
| void | **[AddNode](../Classes/classFEMesh.md#function-addnode)**([FENode](../Classes/classFENode.md) & n) |
| [FEMesh](../Classes/classFEMesh.md) * | **[DetachSelectedMesh](../Classes/classFEMesh.md#function-detachselectedmesh)**() |
| void | **[DetachSelectedPart](../Classes/classFEMesh.md#function-detachselectedpart)**() |
| [FEMesh](../Classes/classFEMesh.md) * | **[ExtractSelectedFaces](../Classes/classFEMesh.md#function-extractselectedfaces)**() |
| void | **[DeleteTaggedElements](../Classes/classFEMesh.md#function-deletetaggedelements)**(int tag) |
| void | **[DeleteTaggedFaces](../Classes/classFEMesh.md#function-deletetaggedfaces)**(int tag) |
| void | **[DeleteTaggedEdges](../Classes/classFEMesh.md#function-deletetaggededges)**(int tag) |
| void | **[FindDuplicateFaces](../Classes/classFEMesh.md#function-findduplicatefaces)**(vector< int > & l) |
| void | **[FindDuplicateEdges](../Classes/classFEMesh.md#function-findduplicateedges)**(vector< int > & l) |
| void | **[DeleteSelectedElements](../Classes/classFEMesh.md#function-deleteselectedelements)**() |
| void | **[DeleteSelectedFaces](../Classes/classFEMesh.md#function-deleteselectedfaces)**() |
| void | **[DeleteSelectedNodes](../Classes/classFEMesh.md#function-deleteselectednodes)**() |
| void | **[InvertTaggedElements](../Classes/classFEMesh.md#function-inverttaggedelements)**(int ntag) |
| void | **[InvertSelectedElements](../Classes/classFEMesh.md#function-invertselectedelements)**() |
| double | **[ShellJacobian](../Classes/classFEMesh.md#function-shelljacobian)**([FEElement](../Classes/classFEElement.md) & el) |
| void | **[RemoveDuplicateElements](../Classes/classFEMesh.md#function-removeduplicateelements)**() |
| void | **[FixinvertedElements](../Classes/classFEMesh.md#function-fixinvertedelements)**() |
| void | **[FixReferenceSurface](../Classes/classFEMesh.md#function-fixreferencesurface)**() |
| void | **[InterpolateShellThickness](../Classes/classFEMesh.md#function-interpolateshellthickness)**(double ) |
| void | **[RemoveNonManifoldElements](../Classes/classFEMesh.md#function-removenonmanifoldelements)**() |
| void | **[FixElementWinding](../Classes/classFEMesh.md#function-fixelementwinding)**() |
| void | **[FixElementWinding2](../Classes/classFEMesh.md#function-fixelementwinding2)**() |
| void | **[TagAllElements](../Classes/classFEMesh.md#function-tagallelements)**(int ntag) |
| int | **[DataFields](../Classes/classFEMesh.md#function-datafields)**() |
| [FEElementData](../Classes/classFEElementData.md) * | **[AddDataField](../Classes/classFEMesh.md#function-adddatafield)**(const char * szname, double v =0.0) |
| [FEElementData](../Classes/classFEElementData.md) & | **[GetDataField](../Classes/classFEMesh.md#function-getdatafield)**(int i) |
| double | **[ShortestEdge](../Classes/classFEMesh.md#function-shortestedge)**() |
| void | **[BuildNodeElementTable](../Classes/classFEMesh.md#function-buildnodeelementtable)**(vector< vector< int > > & NET) |
| void | **[BuildNodeFaceTable](../Classes/classFEMesh.md#function-buildnodefacetable)**(vector< vector< int > > & NFT) |
| void | **[BuildNodeEdgeTable](../Classes/classFEMesh.md#function-buildnodeedgetable)**(vector< vector< int > > & NET) |
| void | **[BuildEdgeTable](../Classes/classFEMesh.md#function-buildedgetable)**(vector< pair< int, int > > & ET) |
| void | **[BuildNodeNodeTable](../Classes/classFEMesh.md#function-buildnodenodetable)**(vector< set< int > > & NNT) |
| void | **[BuildSurfaceNodeNodeTable](../Classes/classFEMesh.md#function-buildsurfacenodenodetable)**(vector< set< int > > & NNT) |
| void | **[BuildElementEdgeTable](../Classes/classFEMesh.md#function-buildelementedgetable)**(vector< vector< int > > & EET, vector< pair< int, int > > & ET) |
| void | **[BuildFaceTable](../Classes/classFEMesh.md#function-buildfacetable)**(vector< [FEFace](../Classes/classFEFace.md) > & FT) |
| void | **[BuildElementFaceTable](../Classes/classFEMesh.md#function-buildelementfacetable)**(vector< vector< int > > & EFT, vector< [FEFace](../Classes/classFEFace.md) > & FT) |
| void | **[BuildFaceEdgeTable](../Classes/classFEMesh.md#function-buildfaceedgetable)**(vector< vector< int > > & FET, vector< pair< int, int > > & ET) |
| void | **[BuildFaceFaceTable](../Classes/classFEMesh.md#function-buildfacefacetable)**(vector< int > & FFT, vector< [FEFace](../Classes/classFEFace.md) > & FT) |
| void | **[BuildEdgeEdgeTable](../Classes/classFEMesh.md#function-buildedgeedgetable)**(vector< int > & EET, vector< pair< int, int > > & ET) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| bool | **[IntersectTri](../Classes/classFEMesh.md#function-intersecttri)**([vec3d](../Classes/classvec3d.md) * y, [vec3d](../Classes/classvec3d.md) x, [vec3d](../Classes/classvec3d.md) n, [vec3d](../Classes/classvec3d.md) & q, double & g) |
| bool | **[IntersectQuad](../Classes/classFEMesh.md#function-intersectquad)**([vec3d](../Classes/classvec3d.md) * y, [vec3d](../Classes/classvec3d.md) x, [vec3d](../Classes/classvec3d.md) n, [vec3d](../Classes/classvec3d.md) & q, double & g) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| std::vector< [FEElement](../Classes/classFEElement.md) > | **[m_Elem](../Classes/classFEMesh.md#variable-m-elem)** <br>FE elements.  |
| std::vector< double > | **[m_data](../Classes/classFEMesh.md#variable-m-data)** <br>element values  |
| double | **[m_min](../Classes/classFEMesh.md#variable-m-min)**  |
| double | **[m_max](../Classes/classFEMesh.md#variable-m-max)** <br>value range of element data  |
| vector< [FEElementData](../Classes/classFEElementData.md) > | **[m_map](../Classes/classFEMesh.md#variable-m-map)**  |

## Additional inherited members

**Public Functions inherited from [FECoreMesh](../Classes/classFECoreMesh.md)**

|                | Name           |
| -------------- | -------------- |
| | **[FECoreMesh](../Classes/classFECoreMesh.md#function-fecoremesh)**()<br>constructor  |
| virtual | **[~FECoreMesh](../Classes/classFECoreMesh.md#function-~fecoremesh)**()<br>destructor  |
| bool | **[IsType](../Classes/classFECoreMesh.md#function-istype)**(int ntype)<br>check the type of the mesh  |
| int | **[Nodes](../Classes/classFECoreMesh.md#function-nodes)**() const |
| int | **[Edges](../Classes/classFECoreMesh.md#function-edges)**() const |
| int | **[Faces](../Classes/classFECoreMesh.md#function-faces)**() const |
| [FENode](../Classes/classFENode.md) & | **[Node](../Classes/classFECoreMesh.md#function-node)**(int n) |
| [FEEdge](../Classes/classFEEdge.md) & | **[Edge](../Classes/classFECoreMesh.md#function-edge)**(int n) |
| [FEFace](../Classes/classFEFace.md) & | **[Face](../Classes/classFECoreMesh.md#function-face)**(int n) |
| [FENode](../Classes/classFENode.md) * | **[NodePtr](../Classes/classFECoreMesh.md#function-nodeptr)**(int n =0) |
| [FEEdge](../Classes/classFEEdge.md) * | **[EdgePtr](../Classes/classFECoreMesh.md#function-edgeptr)**(int n =0) |
| [FEFace](../Classes/classFEFace.md) * | **[FacePtr](../Classes/classFECoreMesh.md#function-faceptr)**(int n =0) |

**Protected Attributes inherited from [FECoreMesh](../Classes/classFECoreMesh.md)**

|                | Name           |
| -------------- | -------------- |
| std::vector< [FENode](../Classes/classFENode.md) > | **[m_Node](../Classes/classFECoreMesh.md#variable-m-node)** <br>FE nodes.  |
| std::vector< [FEEdge](../Classes/classFEEdge.md) > | **[m_Edge](../Classes/classFECoreMesh.md#variable-m-edge)** <br>FE edges.  |
| std::vector< [FEFace](../Classes/classFEFace.md) > | **[m_Face](../Classes/classFECoreMesh.md#variable-m-face)** <br>FE faces.  |


## Public Functions Documentation

### function FEMesh

```cpp
FEMesh()
```


### function FEMesh

```cpp
FEMesh(
    FEMesh & m
)
```


### function ~FEMesh

```cpp
virtual ~FEMesh()
```


### function Create

```cpp
virtual void Create(
    int nodes,
    int elems,
    int faces =0,
    int edges =0
)
```

allocate space for mesh 

**Reimplements**: [FECoreMesh::Create](../Classes/classFECoreMesh.md#function-create)


### function ShallowCopy

```cpp
void ShallowCopy(
    FEMesh * pm
)
```


### function Clear

```cpp
void Clear()
```

&mdash; C L E A N U P &mdash;

### function DeleteEdges

```cpp
inline void DeleteEdges()
```


### function Elements

```cpp
inline virtual int Elements() const
```

return number of elements 

**Reimplements**: [FECoreMesh::Elements](../Classes/classFECoreMesh.md#function-elements)


### function Element

```cpp
inline FEElement & Element(
    int n
)
```

return element 

### function ElementRef

```cpp
inline virtual FEElement_ & ElementRef(
    int n
)
```

return reference to element 

**Reimplements**: [FECoreMesh::ElementRef](../Classes/classFECoreMesh.md#function-elementref)


### function ElementPtr

```cpp
inline virtual FEElement * ElementPtr(
    int n =0
)
```

return pointer to element 

**Reimplements**: [FECoreMesh::ElementPtr](../Classes/classFECoreMesh.md#function-elementptr)


### function FindFace

```cpp
int FindFace(
    FEElement * pe,
    FEFace & f,
    FEFace & fe
)
```


### function FindNodesFromPart

```cpp
void FindNodesFromPart(
    int gid,
    vector< int > & node
)
```


### function NodeArray

```cpp
inline std::vector< FENode > & NodeArray()
```


**Todo**: Maybe I should delete these 

Get the node array 


### function FaceArray

```cpp
inline std::vector< FEFace > & FaceArray()
```


### function ElementArray

```cpp
inline std::vector< FEElement > & ElementArray()
```


### function GetElementValue

```cpp
inline double GetElementValue(
    int n
)
```


### function SetElementValue

```cpp
inline void SetElementValue(
    int n,
    double v
)
```


### function UpdateValueRange

```cpp
void UpdateValueRange()
```


### function GetValueRange

```cpp
void GetValueRange(
    double & vmin,
    double & vmax
)
```


### function ProjectToSurface

```cpp
vec3d ProjectToSurface(
    vec3d r,
    vec3d t
)
```


### function ProjectToFace

```cpp
vec3d ProjectToFace(
    vec3d p,
    FEFace & f,
    double & r,
    double & s
)
```


### function ProjectToEdge

```cpp
vec3d ProjectToEdge(
    vec3d e1,
    vec3d e2,
    vec3d p,
    double & r
)
```


### function FindIntersection

```cpp
bool FindIntersection(
    FEFace & f,
    vec3d x,
    vec3d n,
    vec3d & q,
    double & g
)
```


### function Update

```cpp
void Update()
```


### function UpdateElementNeighbors

```cpp
void UpdateElementNeighbors()
```


### function UpdateFaces

```cpp
void UpdateFaces()
```


### function UpdateEdges

```cpp
void UpdateEdges()
```


### function UpdateNodes

```cpp
void UpdateNodes()
```


### function AutoSmooth

```cpp
void AutoSmooth(
    double w
)
```


### function UpdateNormals

```cpp
void UpdateNormals()
```


### function PartitionSelection

```cpp
void PartitionSelection()
```


### function RemoveIsolatedNodes

```cpp
void RemoveIsolatedNodes()
```


### function AddNode

```cpp
inline void AddNode(
    FENode & n
)
```


### function DetachSelectedMesh

```cpp
FEMesh * DetachSelectedMesh()
```


### function DetachSelectedPart

```cpp
void DetachSelectedPart()
```


### function ExtractSelectedFaces

```cpp
FEMesh * ExtractSelectedFaces()
```


### function DeleteTaggedElements

```cpp
void DeleteTaggedElements(
    int tag
)
```


### function DeleteTaggedFaces

```cpp
void DeleteTaggedFaces(
    int tag
)
```


### function DeleteTaggedEdges

```cpp
void DeleteTaggedEdges(
    int tag
)
```


### function FindDuplicateFaces

```cpp
void FindDuplicateFaces(
    vector< int > & l
)
```


### function FindDuplicateEdges

```cpp
void FindDuplicateEdges(
    vector< int > & l
)
```


### function DeleteSelectedElements

```cpp
void DeleteSelectedElements()
```


### function DeleteSelectedFaces

```cpp
void DeleteSelectedFaces()
```


### function DeleteSelectedNodes

```cpp
void DeleteSelectedNodes()
```


### function InvertTaggedElements

```cpp
void InvertTaggedElements(
    int ntag
)
```


### function InvertSelectedElements

```cpp
void InvertSelectedElements()
```


### function ShellJacobian

```cpp
double ShellJacobian(
    FEElement & el
)
```


### function RemoveDuplicateElements

```cpp
void RemoveDuplicateElements()
```


### function FixinvertedElements

```cpp
void FixinvertedElements()
```


### function FixReferenceSurface

```cpp
void FixReferenceSurface()
```


### function InterpolateShellThickness

```cpp
void InterpolateShellThickness(
    double 
)
```


### function RemoveNonManifoldElements

```cpp
void RemoveNonManifoldElements()
```


### function FixElementWinding

```cpp
void FixElementWinding()
```


### function FixElementWinding2

```cpp
void FixElementWinding2()
```


### function TagAllElements

```cpp
void TagAllElements(
    int ntag
)
```


### function DataFields

```cpp
inline int DataFields()
```


### function AddDataField

```cpp
FEElementData * AddDataField(
    const char * szname,
    double v =0.0
)
```


### function GetDataField

```cpp
inline FEElementData & GetDataField(
    int i
)
```


### function ShortestEdge

```cpp
double ShortestEdge()
```


### function BuildNodeElementTable

```cpp
void BuildNodeElementTable(
    vector< vector< int > > & NET
)
```


### function BuildNodeFaceTable

```cpp
void BuildNodeFaceTable(
    vector< vector< int > > & NFT
)
```


### function BuildNodeEdgeTable

```cpp
void BuildNodeEdgeTable(
    vector< vector< int > > & NET
)
```


### function BuildEdgeTable

```cpp
void BuildEdgeTable(
    vector< pair< int, int > > & ET
)
```


### function BuildNodeNodeTable

```cpp
void BuildNodeNodeTable(
    vector< set< int > > & NNT
)
```


### function BuildSurfaceNodeNodeTable

```cpp
void BuildSurfaceNodeNodeTable(
    vector< set< int > > & NNT
)
```


### function BuildElementEdgeTable

```cpp
void BuildElementEdgeTable(
    vector< vector< int > > & EET,
    vector< pair< int, int > > & ET
)
```


### function BuildFaceTable

```cpp
void BuildFaceTable(
    vector< FEFace > & FT
)
```


### function BuildElementFaceTable

```cpp
void BuildElementFaceTable(
    vector< vector< int > > & EFT,
    vector< FEFace > & FT
)
```


### function BuildFaceEdgeTable

```cpp
void BuildFaceEdgeTable(
    vector< vector< int > > & FET,
    vector< pair< int, int > > & ET
)
```


### function BuildFaceFaceTable

```cpp
void BuildFaceFaceTable(
    vector< int > & FFT,
    vector< FEFace > & FT
)
```


### function BuildEdgeEdgeTable

```cpp
void BuildEdgeEdgeTable(
    vector< int > & EET,
    vector< pair< int, int > > & ET
)
```


## Protected Functions Documentation

### function IntersectTri

```cpp
bool IntersectTri(
    vec3d * y,
    vec3d x,
    vec3d n,
    vec3d & q,
    double & g
)
```


### function IntersectQuad

```cpp
bool IntersectQuad(
    vec3d * y,
    vec3d x,
    vec3d n,
    vec3d & q,
    double & g
)
```


## Protected Attributes Documentation

### variable m_Elem

```cpp
std::vector< FEElement > m_Elem;
```

FE elements. 

### variable m_data

```cpp
std::vector< double > m_data;
```

element values 

### variable m_min

```cpp
double m_min;
```


### variable m_max

```cpp
double m_max;
```

value range of element data 

### variable m_map

```cpp
vector< FEElementData > m_map;
```


-------------------------------

Updated on 2022-01-01 at 17:29:13 +0000