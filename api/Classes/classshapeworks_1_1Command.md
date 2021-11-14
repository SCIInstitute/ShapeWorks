---
title: shapeworks::Command

---

# shapeworks::Command





Inherited by [shapeworks::GroomCommandGroup](../Classes/classshapeworks_1_1GroomCommandGroup.md), [shapeworks::ImageCommand](../Classes/classshapeworks_1_1ImageCommand.md), [shapeworks::MeshCommand](../Classes/classshapeworks_1_1MeshCommand.md), [shapeworks::OptimizeCommandGroup](../Classes/classshapeworks_1_1OptimizeCommandGroup.md), [shapeworks::ParticleSystemCommand](../Classes/classshapeworks_1_1ParticleSystemCommand.md), [shapeworks::ShapeworksCommand](../Classes/classshapeworks_1_1ShapeworksCommand.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| virtual const std::string | **[type](../Classes/classshapeworks_1_1Command.md#function-type)**() |
| const std::string | **[name](../Classes/classshapeworks_1_1Command.md#function-name)**() const |
| const std::string | **[usage](../Classes/classshapeworks_1_1Command.md#function-usage)**() const |
| const std::string | **[desc](../Classes/classshapeworks_1_1Command.md#function-desc)**() const |
| std::vector< std::string > | **[parse_args](../Classes/classshapeworks_1_1Command.md#function-parse-args)**(const std::vector< std::string > & arguments)<br>parses the arguments for this command, saving them in the parser and returning the leftovers  |
| int | **[run](../Classes/classshapeworks_1_1Command.md#function-run)**([SharedCommandData](../Classes/structshapeworks_1_1SharedCommandData.md) & sharedData)<br>calls execute for this command using the parsed args, returning system exit value  |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| virtual void | **[buildParser](../Classes/classshapeworks_1_1Command.md#function-buildparser)**() |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| optparse::OptionParser | **[parser](../Classes/classshapeworks_1_1Command.md#variable-parser)**  |

## Public Functions Documentation

### function type

```cpp
inline virtual const std::string type()
```


**Reimplemented by**: [shapeworks::ImageCommand::type](../Classes/classshapeworks_1_1ImageCommand.md#function-type), [shapeworks::MeshCommand::type](../Classes/classshapeworks_1_1MeshCommand.md#function-type), [shapeworks::OptimizeCommandGroup::type](../Classes/classshapeworks_1_1OptimizeCommandGroup.md#function-type), [shapeworks::GroomCommandGroup::type](../Classes/classshapeworks_1_1GroomCommandGroup.md#function-type), [shapeworks::ParticleSystemCommand::type](../Classes/classshapeworks_1_1ParticleSystemCommand.md#function-type), [shapeworks::ShapeworksCommand::type](../Classes/classshapeworks_1_1ShapeworksCommand.md#function-type)


### function name

```cpp
inline const std::string name() const
```


### function usage

```cpp
inline const std::string usage() const
```


### function desc

```cpp
inline const std::string desc() const
```


### function parse_args

```cpp
std::vector< std::string > parse_args(
    const std::vector< std::string > & arguments
)
```

parses the arguments for this command, saving them in the parser and returning the leftovers 

### function run

```cpp
int run(
    SharedCommandData & sharedData
)
```

calls execute for this command using the parsed args, returning system exit value 

## Protected Functions Documentation

### function buildParser

```cpp
virtual void buildParser()
```


## Protected Attributes Documentation

### variable parser

```cpp
optparse::OptionParser parser;
```


-------------------------------

Updated on 2021-11-14 at 17:52:10 +0000