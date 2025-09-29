---
title: monailabel::MonaiLabelJob

---

# monailabel::MonaiLabelJob





Inherits from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md), QObject

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[onUploadSampleClicked](../Classes/classmonailabel_1_1MonaiLabelJob.md#slot-onuploadsampleclicked)**() |
| void | **[onRunSegmentationClicked](../Classes/classmonailabel_1_1MonaiLabelJob.md#slot-onrunsegmentationclicked)**() |
| void | **[onSubmitLabelClicked](../Classes/classmonailabel_1_1MonaiLabelJob.md#slot-onsubmitlabelclicked)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[triggerUpdateView](../Classes/classmonailabel_1_1MonaiLabelJob.md#signal-triggerupdateview)**() |
| void | **[triggerClientInitialized](../Classes/classmonailabel_1_1MonaiLabelJob.md#signal-triggerclientinitialized)**(bool success) |
| void | **[triggerUploadSampleCompleted](../Classes/classmonailabel_1_1MonaiLabelJob.md#signal-triggeruploadsamplecompleted)**() |
| void | **[triggerSegmentationCompleted](../Classes/classmonailabel_1_1MonaiLabelJob.md#signal-triggersegmentationcompleted)**() |
| void | **[triggerSubmitLabelCompleted](../Classes/classmonailabel_1_1MonaiLabelJob.md#signal-triggersubmitlabelcompleted)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MonaiLabelJob](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-monailabeljob)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session, const std::string & server_url, const std::string & client_id, const std::string & strategy, const std::string & model_type) |
| | **[~MonaiLabelJob](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-~monailabeljob)**() |
| void | **[setServer](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-setserver)**(const std::string & server_url) |
| void | **[setModelType](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-setmodeltype)**(const std::string & model_type) |
| const std::string & | **[getServer](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-getserver)**() |
| void | **[setClientId](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-setclientid)**(const std::string & client_id ="") |
| const std::string & | **[getClientId](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-getclientid)**() |
| void | **[initializeClient](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-initializeclient)**() |
| std::shared_ptr< py::object > | **[getClient](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-getclient)**() const |
| py::dict | **[getInfo](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-getinfo)**() |
| std::string | **[getModelName](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-getmodelname)**(std::string modelType) |
| std::vector< std::string > | **[getModelNames](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-getmodelnames)**(const std::string & model_type) |
| std::string | **[getSessionId](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-getsessionid)**() |
| py::dict | **[getParamsFromConfig](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-getparamsfromconfig)**(std::string section, std::string name) |
| py::dict | **[nextSample](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-nextsample)**(std::string strategy, py::dict params) |
| py::tuple | **[infer](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-infer)**(std::string model, std::string image_in, py::dict params, std::string label_in, std::string file, std::string session_id) |
| py::dict | **[saveLabel](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-savelabel)**(std::string image_in, std::string label_in, py::dict params) |
| py::dict | **[uploadImage](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-uploadimage)**(std::string image_in, std::string image_id) |
| void | **[updateShapes](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-updateshapes)**() |
| void | **[runSegmentationModel](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-runsegmentationmodel)**() |
| virtual void | **[run](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-run)**() override<br>run the job  |
| virtual QString | **[name](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-name)**() override<br>get the name of the job  |
| void | **[python_message](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-python-message)**(std::string str) |
| void | **[setCurrentSampleNumber](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-setcurrentsamplenumber)**(int n) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| const std::string | **[MONAI_RESULT_EXTENSION](../Classes/classmonailabel_1_1MonaiLabelJob.md#variable-monai-result-extension)**  |
| const std::string | **[MONAI_RESULT_DTYPE](../Classes/classmonailabel_1_1MonaiLabelJob.md#variable-monai-result-dtype)**  |

## Additional inherited members

**Public Signals inherited from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[progress](../Classes/classshapeworks_1_1Job.md#signal-progress)**(double , QString  ="") |
| void | **[finished](../Classes/classshapeworks_1_1Job.md#signal-finished)**() |

**Public Functions inherited from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md)**

|                | Name           |
| -------------- | -------------- |
| | **[Job](../Classes/classshapeworks_1_1Job.md#function-job)**() |
| virtual | **[~Job](../Classes/classshapeworks_1_1Job.md#function-~job)**() |
| virtual QString | **[get_completion_message](../Classes/classshapeworks_1_1Job.md#function-get-completion-message)**()<br>get a message to display when the job is complete  |
| virtual QString | **[get_abort_message](../Classes/classshapeworks_1_1Job.md#function-get-abort-message)**()<br>get a message to display when the job is aborted  |
| void | **[start_timer](../Classes/classshapeworks_1_1Job.md#function-start-timer)**()<br>start the timer  |
| qint64 | **[timer_elapsed](../Classes/classshapeworks_1_1Job.md#function-timer-elapsed)**()<br>how much time has elapsed since the timer was started  |
| void | **[set_complete](../Classes/classshapeworks_1_1Job.md#function-set-complete)**(bool complete)<br>set the job as complete  |
| bool | **[is_complete](../Classes/classshapeworks_1_1Job.md#function-is-complete)**() const<br>is the job complete?  |
| void | **[abort](../Classes/classshapeworks_1_1Job.md#function-abort)**()<br>abort the job  |
| bool | **[is_aborted](../Classes/classshapeworks_1_1Job.md#function-is-aborted)**() const<br>was the job aborted?  |
| void | **[set_quiet_mode](../Classes/classshapeworks_1_1Job.md#function-set-quiet-mode)**(bool quiet)<br>set to quiet mode (no progress messages)  |
| bool | **[get_quiet_mode](../Classes/classshapeworks_1_1Job.md#function-get-quiet-mode)**()<br>get quiet mode  |


## Public Slots Documentation

### slot onUploadSampleClicked

```cpp
void onUploadSampleClicked()
```


### slot onRunSegmentationClicked

```cpp
void onRunSegmentationClicked()
```


### slot onSubmitLabelClicked

```cpp
void onSubmitLabelClicked()
```


## Public Signals Documentation

### signal triggerUpdateView

```cpp
void triggerUpdateView()
```


### signal triggerClientInitialized

```cpp
void triggerClientInitialized(
    bool success
)
```


### signal triggerUploadSampleCompleted

```cpp
void triggerUploadSampleCompleted()
```


### signal triggerSegmentationCompleted

```cpp
void triggerSegmentationCompleted()
```


### signal triggerSubmitLabelCompleted

```cpp
void triggerSubmitLabelCompleted()
```


## Public Functions Documentation

### function MonaiLabelJob

```cpp
MonaiLabelJob(
    QSharedPointer< Session > session,
    const std::string & server_url,
    const std::string & client_id,
    const std::string & strategy,
    const std::string & model_type
)
```


### function ~MonaiLabelJob

```cpp
~MonaiLabelJob()
```


### function setServer

```cpp
void setServer(
    const std::string & server_url
)
```


### function setModelType

```cpp
void setModelType(
    const std::string & model_type
)
```


### function getServer

```cpp
inline const std::string & getServer()
```


### function setClientId

```cpp
void setClientId(
    const std::string & client_id =""
)
```


### function getClientId

```cpp
inline const std::string & getClientId()
```


### function initializeClient

```cpp
void initializeClient()
```


### function getClient

```cpp
inline std::shared_ptr< py::object > getClient() const
```


### function getInfo

```cpp
py::dict getInfo()
```


### function getModelName

```cpp
std::string getModelName(
    std::string modelType
)
```


### function getModelNames

```cpp
std::vector< std::string > getModelNames(
    const std::string & model_type
)
```


### function getSessionId

```cpp
std::string getSessionId()
```


### function getParamsFromConfig

```cpp
py::dict getParamsFromConfig(
    std::string section,
    std::string name
)
```


### function nextSample

```cpp
py::dict nextSample(
    std::string strategy,
    py::dict params
)
```


### function infer

```cpp
py::tuple infer(
    std::string model,
    std::string image_in,
    py::dict params,
    std::string label_in,
    std::string file,
    std::string session_id
)
```


### function saveLabel

```cpp
py::dict saveLabel(
    std::string image_in,
    std::string label_in,
    py::dict params
)
```


### function uploadImage

```cpp
py::dict uploadImage(
    std::string image_in,
    std::string image_id
)
```


### function updateShapes

```cpp
void updateShapes()
```


### function runSegmentationModel

```cpp
void runSegmentationModel()
```


### function run

```cpp
virtual void run() override
```

run the job 

**Reimplements**: [shapeworks::Job::run](../Classes/classshapeworks_1_1Job.md#function-run)


### function name

```cpp
virtual QString name() override
```

get the name of the job 

**Reimplements**: [shapeworks::Job::name](../Classes/classshapeworks_1_1Job.md#function-name)


### function python_message

```cpp
void python_message(
    std::string str
)
```


### function setCurrentSampleNumber

```cpp
void setCurrentSampleNumber(
    int n
)
```


## Public Attributes Documentation

### variable MONAI_RESULT_EXTENSION

```cpp
static const std::string MONAI_RESULT_EXTENSION;
```


### variable MONAI_RESULT_DTYPE

```cpp
static const std::string MONAI_RESULT_DTYPE;
```


-------------------------------

Updated on 2025-09-29 at 22:02:14 +0000