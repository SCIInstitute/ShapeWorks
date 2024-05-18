---
title: shapeworks::AnalyzeCommandGroup

---

# shapeworks::AnalyzeCommandGroup





Inherits from [shapeworks::Command](../Classes/classshapeworks_1_1Command.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| virtual const std::string | **[type](../Classes/classshapeworks_1_1AnalyzeCommandGroup.md#function-type)**() override |

## Additional inherited members

**Public Functions inherited from [shapeworks::Command](../Classes/classshapeworks_1_1Command.md)**

|                | Name           |
| -------------- | -------------- |
| const std::string | **[name](../Classes/classshapeworks_1_1Command.md#function-name)**() const |
| const std::string | **[usage](../Classes/classshapeworks_1_1Command.md#function-usage)**() const |
| const std::string | **[desc](../Classes/classshapeworks_1_1Command.md#function-desc)**() const |
| std::vector< std::string > | **[parse_args](../Classes/classshapeworks_1_1Command.md#function-parse-args)**(const std::vector< std::string > & arguments)<br>parses the arguments for this command, saving them in the parser and returning the leftovers  |
| int | **[run](../Classes/classshapeworks_1_1Command.md#function-run)**([SharedCommandData](../Classes/structshapeworks_1_1SharedCommandData.md) & sharedData)<br>calls execute for this command using the parsed args, returning system exit value  |

**Protected Functions inherited from [shapeworks::Command](../Classes/classshapeworks_1_1Command.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[buildParser](../Classes/classshapeworks_1_1Command.md#function-buildparser)**() |

**Protected Attributes inherited from [shapeworks::Command](../Classes/classshapeworks_1_1Command.md)**

|                | Name           |
| -------------- | -------------- |
| optparse::OptionParser | **[parser](../Classes/classshapeworks_1_1Command.md#variable-parser)**  |


## Public Functions Documentation

### function type

```cpp
inline virtual const std::string type() override
```


**Reimplements**: [shapeworks::Command::type](../Classes/classshapeworks_1_1Command.md#function-type)


-------------------------------

Updated on 2024-05-18 at 07:02:35 +0000