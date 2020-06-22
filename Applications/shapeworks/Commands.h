#pragma once

#include "Command.h"

namespace shapeworks {

// boilerplate for a command. Copy this to start a new command
#if 0
///////////////////////////////////////////////////////////////////////////////
class Example : public ImageCommand <--be sure to derive from the appropriate type
{
public:
  static Example& getCommand() { static Example instance; return instance; }

private:
  Example() { buildParser(); } // purposely private ctor so only the single instance can be retrieved
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};
#endif 

///////////////////////////////////////////////////////////////////////////////
class ReadImage : public ImageCommand
{
public:
  static ReadImage& getCommand() { static ReadImage instance; return instance; }

private:
  ReadImage() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class WriteImage : public ImageCommand
{
public:
  static WriteImage& getCommand() { static WriteImage instance; return instance; }

private:
  WriteImage() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class ImageInfo : public ImageCommand
{
public:
  static ImageInfo& getCommand() { static ImageInfo instance; return instance; }

private:
  ImageInfo() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Antialias : public ImageCommand
{
public:
  static Antialias& getCommand() { static Antialias instance; return instance; }

private:
  Antialias() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class ResampleImage : public ImageCommand
{
public:
  static ResampleImage& getCommand() { static ResampleImage instance; return instance; }

private:
  ResampleImage() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class RecenterImage : public ImageCommand
{
public:
  static RecenterImage &getCommand()
  {
    static RecenterImage instance;
    return instance;
  }

private:
  RecenterImage() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class PadImage : public ImageCommand
{
public:
  static PadImage& getCommand() { static PadImage instance; return instance; }

private:
  PadImage() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Translate : public ImageCommand
{
public:
  static Translate &getCommand() { static Translate instance; return instance; }

private:
  Translate() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Scale : public ImageCommand
{
public:
  static Scale &getCommand() { static Scale instance; return instance; }

private:
  Scale() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Rotate : public ImageCommand
{
public:
  static Rotate &getCommand() { static Rotate instance; return instance; }

private:
  Rotate() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class ExtractLabel : public ImageCommand
{
public:
  static ExtractLabel& getCommand() { static ExtractLabel instance; return instance; }

private:
  ExtractLabel() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class CloseHoles : public ImageCommand
{
public:
  static CloseHoles& getCommand() { static CloseHoles instance; return instance; }

private:
  CloseHoles() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Binarize : public ImageCommand
{
public:
  static Binarize& getCommand() { static Binarize instance; return instance; }

private:
  Binarize() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class ComputeDT : public ImageCommand
{
public:
  static ComputeDT &getCommand() { static ComputeDT instance; return instance; }

private:
  ComputeDT() { buildParser(); } 
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class CurvatureFilter : public ImageCommand
{
public:
  static CurvatureFilter& getCommand() { static CurvatureFilter instance; return instance; }

private:
  CurvatureFilter() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class GradientFilter : public ImageCommand
{
public:
  static GradientFilter& getCommand() { static GradientFilter instance; return instance; }

private:
  GradientFilter() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class SigmoidFilter : public ImageCommand
{
public:
  static SigmoidFilter& getCommand() { static SigmoidFilter instance; return instance; }

private:
  SigmoidFilter() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class TPLevelSetFilter : public ImageCommand
{
public:
  static TPLevelSetFilter& getCommand() { static TPLevelSetFilter instance; return instance; }

private:
  TPLevelSetFilter() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class TopologyPreservingFilter : public ImageCommand
{
public:
  static TopologyPreservingFilter& getCommand() { static TopologyPreservingFilter instance; return instance; }

private:
  TopologyPreservingFilter() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Blur : public ImageCommand
{
public:
  static Blur &getCommand() { static Blur instance; return instance; }

private:
  Blur() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class BoundingBox : public ImageCommand
{
public:
  static BoundingBox &getCommand() { static BoundingBox instance; return instance; }

private:
  BoundingBox() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class CropImage : public ImageCommand
{
public:
  static CropImage &getCommand() { static CropImage instance; return instance; }

private:
  CropImage() { buildParser(); } 
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class ICPRigid : public ImageCommand
{
public:
  static ICPRigid &getCommand() { static ICPRigid instance; return instance; }

private:
  ICPRigid() { buildParser(); } 
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class ClipVolume : public ImageCommand 
{
public:
  static ClipVolume &getCommand() { static ClipVolume instance; return instance; }

private:
  ClipVolume() { buildParser(); } 
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class ReflectVolume : public ImageCommand
{
public:
  static ReflectVolume &getCommand() { static ReflectVolume instance; return instance; }

private:
  ReflectVolume() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class SetOrigin : public ImageCommand 
{
public:
  static SetOrigin &getCommand() { static SetOrigin instance; return instance; }

private:
  SetOrigin() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class WarpImage : public ImageCommand
{
public:
  static WarpImage &getCommand()
  {
    static WarpImage instance;
    return instance;
  }

private:
  WarpImage() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Compare : public ImageCommand
{
public:
  static Compare &getCommand() { static Compare instance; return instance; }

private:
  Compare() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Filter : public ImageCommand
{
public:
  static Filter &getCommand() { static Filter instance; return instance; }

private:
  Filter() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class NegateImage : public ImageCommand
{
public:
  static NegateImage& getCommand() { static NegateImage instance; return instance; }

private:
  NegateImage() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class AddImage : public ImageCommand
{
public:
  static AddImage& getCommand() { static AddImage instance; return instance; }

private:
  AddImage() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class SubtractImage : public ImageCommand
{
public:
  static SubtractImage& getCommand() { static SubtractImage instance; return instance; }

private:
  SubtractImage() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class ReadParticleSystem : public ParticleSystemCommand
{
public:
  static ReadParticleSystem &getCommand() { static ReadParticleSystem instance; return instance; }

private:
  ReadParticleSystem() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Compactness : public ParticleSystemCommand
{
public:
  static Compactness &getCommand() { static Compactness instance; return instance; }

private:
  Compactness() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Generalization : public ParticleSystemCommand
{
public:
  static Generalization &getCommand() { static Generalization instance; return instance; }

private:
  Generalization() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Specificity : public ParticleSystemCommand
{
public:
  static Specificity &getCommand() { static Specificity instance; return instance; }

private:
  Specificity() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class ReadMesh : public MeshCommand
{
public:
  static ReadMesh &getCommand() { static ReadMesh instance; return instance; }

private:
  ReadMesh() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class WriteMesh : public MeshCommand
{
public:
  static WriteMesh &getCommand() { static WriteMesh instance; return instance; }

private:
  WriteMesh() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Coverage : public MeshCommand
{
public:
  static Coverage &getCommand() { static Coverage instance; return instance; }

private:
  Coverage() { buildParser(); }
  void buildParser() override;
  bool execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

} // shapeworks
