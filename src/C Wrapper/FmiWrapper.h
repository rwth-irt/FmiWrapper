#pragma once
#include <stdbool.h>
#include <stddef.h>

/*!
    \brief A wrapper to simplify using fmus in other languages. The API is supposed to stay as close the fmi2 standard API as possible.

    Especially the variadic parameter of fmi2CallbackFunctions are troublesome:
    The allocation and freeing of raw memory usually isn't supported by higher languages.
    The logger callback uses variadic arguments which only exist in C.
    Also marshalling the structs correctly is hard.
    This library wraps those functions by supplying simplified callbacks and hiding the fmi2 structs.
*/


/* Definition of macro to export symbols. */
#if !defined(PUBLIC_EXPORT)
#if defined _WIN32 || defined __CYGWIN__
#define PUBLIC_EXPORT __declspec(dllexport)
#else
#if __GNUC__ >= 4
#define PUBLIC_EXPORT __attribute__ ((visibility ("default")))
#else
#define PUBLIC_EXPORT
#endif
#endif
#else
#define PUBLIC_EXPORT
#endif

/*! The state struct that contains enviroment callbacks and the fmu functions. */
typedef struct wrappedModel wrappedModel;

/*! A simplified log callback for the fmu. */
typedef void (*logCallback)(const char *instanceName, int status, const char *category, const char *message);
/*! A simplified stepFinished callback for the fmu. */
typedef void (*stepFinishedCallback)(int status);

/* Creation and destruction of FMU instances and setting debug status */

/*!
    \brief Create a instance of a fmu that uses the simplified callbacks.
    \param fileName The filename of the binary. Can be a relative or ideally a full path.
    \param logCallback This function will be called when the fmu logs.
    \param stepFinishedCallback This function will be called when a simulation step has finished.
    \param other These parameters match the ones from the fmi2 standard.
    \return A pointer to a component that wraps the fmu functions. Pass this pointer to the fmi2 function calls. Returns NULL if it failed.
*/
PUBLIC_EXPORT wrappedModel *fmi2Instantiate(const char *fileName, logCallback logCallback, stepFinishedCallback stepFinishedCallback,
                                            const char *instanceName, int fmuType, const char *guid, const char *resourceLocation, bool visible, bool loggingOn);
/*!
    \brief Releases the underlying fmu instance and the wrapper.
    \param wrapper Pointer returned by createWrapper.
*/
PUBLIC_EXPORT void fmi2FreeInstance(wrappedModel *wrapper);
PUBLIC_EXPORT int fmi2SetDebugLogging(wrappedModel *wrapper, bool loggingOn, size_t nCategories, const char *categories[]);

/* Inquire version numbers of header files and setting logging status */
PUBLIC_EXPORT const char *fmi2GetTypesPlatform(wrappedModel *wrapper);
PUBLIC_EXPORT const char *fmi2GetVersion(wrappedModel *wrapper);

/* Enter and exit initialization mode, terminate and reset */
PUBLIC_EXPORT int fmi2SetupExperiment(wrappedModel *wrapper, bool toleranceDefined, double tolerance, double startTime, bool stopTimeDefined, double stopTime);
PUBLIC_EXPORT int fmi2EnterInitializationMode(wrappedModel *wrapper);
PUBLIC_EXPORT int fmi2ExitInitializationMode(wrappedModel *wrapper);
PUBLIC_EXPORT int fmi2Terminate(wrappedModel *wrapper);
PUBLIC_EXPORT int fmi2Reset(wrappedModel *wrapper);

/* Getting and setting variable values */
PUBLIC_EXPORT int fmi2GetReal(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, double value[]);
PUBLIC_EXPORT int fmi2GetInteger(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, int value[]);
PUBLIC_EXPORT int fmi2GetBoolean(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, bool value[]);
PUBLIC_EXPORT int fmi2GetString(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, const char *value[]);

PUBLIC_EXPORT int fmi2SetReal(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, const double value[]);
PUBLIC_EXPORT int fmi2SetInteger(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, const int value[]);
PUBLIC_EXPORT int fmi2SetBoolean(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, const bool value[]);
PUBLIC_EXPORT int fmi2SetString(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, const char *value[]);

