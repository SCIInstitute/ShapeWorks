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
class Resamplevolume : public ImageCommand
{
public:
  static Resamplevolume& getCommand() { static Resamplevolume instance; return instance; }

private:
  Resamplevolume() { buildParser(); }
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

///////////////////////////////////////////////////////////////////////////////
class Padvolume : public ImageCommand
{
public:
  static Padvolume& getCommand() { static Padvolume instance; return instance; }

private:
  Padvolume() { buildParser(); }
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


} // Shapeworks
