#pragma once

#include "Command.h"

namespace Shapeworks {

///////////////////////////////////////////////////////////////////////////////
class Antialias : public ImageCommand
{
public:
  static Antialias& getCommand()
  {
    static Antialias instance;

    /*debug stuff*/
    std::cout << "created Antialias command: \n\tname: " << instance.name() << "\n\tdesc: " << instance.desc()
              << "\n\tusage: " << instance.usage() << std::endl;
    

    /*end debug stuff*/

    return instance;
  }

private:
  Antialias() { buildParser(); } // purposely private ctor so only the single instance can be retrieved
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;

};

///////////////////////////////////////////////////////////////////////////////
class Reshapevolume : public ImageCommand
{
public:
  static Reshapevolume& getCommand()
  {
    static Reshapevolume instance;

    /*debug stuff*/
    std::cout << "created Reshapevolume command: \n\tname: " << instance.name() << "\n\tdesc: " << instance.desc()
              << "\n\tusage: " << instance.usage() << std::endl;
    

    /*end debug stuff*/

    return instance;
  }

private:
  Reshapevolume() { buildParser(); } // purposely private ctor so only the single instance can be retrieved
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;

};

///////////////////////////////////////////////////////////////////////////////
class Smoothmesh : public MeshCommand
{
public:
  static Smoothmesh& getCommand()
  {
    static Smoothmesh instance;

    /*debug stuff*/
    std::cout << "created Smoothmesh command: \n\tname: " << instance.name() << "\n\tdesc: " << instance.desc()
              << "\n\tusage: " << instance.usage() << std::endl;
    

    /*end debug stuff*/

    return instance;
  }

private:
  Smoothmesh() { buildParser(); } // purposely private ctor so only the single instance can be retrieved
  void buildParser() override;
  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;

};

} // Shapeworks