/* Getting and setting the internal FMU state */
PUBLIC_EXPORT int fmi2GetFMUstate(wrappedModel *wrapper, void **FMUstate);
PUBLIC_EXPORT int fmi2SetFMUstate(wrappedModel *wrapper, void *FMUstate);
PUBLIC_EXPORT int fmi2FreeFMUstate(wrappedModel *wrapper, void **FMUstate);
PUBLIC_EXPORT int fmi2SerializedFMUstateSize(wrappedModel *wrapper, void *FMUstate, size_t *size);
PUBLIC_EXPORT int fmi2SerializeFMUstate(wrappedModel *wrapper, void *FMUstate, char serializedState[], size_t size);
PUBLIC_EXPORT int fmi2DeSerializeFMUstate(wrappedModel *wrapper, const char serializedState[], size_t size, void **FMUstate);

/* Getting partial derivatives */
PUBLIC_EXPORT int fmi2GetDirectionalDerivative(wrappedModel *wrapper, const unsigned int vUnknown_ref[], size_t nUnknown,
                                               const unsigned int vKnown_ref[], size_t nKnown,
                                               const double dvKnown[], double dvUnknown[]);

/* **************************************************
Typess for Functions for FMI2 for Model Exchange
****************************************************/

/* Enter and exit the different modes */
PUBLIC_EXPORT int fmi2EnterEventMode(wrappedModel *wrapper);
PUBLIC_EXPORT int fmi2NewDiscreteStates(wrappedModel *wrapper, bool *newDiscreteStatesNeeded, bool *terminateSimulation, bool *nominalsOfContinuousStatesChanged, bool *valuesOfContinuousStatesChanged, bool *nextEventTimeDefined, double *nextEventTime);
PUBLIC_EXPORT int fmi2EnterContinuousTimeMode(wrappedModel *wrapper);
/*! All of the int & int * parameters are fmi2Boolean. */
PUBLIC_EXPORT int fmi2CompletedIntegratorStep(wrappedModel *wrapper, bool noSetFMUStatePriorToCurrentPoint, bool *enterEventMode, bool *terminateSimulation);

/* Providing independent variables and re-initialization of caching */
PUBLIC_EXPORT int fmi2SetTime(wrappedModel *wrapper, double time);
PUBLIC_EXPORT int fmi2SetContinuousStates(wrappedModel *wrapper, const double x[], size_t nx);

/* Evaluation of the model equations */
PUBLIC_EXPORT int fmi2GetDerivatives(wrappedModel *wrapper, double derivatives[], size_t nx);
PUBLIC_EXPORT int fmi2GetEventIndicators(wrappedModel *wrapper, double eventIndicators[], size_t ni);
PUBLIC_EXPORT int fmi2GetContinuousStates(wrappedModel *wrapper, double x[], size_t nx);
PUBLIC_EXPORT int fmi2GetNominalsOfContinuousStates(wrappedModel *wrapper, double x_nominal[], size_t nx);

/* **************************************************
Types for Functions for FMI2 for Co-Simulation
****************************************************/

/* Simulating the slave */
PUBLIC_EXPORT int fmi2SetRealInputDerivatives(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, const int order[], const double value[]);
PUBLIC_EXPORT int fmi2GetRealOutputDerivatives(wrappedModel *wrapper, const unsigned int vr[], size_t nvr, const int order[], double value[]);

PUBLIC_EXPORT int fmi2DoStep(wrappedModel *wrapper, double currentCommunicationPoint, double communicationStepSize, bool noSetFMUStatePriorToCurrentPoint);
PUBLIC_EXPORT int fmi2CancelStep(wrappedModel *wrapper);

/* Inquire slave status */
PUBLIC_EXPORT int fmi2GetStatus(wrappedModel *wrapper, const int statusKind, int *value);
PUBLIC_EXPORT int fmi2GetRealStatus(wrappedModel *wrapper, const int statusKind, double *value);
PUBLIC_EXPORT int fmi2GetIntegerStatus(wrappedModel *wrapper, const int statusKind, int *value);
PUBLIC_EXPORT int fmi2GetBooleanStatus(wrappedModel *wrapper, const int statusKind, int *value);
PUBLIC_EXPORT int fmi2GetStringStatus(wrappedModel *wrapper, const int statusKind, const char **value);
