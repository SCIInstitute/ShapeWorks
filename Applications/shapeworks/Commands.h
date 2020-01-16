#pragma once

#include "Command.h"
#include "ShapeCollection.h"

namespace shapeworks {



// boilerplate for a command. Copy this to start a new command
#if 0
///////////////////////////////////////////////////////////////////////////////
class Example : public ImageCommand  <-- be sure to derive from the appropriate type
{
public:
  static Example& getCommand() { static Example instance; return instance; }

private:
  Example() { buildParser(); } // purposely private ctor so only the single instance can be retrieved
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
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
class Binarize : public ImageCommand
{
public:
  static Binarize& getCommand() { static Binarize instance; return instance; }

private:
  Binarize() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
//TODO: find out how to organize this from Cam/Archna
class EvaluateSSM : public Command
{
public:
    static EvaluateSSM& getCommand() { static EvaluateSSM instance; return instance; }

private:
    ShapeCollection shapes;
    EvaluateSSM() { buildParser(); }
    void buildParser() override;
    int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};


} // shapeworks
