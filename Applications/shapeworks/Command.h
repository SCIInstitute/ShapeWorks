#pragma once

/*
 * Command provided by unified shapeworks executable.
 */

#include "OptionParser.h"
#include "SharedCommandData.h"

#include <iostream>
#include <stdexcept>


namespace Shapeworks {

class Command {
public:
  const std::string name() const { return parser.prog(); }
  const std::string usage() const { return parser.get_usage(); }
  const std::string desc() const { return parser.description(); }

  std::vector<std::string> parse_args(const std::vector<std::string> &arguments);
  int run(SharedCommandData &sharedData);

private:
  virtual bool read(const std::string inFilename, SharedCommandData &sharedData) = 0;
  virtual bool write(const std::string outFilename, SharedCommandData &sharedData) = 0;
  virtual int execute(const optparse::Values &options, SharedCommandData &sharedData) = 0;

protected:
  virtual void buildParser(); // derived classes should specialize and call this as well

  optparse::OptionParser parser;
};

class ImageCommand : public Command
{
private:
  bool read(const std::string inFilename, SharedCommandData &sharedData) override;
  bool write(const std::string outFilename, SharedCommandData &sharedData) override;
};

class MeshCommand : public Command
{
private:
  bool read(const std::string inFilename, SharedCommandData &sharedData) override;
  bool write(const std::string outFilename, SharedCommandData &sharedData) override;
};

}; // Shapeworks


std::ostream& operator<<(std::ostream& os, const Shapeworks::Command &cmd);

