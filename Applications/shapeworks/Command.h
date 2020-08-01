#pragma once

/*
 * Command provided by unified shapeworks executable.
 */

#include "OptionParser.h"
#include "SharedCommandData.h"

#include <iostream>
#include <stdexcept>

#define COMMAND_DECLARE(CommandName, CommandType)                                           \
  class CommandName : public CommandType                                                   \
  {                                                                                        \
  public:                                                                                  \
    static CommandName &getCommand() { static CommandName instance; return instance; }     \
                                                                                           \
  private:                                                                                 \
    CommandName() { buildParser(); }                                                       \
    void buildParser() override;                                                           \
    bool execute(const optparse::Values &options, SharedCommandData &sharedData) override; \
  }

namespace shapeworks {

class Command {
public:
  virtual const std::string type() { return "General"; }

  const std::string name() const { return parser.prog(); }
  const std::string usage() const { return parser.get_usage(); }
  const std::string desc() const { return parser.description(); }
  
  /// parses the arguments for this command, saving them in the parser and returning the leftovers
  std::vector<std::string> parse_args(const std::vector<std::string> &arguments);

  /// calls execute for this command using the parsed args, returning system exit value
  int run(SharedCommandData &sharedData);

private:
  virtual bool execute(const optparse::Values &options, SharedCommandData &sharedData) = 0;

protected:
  virtual void buildParser(); // derived classes should specialize and call this as well

  optparse::OptionParser parser;
};

class ImageCommand : public Command
{
public:
  const std::string type() override { return "Image"; }

private:
};

class MeshCommand : public Command
{
public:
  const std::string type() override { return "Mesh"; }

private:
};

class ParticleSystemCommand : public Command
{
public:
  const std::string type() override { return "ParticleSystem"; }

private:
};

}; // shapeworks

std::ostream& operator<<(std::ostream& os, const shapeworks::Command &cmd);
