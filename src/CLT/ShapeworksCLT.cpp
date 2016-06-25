#include <iostream>
#include <tinyxml.h>
#include <string>
#include <vector>
#include <map>
#include <ShapeWorksGroom.h>
#include <ShapeWorksOptimize.h>
#include "itkNrrdImageIOFactory.h"
#include "itkMetaImageIOFactory.h"

int main(int argc, char ** argv) {
  try {
    if (argc < 4) {
      std::cerr << "Usage: ./ShapeworksCLT PARAMETER_FILE.xml INPUT1 INPUT2 ..." << std::endl;
      return EXIT_FAILURE;
    }
    // setup XML
    TiXmlDocument doc;
    if (!doc.LoadFile(argv[1])) {
      std::cerr << "Failed to load XML: " << argv[1] << "!" << std::endl;
      std::cerr << doc.ErrorDesc() << std::endl;
      return EXIT_FAILURE;
    }
    auto root = doc.FirstChildElement();
    if (!root) {
      std::cerr << "Failed to load file : no root element." << std::endl;
      doc.Clear();
      return EXIT_FAILURE;
    }
    //read the parameters
    std::map<std::string, std::string> parameters;
    for (TiXmlElement * elem = root->FirstChildElement();
    elem != NULL; elem = elem->NextSiblingElement()) {
      auto elemName = std::string(elem->Value());
      auto txt = std::string(elem->FirstChild()->ToText()->Value());
      parameters.insert(std::make_pair(elemName, txt));
    }
    doc.Clear();
    //read the files
    std::vector<ImageType::Pointer> inputs;
    for (size_t i = 2; i < argc; i++) {
      auto fname = std::string(argv[i]);
      if (fname.find(".nrrd") != std::string::npos) {
        itk::NrrdImageIOFactory::RegisterOneFactory();
      } else if (fname.find(".mha") != std::string::npos) {
        itk::MetaImageIOFactory::RegisterOneFactory();
      }
      // read file using ITK
      ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName(fname);
      reader->Update();
      inputs.push_back(reader->GetOutput());
    }
    //default groom options
    bool groom_center = false, groom_isolate = false,
      groom_fill_holes = false, groom_crop = false,
      groom_pad = false, groom_antialias = false,
      groom_fastmarching = false, groom_blur = false;
    size_t groom_antialias_amount = 10, groom_pad_value = 10;
    double groom_fastmarching_isovalue = 0.5, groom_fastmarching_sigma = 2.,
      groom_blur_sigma = 2.;
    //default optimize options
    size_t optimize_scales = 8;
    std::vector<unsigned int> iters;
    std::vector<double> start_reg, end_reg, decay_span, tolerance;
    size_t iters_default = 1000;
    double tolerance_default = 0.01,
      start_reg_default = 100., end_reg_default = 2.,
      decay_span_default = 0., weight = 1.;
    std::string planesFile;
    //collect the values from the parameters
    for (auto a : parameters) {
      //groom parameters
      if (a.first == "groom_center") {
        groom_center = a.second == "true" ? true : false;
      } else if (a.first == "groom_isolate") {
        groom_isolate = a.second == "true" ? true : false;
      } else if (a.first == "groom_fill_holes") {
        groom_fill_holes = a.second == "true" ? true : false;
      } else if (a.first == "groom_crop") {
        groom_crop = a.second == "true" ? true : false;
      } else if (a.first == "groom_pad") {
        groom_pad = a.second == "true" ? true : false;
      } else if (a.first == "groom_antialias") {
        groom_antialias = a.second == "true" ? true : false;
      } else if (a.first == "groom_fastmarching") {
        groom_fastmarching = a.second == "true" ? true : false;
      } else if (a.first == "groom_blur") {
        groom_blur = a.second == "true" ? true : false;
      } else if (a.first == "groom_pad_value") {
        groom_pad_value = std::stoi(a.second);
      } else if (a.first == "groom_antialias_amount") {
        groom_antialias_amount = std::stoi(a.second);
      } else if (a.first == "groom_fastmarching_isovalue") {
        groom_fastmarching_isovalue = std::stod(a.second);
      } else if (a.first == "groom_fastmarching_sigma") {
        groom_fastmarching_sigma = std::stod(a.second);
      } else if (a.first == "groom_blur_sigma") {
        groom_blur_sigma = std::stod(a.second);
        //optimize parameters
      } else if (a.first == "optimize_planes") {
        planesFile = a.second;
      } else if (a.first == "optimize_scales") {
        groom_blur_sigma = std::stoi(a.second);
      } else if (a.first.find("optimize_start_reg") != std::string::npos) {
        if (a.first == "optimize_start_reg") {
          start_reg_default = std::stod(a.second);
        } else {
          start_reg.push_back(std::stod(a.second));
        }
      } else if (a.first.find("optimize_end_reg") != std::string::npos) {
        if (a.first == "optimize_end_reg") {
          end_reg_default = std::stod(a.second);
        } else {
          end_reg.push_back(std::stod(a.second));
        }
      } else if (a.first.find("optimize_iters") != std::string::npos) {
        if (a.first == "optimize_iters") {
          iters_default = std::stoi(a.second);
        } else {
          iters.push_back(std::stoi(a.second));
        }
      } else if (a.first.find("optimize_decay_span") != std::string::npos) {
        if (a.first == "optimize_decay_span") {
          decay_span_default = std::stod(a.second);
        } else {
          decay_span.push_back(std::stod(a.second));
        }
      } else if (a.first.find("optimize_weight") != std::string::npos) {
        weight = std::stod(a.second);
      }
    }
    //run the groom step
    ShapeWorksGroom groom(inputs, 0, 1, groom_blur_sigma,
      groom_pad_value, groom_antialias_amount, true);
    if (groom_center) { groom.queueTool("center"); }
    if (groom_isolate) { groom.queueTool("isolate"); }
    if (groom_fill_holes) { groom.queueTool("hole_fill"); }
    if (groom_crop) { groom.queueTool("auto_crop"); }
    if (groom_pad) { groom.queueTool("auto_pad"); }
    if (groom_antialias) { groom.queueTool("antialias"); }
    if (groom_fastmarching) { groom.queueTool("fastmarching"); }
    if (groom_blur) { groom.queueTool("blur"); }
    groom.run();
    auto groomed_img = groom.getImages();
    //add optimize scale options if not enough
    while (start_reg.size() < optimize_scales) {
      start_reg.push_back(start_reg_default);
    }
    while (end_reg.size() < optimize_scales) {
      end_reg.push_back(end_reg_default);
    }
    while (iters.size() < optimize_scales) {
      iters.push_back(iters_default);
    }
    while (decay_span.size() < optimize_scales) {
      decay_span.push_back(decay_span_default);
    }
    while (tolerance.size() < optimize_scales) {
      tolerance.push_back(tolerance_default);
    }
    //remove optimize scale options if too many
    while (start_reg.size() > optimize_scales) {
      start_reg.pop_back();
    }
    while (end_reg.size() > optimize_scales) {
      end_reg.pop_back();
    }
    while (iters.size() > optimize_scales) {
      iters.pop_back();
    }
    while (decay_span.size() > optimize_scales) {
      decay_span.pop_back();
    }
    while (tolerance.size() > optimize_scales) {
      tolerance.pop_back();
    }
    //get the cutting planes from file if possible
    std::ifstream planes(planesFile.c_str());
    std::vector<std::array<itk::Point<float>, 3 > > cutPlanes;
    while (planes.good()) {
      float x1, y1, z1, x2, y2, z2, x3, y3, z3;
      planes >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3;
      if (!planes.good()) { break; }
      itk::Point<float> p1, p2, p3;
      p1[0] = x1; p1[1] = y1; p1[2] = z1;
      p2[0] = x2; p2[1] = y2; p2[2] = z2;
      p3[0] = x3; p3[1] = y3; p3[2] = z3;
      cutPlanes.push_back({ { p1, p2, p3 } });
    }
    if (cutPlanes.empty() || (cutPlanes.size() > 1 &&
      cutPlanes.size() != groomed_img.size())) {
      std::cerr << "Error reading cutting plane file. Must have 1 " <<
        "set of 3 points, or X set of 3 points, where X is number of samples."
        << std::endl;
      cutPlanes.clear();
    } 
    //run the optimize step
    ShapeWorksOptimize optimize(groomed_img, cutPlanes, optimize_scales,
      start_reg, end_reg, iters, decay_span, weight, true);
    optimize.run();
    auto lpts = optimize.localPoints();
    auto wpts = optimize.globalPoints();
    for (size_t i = 0; i < inputs.size(); i++) {
      auto base = std::string(argv[i + 2]);
      if (base.find(".") != std::string::npos) {
        base = base.substr(0, base.find_last_of("."));
      }
      std::ofstream outl(base + std::string(".lpts"));
      std::ofstream outw(base + std::string(".wpts"));
      for (size_t j = 0; j < lpts[i].size(); j++) {
        for (size_t k = 0; k < lpts[i][j].Size(); k++) {
          outl << lpts[i][j][k] << std::endl;
          outw << wpts[i][j][k] << std::endl;
        }
      }
      outl.close();
      outw.close();
    }
  } catch (std::exception e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Done!" << std::endl;
  return EXIT_SUCCESS;
}