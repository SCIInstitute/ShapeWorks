#pragma once

/*
 * Command provided by unified shapeworks executable.
 */

#include "OptionParser.h"
#include "SharedCommandData.h"

#include <iostream>
#include <stdexcept>


namespace shapeworks {

class Command {
public:
  virtual const std::string type() { return "General"; }

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
public:
  const std::string type() override { return "Image"; }

private:
  bool read(const std::string inFilename, SharedCommandData &sharedData) override;
  bool write(const std::string outFilename, SharedCommandData &sharedData) override;
};

class MeshCommand : public Command
{
public:
  const std::string type() override { return "Mesh"; }

private:
  bool read(const std::string inFilename, SharedCommandData &sharedData) override;
  bool write(const std::string outFilename, SharedCommandData &sharedData) override;
};

// TODO: something like this for a command that reads a list of files, computes a transform from all of them (or say, the average of the input images), then writes the single result (or lets it be reused for downstream commands)
// NOTE: for most of the existing commands that take a list of files, they could (and should) be executed once for each file, so this type of command should only be used for operations that *require* a list of files to compute a given result, and the others should simply be called once for each file in a given list.
class MultiImageCommand : public Command
{
public:
  const std::string type() override { return "Multi-image"; }

private:
  bool read(const std::string filelist, SharedCommandData &sharedData) override;
  bool write(const std::string outFilename, SharedCommandData &sharedData) override;
};

}; // Shapeworks


std::ostream& operator<<(std::ostream& os, const Shapeworks::Command &cmd);

