#pragma once

namespace shapeworks {

/// print buffer info for the given array (dims, format, strides, etc)
void printNumpyArrayInfo(const py::array& np_array) {
  // get input array info
  auto info = np_array.request();

  /*
    struct buffer_info {
    void *ptr;
    py::ssize_t itemsize;
    std::string format;
    py::ssize_t ndim;
    std::vector<py::ssize_t> shape;
    std::vector<py::ssize_t> strides;
    };
  */

  std::cout << "buffer info: \n"
            << "\tinfo.ptr: " << info.ptr << std::endl
            << "writeable: " << np_array.writeable() << std::endl
            << "owns data: " << np_array.owndata() << std::endl
            << "\tinfo.itemsize: " << info.itemsize << std::endl
            << "\tinfo.format: " << info.format << std::endl
            << "\tinfo.ndim: " << info.ndim << std::endl;
  std::cout << "shape ([z][y]x): ";
  for (auto& n: info.shape) {
    std::cout << n << " ";
  }
  std::cout << "\nstrides ([z][y]x): ";
  for (auto& n: info.strides) {
    std::cout << n << " ";
  }
  std::cout << std::endl;
}

/// verify py::array has expected order and is densely packed, throw if not
void verifyOrderAndPacking(const py::array& np_array) {
  auto info = np_array.request();

  // verify it's C order, not Fortran order
  auto c_order = pybind11::detail::array_proxy(np_array.ptr())->flags & pybind11::detail::npy_api::NPY_ARRAY_C_CONTIGUOUS_;
  if (!c_order) {
    throw std::invalid_argument("array must be C_CONTIGUOUS; use numpy.transpose() to reorder");
  }

  // verify data is densely packed by checking strides is same as shape
  std::vector<py::ssize_t> strides(info.ndim, info.itemsize);
  for (int i = 0; i < info.ndim-1; i++) {
    for (int j = i+1; j < info.ndim; j++) {
      strides[i] *= info.shape[j];
    }
  }

  for (int i = 0; i < info.ndim; i++) {
    if (info.strides[i] != strides[i]) {
      throw std::invalid_argument(std::string("array not densely packed in ") + std::to_string(i) +
                                  std::string("th dimension: expected ") + std::to_string(strides[i]) +
                                  std::string(" strides, not ") + std::to_string(info.strides[i]));
    }
  }
}

/// sets the OWNDATA flag of the given array to `owns`
void setOwnership(py::array& array, bool owns) {
  std::bitset<32> own_data_flag(pybind11::detail::npy_api::NPY_ARRAY_OWNDATA_);
  if (!owns) {
    int disown_data_flag = static_cast<int>(~own_data_flag.to_ulong());
    pybind11::detail::array_proxy(array.ptr())->flags &= disown_data_flag;
  }
  else {
    pybind11::detail::array_proxy(array.ptr())->flags |= static_cast<int>(own_data_flag.to_ulong());
  }

  if (array.owndata() != owns) {
    throw std::runtime_error("error modifying python array ownership");
  }
}

/// helper function for Image.init and Image.assign
Image::ImageType::Pointer wrapNumpyArr(py::array& np_array) {
  //printNumpyArrayInfo(np_array);

  // get input array info
  auto info = np_array.request();

  // verify it's 3d
  if (info.ndim != 3) {
    throw std::invalid_argument(std::string("array must be 3d, but ndim = ") + std::to_string(info.ndim));
  }

  // verify py::array (throws on error)
  verifyOrderAndPacking(np_array);

  // array must be dtype.float32 and own its data to transfer it to Image
  if (info.format != py::format_descriptor<Image::PixelType>::format()) {
    // inform the user how to create correct type array rather than copy
    throw std::invalid_argument("array must be same dtype as Image; convert using `np.array(arr, dtype=np.float32)`");
  }
  if (!np_array.owndata()) {
    throw std::invalid_argument("error: numpy array does not own data (see `arr.flags()`) to be transferred to Image");
  }

  // Pass ownership of the array to Image to prevent Python from
  // deallocating (the shapeworks Image will dealloate when it's time).
  setOwnership(np_array, false);

  // import data, passing ownership of memory to ensure there will be no leak
  using ImportType = itk::ImportImageFilter<Image::PixelType, 3>;
  auto importer = ImportType::New();

  ImportType::SizeType size; // i.e., Dims (remember numpy orders zyx)
  size[0] = np_array.shape()[2];
  size[1] = np_array.shape()[1];
  size[2] = np_array.shape()[0];

  assert(size[0]*size[1]*size[2]*sizeof(Image::PixelType) == np_array.size());
  importer->SetImportPointer(static_cast<Image::PixelType *>(info.ptr),
                             size[0]*size[1]*size[2]*sizeof(Image::PixelType),
                             true /*importer take_ownership*/);
  ImportType::IndexType start({0,0,0}); // i.e., Coord
  ImportType::RegionType region;
  region.SetIndex(start);
  region.SetSize(size);
  importer->SetRegion(region);
  importer->Update();
  return importer->GetOutput();
}

/// converts py::array to vtkDataArray, optionally taking ownership of data
Array pyToArr(py::array& np_array, bool take_ownership = true) {
  //printNumpyArrayInfo(np_array);

  //
  // Verify the data is of appropriate size, shape, type, and ownership.
  //
  // get input array info
  auto info = np_array.request();

  // verify py::array (throws on error)
  verifyOrderAndPacking(np_array);

  // verify format
  if (!(info.format == py::format_descriptor<float>::format() ||
        info.format == py::format_descriptor<double>::format())) {
    throw std::invalid_argument(std::string("numpy dtype ") + std::string(info.format) + std::string(" not yet accepted (currently only float32 and float64) (i.e., " + py::format_descriptor<float>::format()) + " and " + py::format_descriptor<double>::format() + ")");
  }

  // verify dims (ex: 2d is an array of vectors, 1d is an array of scalars)
  if (info.ndim < 1 || info.ndim > 2) {
    throw std::invalid_argument(std::string("array must be either 1d or 2d, but ndim = ") + std::to_string(info.ndim));
  }

  // array must own its data to transfer it to Image
  // NOTE: it could be shared, but this avoids a potential dangling pointer
  if (take_ownership && !np_array.owndata()) {
    throw std::invalid_argument("numpy array must own the data to be transferred to Mesh (maybe pass `arr.copy()`)");
  }

  //
  // Create the vtkDataArray and pass the numpy data in.
  //
  // determine nvalues, ncomponents
  auto nvalues = info.shape[0];
  auto ncomponents = info.ndim > 1 ? info.shape[1] : 1;

  // create vtkDataArray pointer, set number of components, allocate and pass data
  auto vtkarr = Array();
  if (info.format == py::format_descriptor<float>::format()) {
    auto arr = vtkFloatArray::New();
    arr->SetArray(static_cast<float*>(info.ptr), nvalues * ncomponents, !take_ownership /*0 passes ownership*/);
    vtkarr = arr;
  }
  else if (info.format == py::format_descriptor<double>::format()) {
    auto arr = vtkDoubleArray::New();
    arr->SetArray(static_cast<double*>(info.ptr), nvalues * ncomponents, !take_ownership /*0 passes ownership*/);
    vtkarr = arr;
  }
  else {
    throw std::invalid_argument("numpy dtype not yet accepted (currently only float32 and float64)");
    // Other options: vtkUnsignedShortArray, vtkUnsignedLongLongArray, vtkUnsignedLongArray, vtkUnsignedIntArray, vtkUnsignedCharArray, vtkSignedCharArray, vtkShortArray, vtkLongLongArray, vtkLongArray, vtkIntArray, vtkIdTypeArray, vtkFloatArray, vtkDoubleArray, vtkCharArray, and vtkBitArray.
  }
  vtkarr->SetNumberOfComponents(ncomponents);

  // prevent Python from deallocating since vtk will do that when it's time
  if (take_ownership) {
    setOwnership(np_array, false);
  }

  return vtkarr;
}

/// ways of tranferring Arrays to Python, copy being the least efficient but most conservative
enum ArrayTransferOptions {
  COPY_ARRAY,  // copies and (by definition) grants ownership
  SHARE_ARRAY, // does not copy or grant ownership
  MOVE_ARRAY   // does not copy, grants ownership if possible
};

/// convert a vtkDataArray (AOS assumed) to a py::array using specified means of transfer
py::array arrToPy(Array& array, ArrayTransferOptions xfer = COPY_ARRAY) {
  const size_t elemsize = array->GetElementComponentSize();
  auto shape = std::vector<size_t> { static_cast<size_t>(array->GetNumberOfTuples()) };
  if (array->GetNumberOfComponents() > 1) {
    shape.push_back(static_cast<size_t>(array->GetNumberOfComponents()));
  }
  auto strides = std::vector<size_t>();
  if (array->GetNumberOfComponents() > 1) {
    strides = std::vector<size_t> {
      static_cast<size_t>(array->GetNumberOfComponents() * elemsize),
      elemsize
    };
  }
  else {
    strides = std::vector<size_t> { elemsize };
  }

  py::dtype py_type;
  if (vtkDoubleArray::SafeDownCast(array)) {
    py_type = py::dtype::of<double>();
  }
  else if (vtkFloatArray::SafeDownCast(array)) {
    py_type = py::dtype::of<float>();
  }
  else {
    throw std::invalid_argument("arrToPy passed currently unhandled array type");
    // Other options: vtkUnsignedShortArray, vtkUnsignedLongLongArray, vtkUnsignedLongArray, vtkUnsignedIntArray, vtkUnsignedCharArray, vtkSignedCharArray, vtkShortArray, vtkLongLongArray, vtkLongArray, vtkIntArray, vtkIdTypeArray, vtkFloatArray, vtkDoubleArray, vtkCharArray, and vtkBitArray.
  }
#if 0
  std::cout << "type of array: " << typeid(array).name() << std::endl
            << "X (num_components): " << array->GetNumberOfComponents() << std::endl
            << "Y (num_tuples): " << array->GetNumberOfTuples() << std::endl
            << "sizeof(element): " << array->GetElementComponentSize() << std::endl
            << "py_type: " << py_type.kind() << std::endl
            << "size: " << py_type.itemsize() << std::endl;
#endif

  py::str dummyDataOwner;
  py::array img{
    py_type,
    shape,
    strides,
    array->GetVoidPointer(0),
    (xfer == COPY_ARRAY ? pybind11::handle() : dummyDataOwner)
  };

  if (xfer == MOVE_ARRAY) {
    if (array->GetReferenceCount() == 1) {
      array->SetReferenceCount(2); // NOTE: tricks vtk into never deleting this array
      setOwnership(img, true);
    }
    else {
      // If array has other references, it will only be shared with Python.
      std::cerr << "NOTE: sharing array (unable to transfer ownership from C++)" << std::endl;
    }
  }

  // set c-contiguous and not f-contiguous, not both (i.e., "NPY_ARRAY_FORCECAST_")
  std::bitset<32> f_order_flag = pybind11::detail::npy_api::NPY_ARRAY_F_CONTIGUOUS_;
  f_order_flag = ~f_order_flag;
  int f_order_flag_int = static_cast<int>(f_order_flag.to_ulong());
  pybind11::detail::array_proxy(img.ptr())->flags &= f_order_flag_int;
  pybind11::detail::array_proxy(img.ptr())->flags |= pybind11::detail::npy_api::NPY_ARRAY_C_CONTIGUOUS_;

  return img;
}

}
