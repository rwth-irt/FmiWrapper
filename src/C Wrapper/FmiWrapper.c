#include "FmiWrapper.h"
#include "SystemFunctions.h"
#include "fmi2FunctionTypes.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

struct wrappedModel
{
    /*! The handle to the shared library. */
    void *sharedLibraryHandle;
    /*! The component returned when instantiating the fmu. */
    fmi2Component component;
    /*! Store the callback functions because some fmus just store the pointer to the struct */
    fmi2CallbackFunctions *callbackFunctions;
    /*! Callback to forward logs from the fmu to the calling enviroment. */
    logCallback enviromentLog;
    /*! Callback to forward the step finished even from the fmu to the calling enviroment. */
    stepFinishedCallback enviromentStepFinished;

    /* **************************************************
    Common Functions
    ****************************************************/

    /* Inquire version numbers of header files */
    fmi2GetTypesPlatformTYPE *fmi2GetTypesPlatform;
    fmi2GetVersionTYPE *fmi2GetVersion;
    fmi2SetDebugLoggingTYPE *fmi2SetDebugLogging;

    /* Creation and destruction of FMU instances */
    fmi2InstantiateTYPE *fmi2Instantiate;
    fmi2FreeInstanceTYPE *fmi2FreeInstance;

    /* Enter and exit initialization mode, terminate and reset */
    fmi2SetupExperimentTYPE *fmi2SetupExperiment;
    fmi2EnterInitializationModeTYPE *fmi2EnterInitializationMode;
    fmi2ExitInitializationModeTYPE *fmi2ExitInitializationMode;
    fmi2TerminateTYPE *fmi2Terminate;
    fmi2ResetTYPE *fmi2Reset;

    /* Getting and setting variables values */
    fmi2GetRealTYPE *fmi2GetReal;
    fmi2GetIntegerTYPE *fmi2GetInteger;
    fmi2GetBooleanTYPE *fmi2GetBoolean;
    fmi2GetStringTYPE *fmi2GetString;

    fmi2SetRealTYPE *fmi2SetReal;
    fmi2SetIntegerTYPE *fmi2SetInteger;
    fmi2SetBooleanTYPE *fmi2SetBoolean;
    fmi2SetStringTYPE *fmi2SetString;

    /* Getting and setting the internal FMU state */
    fmi2GetFMUstateTYPE *fmi2GetFMUstate;
    fmi2SetFMUstateTYPE *fmi2SetFMUstate;
    fmi2FreeFMUstateTYPE *fmi2FreeFMUstate;
    fmi2SerializedFMUstateSizeTYPE *fmi2SerializedFMUstatelength;
    fmi2SerializeFMUstateTYPE *fmi2SerializeFMUstate;
    fmi2DeSerializeFMUstateTYPE *fmi2DeSerializeFMUstate;

    /* Getting partial derivatives */
    fmi2GetDirectionalDerivativeTYPE *fmi2GetDirectionalDerivative;

    /* **************************************************
    Functions for FMI2 for Model Exchange
    ****************************************************/

    /* Enter and exit the different modes */
    fmi2EnterEventModeTYPE *fmi2EnterEventMode;
    fmi2NewDiscreteStatesTYPE *fmi2NewDiscreteStates;
    fmi2EnterContinuousTimeModeTYPE *fmi2EnterContinuousTimeMode;
    fmi2CompletedIntegratorStepTYPE *fmi2CompletedIntegratorStep;

    /* Providing independent variables and re-initialization of caching */
    fmi2SetTimeTYPE *fmi2SetTime;
    fmi2SetContinuousStatesTYPE *fmi2SetContinuousStates;

    /* Evaluation of the model equations */
    fmi2GetDerivativesTYPE *fmi2GetDerivatives;
    fmi2GetEventIndicatorsTYPE *fmi2GetEventIndicators;
    fmi2GetContinuousStatesTYPE *fmi2GetContinuousStates;
    fmi2GetNominalsOfContinuousStatesTYPE *fmi2GetNominalsOfContinuousStates;

    /* **************************************************
    Functions for FMI2 for Co-Simulation
    ****************************************************/

    /* Simulating the slave */
    fmi2SetRealInputDerivativesTYPE *fmi2SetRealInputDerivatives;
    fmi2GetRealOutputDerivativesTYPE *fmi2GetRealOutputDerivatives;

    fmi2DoStepTYPE *fmi2DoStep;
    fmi2CancelStepTYPE *fmi2CancelStep;

