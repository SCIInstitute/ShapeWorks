---
title: FEItem

---

# FEItem





Inherited by [FEEdge](../Classes/classFEEdge.md), [FEElement_](../Classes/classFEElement__.md), [FEFace](../Classes/classFEFace.md), [FENode](../Classes/classFENode.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FEItem](../Classes/classFEItem.md#function-feitem)**() |
| bool | **[IsVisible](../Classes/classFEItem.md#function-isvisible)**() const |
| bool | **[IsSelected](../Classes/classFEItem.md#function-isselected)**() const |
| void | **[Select](../Classes/classFEItem.md#function-select)**() |
| void | **[UnSelect](../Classes/classFEItem.md#function-unselect)**() |
| void | **[Show](../Classes/classFEItem.md#function-show)**() |
| void | **[Hide](../Classes/classFEItem.md#function-hide)**() |
| unsigned int | **[GetFEState](../Classes/classFEItem.md#function-getfestate)**() const |
| void | **[SetFEState](../Classes/classFEItem.md#function-setfestate)**(unsigned int state) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[m_ntag](../Classes/classFEItem.md#variable-m-ntag)**  |
| int | **[m_gid](../Classes/classFEItem.md#variable-m-gid)**  |
| int | **[m_nid](../Classes/classFEItem.md#variable-m-nid)**  |

## Public Functions Documentation

### function FEItem

```cpp
inline FEItem()
```


### function IsVisible

```cpp
inline bool IsVisible() const
```


### function IsSelected

```cpp
inline bool IsSelected() const
```


### function Select

```cpp
inline void Select()
```


### function UnSelect

```cpp
inline void UnSelect()
```


### function Show

```cpp
inline void Show()
```


### function Hide

```cpp
inline void Hide()
```


### function GetFEState

```cpp
inline unsigned int GetFEState() const
```


### function SetFEState

```cpp
inline void SetFEState(
    unsigned int state
)
```


## Public Attributes Documentation

### variable m_ntag

```cpp
int m_ntag;
```


### variable m_gid

```cpp
int m_gid;
```


### variable m_nid

```cpp
int m_nid;
```


-------------------------------

Updated on 2022-02-16 at 06:47:28 +0000