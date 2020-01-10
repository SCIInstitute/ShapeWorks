#pragma once

#include "Command.h"

namespace Shapeworks {



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
class Smoothmesh : public MeshCommand
{
public:
  static Smoothmesh& getCommand() { static Smoothmesh instance; return instance; }

private:
  Smoothmesh() { buildParser(); }
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

} // Shapeworks