    /* Inquire slave status */
    fmi2GetStatusTYPE *fmi2GetStatus;
    fmi2GetRealStatusTYPE *fmi2GetRealStatus;
    fmi2GetIntegerStatusTYPE *fmi2GetIntegerStatus;
    fmi2GetBooleanStatusTYPE *fmi2GetBooleanStatus;
    fmi2GetStringStatusTYPE *fmi2GetStringStatus;
};

/*!
Implementation of the stepFinished callback that is passed to the fmu.
This function calls the stepFinishedCallback of the wrapper.
*/
static void fmuLogCallback(fmi2ComponentEnvironment componentEnvironment, fmi2String instanceName, fmi2Status status, fmi2String category, fmi2String message, ...)
{
    wrappedModel *wrapper = (wrappedModel *)componentEnvironment;
    // fmi2standard: The message is to be used like sprintf.
    // For simplification apply the variadic arguments to the format string and call enviromentLog with this single string
    va_list args;
    va_start(args, message);
    // Get the length of the string + 1 for the \0 char.
    int needed_size = vsnprintf(NULL, 0, message, args) + 1;
    // Create and read into buffer with length + 1 (for \0 character)
    char *buffer = malloc(needed_size);
    vsnprintf(buffer, needed_size, message, args);
    wrapper->enviromentLog(instanceName, status, category, buffer);
    free(buffer);
    va_end(args);
}

/*!
Implementation of the logger callback that is passed to the fmu.
This function calls the logCallback of the wrapper.
*/
static void fmuStepFinished(fmi2ComponentEnvironment componentEnvironment, fmi2Status status)
{
    wrappedModel *wrapper = (wrappedModel *)componentEnvironment;
    wrapper->enviromentStepFinished(status);
}

