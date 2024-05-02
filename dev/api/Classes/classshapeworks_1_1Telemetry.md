---
title: shapeworks::Telemetry

---

# shapeworks::Telemetry





Inherits from QObject

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[handle_network_reply](../Classes/classshapeworks_1_1Telemetry.md#slot-handle-network-reply)**(QNetworkReply * reply) |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Telemetry](../Classes/classshapeworks_1_1Telemetry.md#function-telemetry)**([Preferences](../Classes/classPreferences.md) & prefs) |
| void | **[record_event](../Classes/classshapeworks_1_1Telemetry.md#function-record-event)**(const QString & name, const QVariantMap & params) |

## Public Slots Documentation

### slot handle_network_reply

```cpp
void handle_network_reply(
    QNetworkReply * reply
)
```


## Public Functions Documentation

### function Telemetry

```cpp
Telemetry(
    Preferences & prefs
)
```


### function record_event

```cpp
void record_event(
    const QString & name,
    const QVariantMap & params
)
```


-------------------------------

Updated on 2024-05-02 at 21:06:12 +0000