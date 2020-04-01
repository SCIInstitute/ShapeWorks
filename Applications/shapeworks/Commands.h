#pragma once

#include "Command.h"

namespace shapeworks {

// boilerplate for a command. Copy this to start a new command
#if 0
///////////////////////////////////////////////////////////////////////////////
class Example : public ImageCommand < --be sure to derive from the appropriate type
{
public:
  static Example& getCommand() { static Example instance; return instance; }

private:
  Example() { buildParser(); } // purposely private ctor so only the single instance can be retrieved
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};
#endif // if 0

///////////////////////////////////////////////////////////////////////////////
class ReadImage : public ImageCommand
{
public:
  static ReadImage& getCommand() { static ReadImage instance; return instance; }

private:
  ReadImage() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class WriteImage : public ImageCommand
{
public:
  static WriteImage& getCommand() { static WriteImage instance; return instance; }

private:
  WriteImage() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class ReadMesh : public MeshCommand
{
public:
  static ReadMesh& getCommand() { static ReadMesh instance; return instance; }

private:
  ReadMesh() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class WriteMesh : public MeshCommand
{
public:
  static WriteMesh& getCommand() { static WriteMesh instance; return instance; }

private:
  WriteMesh() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Antialias : public ImageCommand
{
public:
  static Antialias& getCommand() { static Antialias instance; return instance; }

private:
  Antialias() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class ResampleImage : public ImageCommand
{
public:
  static ResampleImage& getCommand() { static ResampleImage instance; return instance; }

private:
  ResampleImage() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class PadImage : public ImageCommand
{
public:
  static PadImage& getCommand() { static PadImage instance; return instance; }

private:
  PadImage() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class SmoothMesh : public MeshCommand
{
public:
  static SmoothMesh& getCommand() { static SmoothMesh instance; return instance; }

private:
  SmoothMesh() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class RecenterImage : public ImageCommand
{
public:
  static RecenterImage& getCommand() { static RecenterImage instance; return instance; }

private:
  RecenterImage() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Coverage : public MeshCommand
{
public:
  static Coverage& getCommand() { static Coverage instance; return instance; }

private:
  Coverage() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Translate : public TransformCommand
{
public:
  static Translate &getCommand() { static Translate instance; return instance; }

private:
  Translate() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class ApplyTransform : public ImageCommand
{
public:
  static ApplyTransform &getCommand() { static ApplyTransform instance; return instance; }

private:
  ApplyTransform() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class ExtractLabel : public ImageCommand
{
public:
  static ExtractLabel& getCommand() { static ExtractLabel instance; return instance; }

private:
  ExtractLabel() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class CloseHoles : public ImageCommand
{
public:
  static CloseHoles& getCommand() { static CloseHoles instance; return instance; }

private:
  CloseHoles() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Threshold : public ImageCommand
{
public:
  static Threshold& getCommand() { static Threshold instance; return instance; }

private:
  Threshold() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class ComputeDT : public ImageCommand
{
public:
  static ComputeDT &getCommand()
  {
    static ComputeDT instance;
    return instance;
  }

private:
  ComputeDT() { buildParser(); } 
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Curvature : public ImageCommand
{
public:
  static Curvature& getCommand() { static Curvature instance; return instance; }

private:
  Curvature() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Gradient : public ImageCommand
{
public:
  static Gradient& getCommand() { static Gradient instance; return instance; }

private:
  Gradient() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Sigmoid : public ImageCommand
{
public:
  static Sigmoid& getCommand() { static Sigmoid instance; return instance; }

private:
  Sigmoid() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class SetLevel : public ImageCommand
{
public:
  static SetLevel& getCommand() { static SetLevel instance; return instance; }

private:
  SetLevel() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Blur : public ImageCommand
{
public:
  static Blur &getCommand() { static Blur instance; return instance; }

private:
  Blur() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class BoundingBox : public ImageCommand
{
public:
  static BoundingBox &getCommand()
  {
    static BoundingBox instance;
    return instance;
  }

private:
  BoundingBox() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class CropImage : public ImageCommand
{
public:
  static CropImage &getCommand() { static CropImage instance; return instance; }

private:
  CropImage() { buildParser(); } 
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

} // shapeworks