/*! Load the functions from the binary into a wrapper struct. */
wrappedModel *createWrapper(const char *fileName, logCallback logCallback, stepFinishedCallback stepFinishedCallback)
{
    // Create the wrapper struct
    wrappedModel *wrapper = malloc(sizeof *wrapper);
    wrapper->sharedLibraryHandle = loadSharedLibrary(fileName);
    if (wrapper->sharedLibraryHandle == NULL)
    {
        // Failed to load the library.
        return NULL;
    }
    wrapper->enviromentLog = logCallback;
    wrapper->enviromentStepFinished = stepFinishedCallback;
    // Load all the funcions
    /* Inquire version numbers of header files */
    wrapper->fmi2GetTypesPlatform = getFunction(wrapper->sharedLibraryHandle, "fmi2GetTypesPlatform");
    wrapper->fmi2GetVersion = getFunction(wrapper->sharedLibraryHandle, "fmi2GetVersion");
    wrapper->fmi2SetDebugLogging = getFunction(wrapper->sharedLibraryHandle, "fmi2SetDebugLogging");
    /* Creation and destruction of FMU instances */
    wrapper->fmi2Instantiate = getFunction(wrapper->sharedLibraryHandle, "fmi2Instantiate");
    wrapper->fmi2FreeInstance = getFunction(wrapper->sharedLibraryHandle, "fmi2FreeInstance");
    /* Enter and exit initialization mode, terminate and reset */
    wrapper->fmi2SetupExperiment = getFunction(wrapper->sharedLibraryHandle, "fmi2SetupExperiment");
    wrapper->fmi2EnterInitializationMode = getFunction(wrapper->sharedLibraryHandle, "fmi2EnterInitializationMode");
    wrapper->fmi2ExitInitializationMode = getFunction(wrapper->sharedLibraryHandle, "fmi2ExitInitializationMode");
    wrapper->fmi2Terminate = getFunction(wrapper->sharedLibraryHandle, "fmi2Terminate");
    wrapper->fmi2Reset = getFunction(wrapper->sharedLibraryHandle, "fmi2Reset");
    /* Getting and setting variables values */
    wrapper->fmi2GetReal = getFunction(wrapper->sharedLibraryHandle, "fmi2GetReal");
    wrapper->fmi2GetInteger = getFunction(wrapper->sharedLibraryHandle, "fmi2GetInteger");
    wrapper->fmi2GetBoolean = getFunction(wrapper->sharedLibraryHandle, "fmi2GetBoolean");
    wrapper->fmi2GetString = getFunction(wrapper->sharedLibraryHandle, "fmi2GetString");

    wrapper->fmi2SetReal = getFunction(wrapper->sharedLibraryHandle, "fmi2SetReal");
    wrapper->fmi2SetInteger = getFunction(wrapper->sharedLibraryHandle, "fmi2SetInteger");
    wrapper->fmi2SetBoolean = getFunction(wrapper->sharedLibraryHandle, "fmi2SetBoolean");
    wrapper->fmi2SetString = getFunction(wrapper->sharedLibraryHandle, "fmi2SetString");
    /* Getting and setting the internal FMU state */
    wrapper->fmi2GetFMUstate = getFunction(wrapper->sharedLibraryHandle, "fmi2GetFMUstate");
    wrapper->fmi2SetFMUstate = getFunction(wrapper->sharedLibraryHandle, "fmi2SetFMUstate");
    wrapper->fmi2FreeFMUstate = getFunction(wrapper->sharedLibraryHandle, "fmi2FreeFMUstate");
    wrapper->fmi2SerializedFMUstatelength = getFunction(wrapper->sharedLibraryHandle, "fmi2SerializedFMUstatelength");
    wrapper->fmi2SerializeFMUstate = getFunction(wrapper->sharedLibraryHandle, "fmi2SerializeFMUstate");
    wrapper->fmi2DeSerializeFMUstate = getFunction(wrapper->sharedLibraryHandle, "fmi2DeSerializeFMUstate");
    /* Getting partial derivatives */
    wrapper->fmi2GetDirectionalDerivative = getFunction(wrapper->sharedLibraryHandle, "fmi2GetDirectionalDerivative");
    /* Enter and exit the different modes */
    wrapper->fmi2EnterEventMode = getFunction(wrapper->sharedLibraryHandle, "fmi2EnterEventMode");
    wrapper->fmi2NewDiscreteStates = getFunction(wrapper->sharedLibraryHandle, "fmi2NewDiscreteStates");
    wrapper->fmi2EnterContinuousTimeMode = getFunction(wrapper->sharedLibraryHandle, "fmi2EnterContinuousTimeMode");
    wrapper->fmi2CompletedIntegratorStep = getFunction(wrapper->sharedLibraryHandle, "fmi2CompletedIntegratorStep");
    /* Providing independent variables and re-initialization of caching */
    wrapper->fmi2SetTime = getFunction(wrapper->sharedLibraryHandle, "fmi2SetTime");
    wrapper->fmi2SetContinuousStates = getFunction(wrapper->sharedLibraryHandle, "fmi2SetContinuousStates");
    /* Evaluation of the model equations */
    wrapper->fmi2GetDerivatives = getFunction(wrapper->sharedLibraryHandle, "fmi2GetDerivatives");
    wrapper->fmi2GetEventIndicators = getFunction(wrapper->sharedLibraryHandle, "fmi2GetEventIndicators");
    wrapper->fmi2GetContinuousStates = getFunction(wrapper->sharedLibraryHandle, "fmi2GetContinuousStates");
    wrapper->fmi2GetNominalsOfContinuousStates = getFunction(wrapper->sharedLibraryHandle, "fmi2GetNominalsOfContinuousStates");
    /* Simulating the slave */
    wrapper->fmi2SetRealInputDerivatives = getFunction(wrapper->sharedLibraryHandle, "fmi2SetRealInputDerivatives");
    wrapper->fmi2GetRealOutputDerivatives = getFunction(wrapper->sharedLibraryHandle, "fmi2GetRealOutputDerivatives");

    wrapper->fmi2DoStep = getFunction(wrapper->sharedLibraryHandle, "fmi2DoStep");
    wrapper->fmi2CancelStep = getFunction(wrapper->sharedLibraryHandle, "fmi2CancelStep");
    /* Inquire slave status */
    wrapper->fmi2GetStatus = getFunction(wrapper->sharedLibraryHandle, "fmi2GetStatus");
    wrapper->fmi2GetRealStatus = getFunction(wrapper->sharedLibraryHandle, "fmi2GetRealStatus");
    wrapper->fmi2GetIntegerStatus = getFunction(wrapper->sharedLibraryHandle, "fmi2GetIntegerStatus");
    wrapper->fmi2GetBooleanStatus = getFunction(wrapper->sharedLibraryHandle, "fmi2GetBooleanStatus");
    wrapper->fmi2GetStringStatus = getFunction(wrapper->sharedLibraryHandle, "fmi2GetStringStatus");
    return wrapper;
}

