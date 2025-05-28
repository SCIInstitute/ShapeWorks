---
title: monailabel::MonaiLabelTool

---

# monailabel::MonaiLabelTool





Inherits from QWidget

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[handle_error](../Classes/classmonailabel_1_1MonaiLabelTool.md#slot-handle-error)**(QString msg) |
| void | **[onConnectServer](../Classes/classmonailabel_1_1MonaiLabelTool.md#slot-onconnectserver)**() |
| void | **[onServerAddressChanged](../Classes/classmonailabel_1_1MonaiLabelTool.md#slot-onserveraddresschanged)**() |
| void | **[onModelTypeChanged](../Classes/classmonailabel_1_1MonaiLabelTool.md#slot-onmodeltypechanged)**(int index) |
| void | **[triggerUpdateView](../Classes/classmonailabel_1_1MonaiLabelTool.md#slot-triggerupdateview)**() |
| void | **[handle_progress](../Classes/classmonailabel_1_1MonaiLabelTool.md#slot-handle-progress)**(int val, QString message) |
| void | **[handleSampleNumberChanged](../Classes/classmonailabel_1_1MonaiLabelTool.md#slot-handlesamplenumberchanged)**() |
| void | **[handleClientInitialized](../Classes/classmonailabel_1_1MonaiLabelTool.md#slot-handleclientinitialized)**(bool success) |
| void | **[handleUploadSampleCompleted](../Classes/classmonailabel_1_1MonaiLabelTool.md#slot-handleuploadsamplecompleted)**() |
| void | **[handleSegmentationCompleted](../Classes/classmonailabel_1_1MonaiLabelTool.md#slot-handlesegmentationcompleted)**() |
| void | **[handleSubmitLabelCompleted](../Classes/classmonailabel_1_1MonaiLabelTool.md#slot-handlesubmitlabelcompleted)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[update_view](../Classes/classmonailabel_1_1MonaiLabelTool.md#signal-update-view)**() |
| void | **[progress](../Classes/classmonailabel_1_1MonaiLabelTool.md#signal-progress)**(int ) |
| void | **[sampleChanged](../Classes/classmonailabel_1_1MonaiLabelTool.md#signal-samplechanged)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MonaiLabelTool](../Classes/classmonailabel_1_1MonaiLabelTool.md#function-monailabeltool)**([Preferences](../Classes/classPreferences.md) & prefs) |
| | **[~MonaiLabelTool](../Classes/classmonailabel_1_1MonaiLabelTool.md#function-~monailabeltool)**() |
| void | **[set_session](../Classes/classmonailabel_1_1MonaiLabelTool.md#function-set-session)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session) |
| void | **[set_app](../Classes/classmonailabel_1_1MonaiLabelTool.md#function-set-app)**([ShapeWorksStudioApp](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md) * app) |
| bool | **[is_active](../Classes/classmonailabel_1_1MonaiLabelTool.md#function-is-active)**() |
| void | **[loadParamsFromUi](../Classes/classmonailabel_1_1MonaiLabelTool.md#function-loadparamsfromui)**() |
| void | **[shutdown](../Classes/classmonailabel_1_1MonaiLabelTool.md#function-shutdown)**() |
| void | **[runSegmentationTool](../Classes/classmonailabel_1_1MonaiLabelTool.md#function-runsegmentationtool)**() |
| void | **[resizeEvent](../Classes/classmonailabel_1_1MonaiLabelTool.md#function-resizeevent)**(QResizeEvent * event) override |
| int | **[getCurrentSampleNumber](../Classes/classmonailabel_1_1MonaiLabelTool.md#function-getcurrentsamplenumber)**() |
| void | **[enable_actions](../Classes/classmonailabel_1_1MonaiLabelTool.md#function-enable-actions)**() |
| void | **[activate](../Classes/classmonailabel_1_1MonaiLabelTool.md#function-activate)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| const std::string | **[MONAI_MODE_SEGMENTATION](../Classes/classmonailabel_1_1MonaiLabelTool.md#variable-monai-mode-segmentation)**  |
| const std::string | **[MONAI_MODE_DEEPGROW](../Classes/classmonailabel_1_1MonaiLabelTool.md#variable-monai-mode-deepgrow)**  |
| const std::string | **[MONAI_MODE_DEEPEDIT](../Classes/classmonailabel_1_1MonaiLabelTool.md#variable-monai-mode-deepedit)**  |
| const std::string | **[MONAI_SAMPLE_STRATEGY_RANDOM](../Classes/classmonailabel_1_1MonaiLabelTool.md#variable-monai-sample-strategy-random)**  |

## Public Slots Documentation

### slot handle_error

```cpp
void handle_error(
    QString msg
)
```


### slot onConnectServer

```cpp
void onConnectServer()
```


### slot onServerAddressChanged

```cpp
void onServerAddressChanged()
```


### slot onModelTypeChanged

```cpp
void onModelTypeChanged(
    int index
)
```


### slot triggerUpdateView

```cpp
void triggerUpdateView()
```


### slot handle_progress

```cpp
void handle_progress(
    int val,
    QString message
)
```


### slot handleSampleNumberChanged

```cpp
void handleSampleNumberChanged()
```


### slot handleClientInitialized

```cpp
void handleClientInitialized(
    bool success
)
```


### slot handleUploadSampleCompleted

```cpp
void handleUploadSampleCompleted()
```


### slot handleSegmentationCompleted

```cpp
void handleSegmentationCompleted()
```


### slot handleSubmitLabelCompleted

```cpp
void handleSubmitLabelCompleted()
```


## Public Signals Documentation

### signal update_view

```cpp
void update_view()
```


### signal progress

```cpp
void progress(
    int 
)
```


### signal sampleChanged

```cpp
void sampleChanged()
```


## Public Functions Documentation

### function MonaiLabelTool

```cpp
MonaiLabelTool(
    Preferences & prefs
)
```


### function ~MonaiLabelTool

```cpp
~MonaiLabelTool()
```


### function set_session

```cpp
void set_session(
    QSharedPointer< Session > session
)
```


### function set_app

```cpp
void set_app(
    ShapeWorksStudioApp * app
)
```


### function is_active

```cpp
bool is_active()
```


### function loadParamsFromUi

```cpp
void loadParamsFromUi()
```


### function shutdown

```cpp
void shutdown()
```


### function runSegmentationTool

```cpp
void runSegmentationTool()
```


### function resizeEvent

```cpp
void resizeEvent(
    QResizeEvent * event
) override
```


### function getCurrentSampleNumber

```cpp
int getCurrentSampleNumber()
```


### function enable_actions

```cpp
void enable_actions()
```


### function activate

```cpp
void activate()
```


## Public Attributes Documentation

### variable MONAI_MODE_SEGMENTATION

```cpp
static const std::string MONAI_MODE_SEGMENTATION;
```


### variable MONAI_MODE_DEEPGROW

```cpp
static const std::string MONAI_MODE_DEEPGROW;
```


### variable MONAI_MODE_DEEPEDIT

```cpp
static const std::string MONAI_MODE_DEEPEDIT;
```


### variable MONAI_SAMPLE_STRATEGY_RANDOM

```cpp
static const std::string MONAI_SAMPLE_STRATEGY_RANDOM;
```


-------------------------------

Updated on 2025-05-28 at 23:15:53 +0000