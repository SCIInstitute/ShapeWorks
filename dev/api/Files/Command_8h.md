---
title: Applications/shapeworks/Command.h

---

# Applications/shapeworks/Command.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Command](../Classes/classshapeworks_1_1Command.md)**  |
| class | **[shapeworks::ImageCommand](../Classes/classshapeworks_1_1ImageCommand.md)**  |
| class | **[shapeworks::MeshCommand](../Classes/classshapeworks_1_1MeshCommand.md)**  |
| class | **[shapeworks::OptimizeCommandGroup](../Classes/classshapeworks_1_1OptimizeCommandGroup.md)**  |
| class | **[shapeworks::GroomCommandGroup](../Classes/classshapeworks_1_1GroomCommandGroup.md)**  |
| class | **[shapeworks::AnalyzeCommandGroup](../Classes/classshapeworks_1_1AnalyzeCommandGroup.md)**  |
| class | **[shapeworks::ProjectCommandGroup](../Classes/classshapeworks_1_1ProjectCommandGroup.md)**  |
| class | **[shapeworks::ParticleSystemCommand](../Classes/classshapeworks_1_1ParticleSystemCommand.md)**  |
| class | **[shapeworks::ShapeworksCommand](../Classes/classshapeworks_1_1ShapeworksCommand.md)**  |

## Functions

|                | Name           |
| -------------- | -------------- |
| std::ostream & | **[operator<<](../Files/Command_8h.md#function-operator<<)**(std::ostream & os, const [shapeworks::Command](../Classes/classshapeworks_1_1Command.md) & cmd) |

## Defines

|                | Name           |
| -------------- | -------------- |
|  | **[COMMAND_DECLARE](../Files/Command_8h.md#define-command-declare)**(CommandName, CommandType)  |


## Functions Documentation

### function operator<<

```cpp
std::ostream & operator<<(
    std::ostream & os,
    const shapeworks::Command & cmd
)
```




## Macros Documentation

### define COMMAND_DECLARE

```cpp
#define COMMAND_DECLARE(
    CommandName,
    CommandType
)
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
```


## Source code

```cpp
#pragma once

/*
 * Command provided by unified shapeworks executable.
 */

#include "OptionParser.h"
#include "SharedCommandData.h"

#include <iostream>
#include <stdexcept>

#define COMMAND_DECLARE(CommandName, CommandType)                                          \
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
  
  std::vector<std::string> parse_args(const std::vector<std::string> &arguments);

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

class OptimizeCommandGroup : public Command
{
public:
  const std::string type() override { return "Optimize"; }

private:
};

class GroomCommandGroup : public Command
{
public:
  const std::string type() override { return "Groom"; }

private:
};

class AnalyzeCommandGroup : public Command
{
public:
  const std::string type() override { return "Analyze"; }

private:
};

class ProjectCommandGroup : public Command
{
public:
  const std::string type() override { return "Project"; }

private:
};

class ParticleSystemCommand : public Command
{
public:
  const std::string type() override { return "ParticleSystem"; }

private:
};

class ShapeworksCommand : public Command
{
public:
  const std::string type() override { return "Shapeworks"; }

private:
};

}; // shapeworks

std::ostream& operator<<(std::ostream& os, const shapeworks::Command &cmd);
```


-------------------------------

Updated on 2024-10-16 at 07:18:27 +0000