/*! Free the handle and memory of the wrapper. */
void freeWrapper(wrappedModel *wrapper)
{
    freeSharedLibrary(wrapper->sharedLibraryHandle);
    free(wrapper->callbackFunctions);
    free(wrapper);
}

/* Creation and destruction of FMU instances and setting debug status */

PUBLIC_EXPORT wrappedModel *fmi2Instantiate(const char *fileName, logCallback logCallback, stepFinishedCallback stepFinishedCallback,
                                            const char *instanceName, int fmuType, const char *guid, const char *resourceLocation, bool visible, bool loggingOn)
{
    // Load the functions from the binary
    wrappedModel *wrapper = createWrapper(fileName, logCallback, stepFinishedCallback);
    if (wrapper == NULL)
    {
        return NULL;
    }
    // Supply static callback functions and the wrapper. The wrapper contains the callbacks of the enviroment.
    wrapper->callbackFunctions = malloc(sizeof(*wrapper->callbackFunctions));
    fmi2CallbackFunctions callbacks = {
        fmuLogCallback,
        calloc,
        free,
        fmuStepFinished,
        wrapper };
    memcpy(wrapper->callbackFunctions, &callbacks, sizeof(*wrapper->callbackFunctions));
    // Instantiate the fmu
    wrapper->component = wrapper->fmi2Instantiate(instanceName, fmuType, guid, resourceLocation, wrapper->callbackFunctions, visible, loggingOn);
    if (wrapper->component == NULL)
    {
        // Failed to instantiate return null
        freeWrapper(wrapper);
        return NULL;
    }
    return wrapper;
}

PUBLIC_EXPORT void fmi2FreeInstance(wrappedModel *wrapper)
{
    wrapper->fmi2FreeInstance(wrapper->component);
    freeWrapper(wrapper);
}

/* Inquire version numbers of header files and setting logging status */

PUBLIC_EXPORT const char *fmi2GetTypesPlatform(wrappedModel *wrapper)
{
    return wrapper->fmi2GetTypesPlatform();
}

PUBLIC_EXPORT const char *fmi2GetVersion(wrappedModel *wrapper)
{
    return wrapper->fmi2GetVersion();
}

PUBLIC_EXPORT int fmi2SetDebugLogging(wrappedModel *wrapper, bool loggingOn, size_t nCategories, const char *categories[])
{
    return wrapper->fmi2SetDebugLogging(wrapper->component, loggingOn, nCategories, categories);
}


/* Enter and exit initialization mode, terminate and reset */

PUBLIC_EXPORT int fmi2SetupExperiment(wrappedModel *wrapper, bool toleranceDefined, double tolerance, double startTime, bool stopTimeDefined, double stopTime)
{
    return wrapper->fmi2SetupExperiment(wrapper->component, toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime);
}

PUBLIC_EXPORT int fmi2EnterInitializationMode(wrappedModel *wrapper)
{
    return wrapper->fmi2EnterInitializationMode(wrapper->component);
}

PUBLIC_EXPORT int fmi2ExitInitializationMode(wrappedModel *wrapper)
{
    return wrapper->fmi2ExitInitializationMode(wrapper->component);
}

PUBLIC_EXPORT int fmi2Terminate(wrappedModel *wrapper)
{
    return wrapper->fmi2Terminate(wrapper->component);
}

PUBLIC_EXPORT int fmi2Reset(wrappedModel *wrapper)
{
    return wrapper->fmi2Reset(wrapper->component);
}

/* Getting and setting variable values */
PUBLIC_EXPORT int fmi2GetReal(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, double value[])
{
    return wrapper->fmi2GetReal(wrapper->component, vr, nvr, value);
}

PUBLIC_EXPORT int fmi2GetInteger(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, int value[])
{
    return wrapper->fmi2GetInteger(wrapper->component, vr, nvr, value);
}

PUBLIC_EXPORT int fmi2GetBoolean(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, bool value[])
{
    // fmi2Boolean is int and is larger than bool
    fmi2Boolean *val = malloc(sizeof(fmi2Boolean) * nvr);
    int result = wrapper->fmi2GetBoolean(wrapper->component, vr, nvr, val);
    // Convert to bool
    for (size_t i = 0; i < nvr; i++)
    {
        value[i] = (bool)val[i];
    }
    free(val);
    return result;
}

PUBLIC_EXPORT int fmi2GetString(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, const char *value[])
{
    return wrapper->fmi2GetString(wrapper->component, vr, nvr, value);
}

