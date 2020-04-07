#pragma once

#include "Command.h"

namespace shapeworks {
///////////////////////////////////////////////////////////////////////////////
class ReadParticleSystem : public ParticleSystemCommand
{
public:
  static ReadParticleSystem &getCommand()
  {
    static ReadParticleSystem instance;
    return instance;
  }

private:
  ReadParticleSystem() { buildParser(); }

  void buildParser() override;

  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

class Compactness : public ParticleSystemCommand
{
public:
  static Compactness &getCommand()
  {
    static Compactness instance;
    return instance;
  }

private:
  Compactness() { buildParser(); }

  void buildParser() override;

  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

class Generalization : public ParticleSystemCommand
{
public:
  static Generalization &getCommand()
  {
    static Generalization instance;
    return instance;
  }

private:
  Generalization() { buildParser(); }

  void buildParser() override;

  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};

class Specificity : public ParticleSystemCommand
{
public:
  static Specificity &getCommand()
  {
    static Specificity instance;
    return instance;
  }

private:
  Specificity() { buildParser(); }

  void buildParser() override;

  int execute(const optparse::Values &options, SharedCommandData &sharedData) override;
};
}