PUBLIC_EXPORT int fmi2SetReal(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, const double value[])
{
    return wrapper->fmi2SetReal(wrapper->component, vr, nvr, value);
}

PUBLIC_EXPORT int fmi2SetInteger(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, const int value[])
{
    return wrapper->fmi2SetInteger(wrapper->component, vr, nvr, value);
}

PUBLIC_EXPORT int fmi2SetBoolean(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, const bool value[])
{
    // fmi2Boolean is int and is larger than bool
    fmi2Boolean *val = malloc(sizeof(fmi2Boolean) * nvr);
    for (size_t i = 0; i < nvr; i++)
    {
        val[i] = value[i];
    }
    int result = wrapper->fmi2SetBoolean(wrapper->component, vr, nvr, val);
    free(val);
    return result;
}

PUBLIC_EXPORT int fmi2SetString(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, const char *value[])
{
    return wrapper->fmi2SetString(wrapper->component, vr, nvr, value);
}

/* Getting and setting the internal FMU state */
PUBLIC_EXPORT int fmi2GetFMUstate(wrappedModel *wrapper, void **FMUstate)
{
    return wrapper->fmi2GetFMUstate(wrapper->component, FMUstate);
}
PUBLIC_EXPORT int fmi2SetFMUstate(wrappedModel *wrapper, void *FMUstate)
{
    return wrapper->fmi2SetFMUstate(wrapper->component, FMUstate);
}
PUBLIC_EXPORT int fmi2FreeFMUstate(wrappedModel *wrapper, void **FMUstate)
{
    return wrapper->fmi2FreeFMUstate(wrapper->component, FMUstate);
}
PUBLIC_EXPORT int fmi2SerializedFMUstatelength(wrappedModel *wrapper, void *FMUstate, size_t *length)
{
    return wrapper->fmi2SerializedFMUstatelength(wrapper->component, FMUstate, length);
}
PUBLIC_EXPORT int fmi2SerializeFMUstate(wrappedModel *wrapper, void *FMUstate, char serializedState[], size_t length)
{
    return wrapper->fmi2SerializeFMUstate(wrapper->component, FMUstate, serializedState, length);
}

PUBLIC_EXPORT int fmi2DeSerializeFMUstate(wrappedModel *wrapper, const char serializedState[], size_t length, void **FMUstate)
{
    return wrapper->fmi2DeSerializeFMUstate(wrapper->component, serializedState, length, FMUstate);
}

/* Getting partial derivatives */
PUBLIC_EXPORT int fmi2GetDirectionalDerivative(wrappedModel *wrapper, const unsigned int vUnknown_ref[], size_t nUnknown,
                                               const unsigned int vKnown_ref[], size_t nKnown,
                                               const double dvKnown[], double dvUnknown[])
{
    return wrapper->fmi2GetDirectionalDerivative(wrapper->component, vUnknown_ref, nUnknown, vKnown_ref, nKnown, dvKnown, dvUnknown);
}

/* **************************************************
Typess for Functions for FMI2 for Model Exchange
****************************************************/

/* Enter and exit the different modes */
PUBLIC_EXPORT int fmi2EnterEventMode(wrappedModel *wrapper)
{
    return wrapper->fmi2EnterEventMode(wrapper->component);
}

PUBLIC_EXPORT int fmi2NewDiscreteStates(wrappedModel *wrapper, bool *newDiscreteStatesNeeded, bool *terminateSimulation, bool *nominalsOfContinuousStatesChanged, bool *valuesOfContinuousStatesChanged, bool *nextEventTimeDefined, double *nextEventTime)
{
    // The struct is a pain to marshal so provide it here and update the reference values.
    fmi2EventInfo info = { 0 };
    int result = wrapper->fmi2NewDiscreteStates(wrapper->component, &info);
    *newDiscreteStatesNeeded = info.newDiscreteStatesNeeded;
    *terminateSimulation = info.terminateSimulation;
    *nominalsOfContinuousStatesChanged = info.nominalsOfContinuousStatesChanged;
    *valuesOfContinuousStatesChanged = info.valuesOfContinuousStatesChanged;
    *nextEventTimeDefined = info.nextEventTimeDefined;
    *nextEventTime = info.nextEventTime;
    return result;
}

PUBLIC_EXPORT int fmi2EnterContinuousTimeMode(wrappedModel *wrapper)
{
    return wrapper->fmi2EnterContinuousTimeMode(wrapper->component);
}

PUBLIC_EXPORT int fmi2CompletedIntegratorStep(wrappedModel *wrapper, bool noSetFMUStatePriorToCurrentPoint, bool *enterEventMode, bool *terminateSimulation)
{
    // fmi2Boolean is larger than bool so cast a bit
    fmi2Boolean eem;
    fmi2Boolean ts;
    int result = wrapper->fmi2CompletedIntegratorStep(wrapper->component, noSetFMUStatePriorToCurrentPoint, &eem, &ts);
    *enterEventMode = (bool)eem;
    *terminateSimulation = (bool)ts;
    return result;
}

/* Providing independent variables and re-initialization of caching */
PUBLIC_EXPORT int fmi2SetTime(wrappedModel *wrapper, double time)
{
    return wrapper->fmi2SetTime(wrapper->component, time);
}

PUBLIC_EXPORT int fmi2SetContinuousStates(wrappedModel *wrapper, const double x[], size_t nx)
{
    return wrapper->fmi2SetContinuousStates(wrapper->component, x, nx);
}

/* Evaluation of the model equations */
PUBLIC_EXPORT int fmi2GetDerivatives(wrappedModel *wrapper, double derivatives[], size_t nx)
{
    return wrapper->fmi2GetDerivatives(wrapper->component, derivatives, nx);
}

PUBLIC_EXPORT int fmi2GetEventIndicators(wrappedModel *wrapper, double eventIndicators[], size_t ni)
{
    return wrapper->fmi2GetEventIndicators(wrapper->component, eventIndicators, ni);
}

PUBLIC_EXPORT int fmi2GetContinuousStates(wrappedModel *wrapper, double x[], size_t nx)
{
    return wrapper->fmi2GetContinuousStates(wrapper->component, x, nx);
}

PUBLIC_EXPORT int fmi2GetNominalsOfContinuousStates(wrappedModel *wrapper, double x_nominal[], size_t nx)
{
    return wrapper->fmi2GetNominalsOfContinuousStates(wrapper->component, x_nominal, nx);
}

/* **************************************************
Types for Functions for FMI2 for Co-Simulation
****************************************************/

/* Simulating the slave */
PUBLIC_EXPORT int fmi2SetRealInputDerivatives(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, const int order[], const double value[])
{
    return wrapper->fmi2SetRealInputDerivatives(wrapper->component, vr, nvr, order, value);
}

PUBLIC_EXPORT int fmi2GetRealOutputDerivatives(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, const int order[], double value[])
{
    return wrapper->fmi2GetRealOutputDerivatives(wrapper->component, vr, nvr, order, value);
}

PUBLIC_EXPORT int fmi2DoStep(wrappedModel *wrapper, double currentCommunicationPoint, double communicationSteplength, bool noSetFMUStatePriorToCurrentPoint)
{
    return wrapper->fmi2DoStep(wrapper->component, currentCommunicationPoint, communicationSteplength, noSetFMUStatePriorToCurrentPoint);
}

PUBLIC_EXPORT int fmi2CancelStep(wrappedModel *wrapper)
{
    return wrapper->fmi2CancelStep(wrapper->component);
}

/* Inquire slave status */
PUBLIC_EXPORT int fmi2GetStatus(wrappedModel *wrapper, const int statusKind, int *value)
{
    // Pass in the original enum and then cast it to int
    fmi2Status val;
    int result = wrapper->fmi2GetStatus(wrapper->component, statusKind, &val);
    *value = val;
    return result;
}

PUBLIC_EXPORT int fmi2GetRealStatus(wrappedModel *wrapper, const int statusKind, double *value)
{
    return wrapper->fmi2GetRealStatus(wrapper->component, statusKind, value);
}

PUBLIC_EXPORT int fmi2GetIntegerStatus(wrappedModel *wrapper, const int statusKind, int *value)
{
    return wrapper->fmi2GetIntegerStatus(wrapper->component, statusKind, value);
}

PUBLIC_EXPORT int fmi2GetBooleanStatus(wrappedModel *wrapper, const int statusKind, int *value)
{
    return wrapper->fmi2GetBooleanStatus(wrapper->component, statusKind, value);
}
PUBLIC_EXPORT int fmi2GetStringStatus(wrappedModel *wrapper, const int statusKind, const char **value)
{
    return wrapper->fmi2GetStringStatus(wrapper->component, statusKind, value);
}
