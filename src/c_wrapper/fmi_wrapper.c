#include "fmi_wrapper.h"
#include "system_functions.h"
#include "fmi2FunctionTypes.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

struct wrapped_fmu
{
    /*! The handle to the shared library. */
    void *shared_library_handle;
    /*! The component returned when instantiating the fmu. */
    fmi2Component component;
    /*! Store the callback functions because some fmus just store the pointer to the struct */
    fmi2CallbackFunctions *callback_functions;
    /*! Callback to forward logs from the fmu to the calling enviroment. */
    log_t log;
    /*! Callback to forward the step finished even from the fmu to the calling enviroment. */
    step_finished_t step_finished;

    /* **************************************************
    Common Functions
    ****************************************************/

    /* Inquire version numbers of header files */
    fmi2GetTypesPlatformTYPE *get_types_platform;
    fmi2GetVersionTYPE *get_version;
    fmi2SetDebugLoggingTYPE *set_debug_logging;

    /* Creation and destruction of FMU instances */
    fmi2InstantiateTYPE *instantiate;
    fmi2FreeInstanceTYPE *free_instance;

    /* Enter and exit initialization mode, terminate and reset */
    fmi2SetupExperimentTYPE *setup_experiment;
    fmi2EnterInitializationModeTYPE *enter_initialization_mode;
    fmi2ExitInitializationModeTYPE *exit_initialization_mode;
    fmi2TerminateTYPE *terminate;
    fmi2ResetTYPE *reset;

    /* Getting and setting variables values */
    fmi2GetRealTYPE *get_real;
    fmi2GetIntegerTYPE *get_integer;
    fmi2GetBooleanTYPE *get_boolean;
    fmi2GetStringTYPE *get_string;

    fmi2SetRealTYPE *set_real;
    fmi2SetIntegerTYPE *set_integer;
    fmi2SetBooleanTYPE *set_boolean;
    fmi2SetStringTYPE *set_string;

    /* Getting and setting the internal FMU state */
    fmi2GetFMUstateTYPE *get_fmu_state;
    fmi2SetFMUstateTYPE *set_fmu_state;
    fmi2FreeFMUstateTYPE *free_fmu_state;
    fmi2SerializedFMUstateSizeTYPE *serialized_fmu_state_size;
    fmi2SerializeFMUstateTYPE *serialize_fmu_state;
    fmi2DeSerializeFMUstateTYPE *deserialize_fmu_state;

    /* Getting partial derivatives */
    fmi2GetDirectionalDerivativeTYPE *get_directional_derivative;

    /* **************************************************
    Functions for FMI2 for Model Exchange
    ****************************************************/

    /* Enter and exit the different modes */
    fmi2EnterEventModeTYPE *enter_event_mode;
    fmi2NewDiscreteStatesTYPE *new_discrete_states;
    fmi2EnterContinuousTimeModeTYPE *enter_continuous_time_mode;
    fmi2CompletedIntegratorStepTYPE *completed_integrator_step;

    /* Providing independent variables and re-initialization of caching */
    fmi2SetTimeTYPE *set_time;
    fmi2SetContinuousStatesTYPE *set_continuous_states;

    /* Evaluation of the model equations */
    fmi2GetDerivativesTYPE *get_derivatives;
    fmi2GetEventIndicatorsTYPE *get_event_indicators;
    fmi2GetContinuousStatesTYPE *get_continuous_states;
    fmi2GetNominalsOfContinuousStatesTYPE *get_nominals_of_continuous_states;

    /* **************************************************
    Functions for FMI2 for Co-Simulation
    ****************************************************/

    /* Simulating the slave */
    fmi2SetRealInputDerivativesTYPE *set_real_input_derivatives;
    fmi2GetRealOutputDerivativesTYPE *get_real_output_derivatives;

    fmi2DoStepTYPE *do_step;
    fmi2CancelStepTYPE *cancel_step;

    /* Inquire slave status */
    fmi2GetStatusTYPE *get_status;
    fmi2GetRealStatusTYPE *get_real_status;
    fmi2GetIntegerStatusTYPE *get_integer_status;
    fmi2GetBooleanStatusTYPE *get_boolean_status;
    fmi2GetStringStatusTYPE *get_string_status;
};

/*!
Implementation of the stepFinished callback that is passed to the fmu.
This function calls the stepFinishedCallback of the wrapper.
*/
static void fmuLogCallback(fmi2ComponentEnvironment component_environment, fmi2String instance_name, fmi2Status status, fmi2String category, fmi2String message, ...)
{
    wrapped_fmu *wrapper = (wrapped_fmu *)component_environment;
    // fmi2standard: The message is to be used like sprintf.
    // For simplification apply the variadic arguments to the format string and call enviromentLog with this single string
    va_list args;
    va_start(args, message);
    // Get the size of the string + 1 for the \0 char.
    int needed_size = vsnprintf(NULL, 0, message, args) + 1;
    // Create and read into buffer with size + 1 (for \0 character)
    char *buffer = malloc(needed_size);
    vsnprintf(buffer, needed_size, message, args);
    wrapper->log(instance_name, status, category, buffer);
    free(buffer);
    va_end(args);
}

/*!
Implementation of the logger callback that is passed to the fmu.
This function calls the logCallback of the wrapper.
*/
static void fmuStepFinished(fmi2ComponentEnvironment component_environment, fmi2Status status)
{
    wrapped_fmu *wrapper = (wrapped_fmu *)component_environment;
    wrapper->step_finished(status);
}

/*! Load the functions from the binary into a wrapper struct. */
wrapped_fmu *create_wrapper(const char *file_name, log_t log_t, step_finished_t step_finished_t)
{
    // Create the wrapper struct
    wrapped_fmu *wrapper = malloc(sizeof *wrapper);
    wrapper->shared_library_handle = loadSharedLibrary(file_name);
    if (wrapper->shared_library_handle == NULL)
    {
        // Failed to load the library.
        return NULL;
    }
    wrapper->log = log_t;
    wrapper->step_finished = step_finished_t;
    // Load all the funcions
    /* Inquire version numbers of header files */
    wrapper->get_types_platform = getFunction(wrapper->shared_library_handle, "fmi2GetTypesPlatform");
    wrapper->get_version = getFunction(wrapper->shared_library_handle, "fmi2GetVersion");
    wrapper->set_debug_logging = getFunction(wrapper->shared_library_handle, "fmi2SetDebugLogging");
    /* Creation and destruction of FMU instances */
    wrapper->instantiate = getFunction(wrapper->shared_library_handle, "fmi2Instantiate");
    wrapper->free_instance = getFunction(wrapper->shared_library_handle, "fmi2FreeInstance");
    /* Enter and exit initialization mode, terminate and reset */
    wrapper->setup_experiment = getFunction(wrapper->shared_library_handle, "fmi2SetupExperiment");
    wrapper->enter_initialization_mode = getFunction(wrapper->shared_library_handle, "fmi2EnterInitializationMode");
    wrapper->exit_initialization_mode = getFunction(wrapper->shared_library_handle, "fmi2ExitInitializationMode");
    wrapper->terminate = getFunction(wrapper->shared_library_handle, "fmi2Terminate");
    wrapper->reset = getFunction(wrapper->shared_library_handle, "fmi2Reset");
    /* Getting and setting variables values */
    wrapper->get_real = getFunction(wrapper->shared_library_handle, "fmi2GetReal");
    wrapper->get_integer = getFunction(wrapper->shared_library_handle, "fmi2GetInteger");
    wrapper->get_boolean = getFunction(wrapper->shared_library_handle, "fmi2GetBoolean");
    wrapper->get_string = getFunction(wrapper->shared_library_handle, "fmi2GetString");

    wrapper->set_real = getFunction(wrapper->shared_library_handle, "fmi2SetReal");
    wrapper->set_integer = getFunction(wrapper->shared_library_handle, "fmi2SetInteger");
    wrapper->set_boolean = getFunction(wrapper->shared_library_handle, "fmi2SetBoolean");
    wrapper->set_string = getFunction(wrapper->shared_library_handle, "fmi2SetString");
    /* Getting and setting the internal FMU state */
    wrapper->get_fmu_state = getFunction(wrapper->shared_library_handle, "fmi2GetFMUstate");
    wrapper->set_fmu_state = getFunction(wrapper->shared_library_handle, "fmi2SetFMUstate");
    wrapper->free_fmu_state = getFunction(wrapper->shared_library_handle, "fmi2FreeFMUstate");
    wrapper->serialized_fmu_state_size = getFunction(wrapper->shared_library_handle, "SerializedFMUstateSize");
    wrapper->serialize_fmu_state = getFunction(wrapper->shared_library_handle, "fmi2SerializeFMUstate");
    wrapper->deserialize_fmu_state = getFunction(wrapper->shared_library_handle, "fmi2DeSerializeFMUstate");
    /* Getting partial derivatives */
    wrapper->get_directional_derivative = getFunction(wrapper->shared_library_handle, "fmi2GetDirectionalDerivative");
    /* Enter and exit the different modes */
    wrapper->enter_event_mode = getFunction(wrapper->shared_library_handle, "fmi2EnterEventMode");
    wrapper->new_discrete_states = getFunction(wrapper->shared_library_handle, "fmi2NewDiscreteStates");
    wrapper->enter_continuous_time_mode = getFunction(wrapper->shared_library_handle, "fmi2EnterContinuousTimeMode");
    wrapper->completed_integrator_step = getFunction(wrapper->shared_library_handle, "fmi2CompletedIntegratorStep");
    /* Providing independent variables and re-initialization of caching */
    wrapper->set_time = getFunction(wrapper->shared_library_handle, "fmi2SetTime");
    wrapper->set_continuous_states = getFunction(wrapper->shared_library_handle, "fmi2SetContinuousStates");
    /* Evaluation of the model equations */
    wrapper->get_derivatives = getFunction(wrapper->shared_library_handle, "fmi2GetDerivatives");
    wrapper->get_event_indicators = getFunction(wrapper->shared_library_handle, "fmi2GetEventIndicators");
    wrapper->get_continuous_states = getFunction(wrapper->shared_library_handle, "fmi2GetContinuousStates");
    wrapper->get_nominals_of_continuous_states = getFunction(wrapper->shared_library_handle, "fmi2GetNominalsOfContinuousStates");
    /* Simulating the slave */
    wrapper->set_real_input_derivatives = getFunction(wrapper->shared_library_handle, "fmi2SetRealInputDerivatives");
    wrapper->get_real_output_derivatives = getFunction(wrapper->shared_library_handle, "fmi2GetRealOutputDerivatives");

    wrapper->do_step = getFunction(wrapper->shared_library_handle, "fmi2DoStep");
    wrapper->cancel_step = getFunction(wrapper->shared_library_handle, "fmi2CancelStep");
    /* Inquire slave status */
    wrapper->get_status = getFunction(wrapper->shared_library_handle, "fmi2GetStatus");
    wrapper->get_real_status = getFunction(wrapper->shared_library_handle, "fmi2GetRealStatus");
    wrapper->get_integer_status = getFunction(wrapper->shared_library_handle, "fmi2GetIntegerStatus");
    wrapper->get_boolean_status = getFunction(wrapper->shared_library_handle, "fmi2GetBooleanStatus");
    wrapper->get_string_status = getFunction(wrapper->shared_library_handle, "fmi2GetStringStatus");
    return wrapper;
}

/*! Free the handle and memory of the wrapper. */
void free_wrapper(wrapped_fmu *wrapper)
{
    freeSharedLibrary(wrapper->shared_library_handle);
    free(wrapper->callback_functions);
    free(wrapper);
}

/* Creation and destruction of FMU instances and setting debug status */

PUBLIC_EXPORT wrapped_fmu *instantiate(const char *file_name, log_t log, step_finished_t step_finished,
                                       fmi2String instance_name, fmi2Type fmu_type, fmi2String guid, fmi2String resource_location, fmi2Boolean visible, fmi2Boolean logging_on)
{
    // Load the functions from the binary
    wrapped_fmu *wrapper = create_wrapper(file_name, log, step_finished);
    if (wrapper == NULL)
    {
        return NULL;
    }
    // Supply static callback functions and the wrapper. The wrapper contains the callbacks of the enviroment.
    wrapper->callback_functions = malloc(sizeof(*wrapper->callback_functions));
    fmi2CallbackFunctions callbacks = {
        fmuLogCallback,
        calloc,
        free,
        fmuStepFinished,
        wrapper };
    memcpy(wrapper->callback_functions, &callbacks, sizeof(*wrapper->callback_functions));
    // Instantiate the fmu
    wrapper->component = wrapper->instantiate(instance_name, fmu_type, guid, resource_location, wrapper->callback_functions, visible, logging_on);
    if (wrapper->component == NULL)
    {
        // Failed, release the wrapper
        free_wrapper(wrapper);
        return NULL;
    }
    return wrapper;
}

PUBLIC_EXPORT void free_instance(wrapped_fmu *wrapper)
{
    wrapper->free_instance(wrapper->component);
    free_wrapper(wrapper);
}

/* Inquire version numbers of header files and setting logging status */

PUBLIC_EXPORT const char *get_types_platform(wrapped_fmu *wrapper)
{
    return wrapper->get_types_platform();
}

PUBLIC_EXPORT const char *get_version(wrapped_fmu *wrapper)
{
    return wrapper->get_version();
}

PUBLIC_EXPORT int set_debug_logging(wrapped_fmu *wrapper, fmi2Boolean logging_on, size_t n_categories, fmi2String categories[])
{
    return wrapper->set_debug_logging(wrapper->component, logging_on, n_categories, categories);
}

/* Enter and exit initialization mode, terminate and reset */

PUBLIC_EXPORT fmi2Status setup_experiment(wrapped_fmu *wrapper, fmi2Boolean tolerance_defined, fmi2Real tolerance, fmi2Real start_time, fmi2Boolean stop_time_defined, fmi2Real stop_time)
{
    return wrapper->setup_experiment(wrapper->component, tolerance_defined, tolerance, start_time, stop_time_defined, stop_time);
}

PUBLIC_EXPORT fmi2Status enter_initialization_mode(wrapped_fmu *wrapper)
{
    return wrapper->enter_initialization_mode(wrapper->component);
}

PUBLIC_EXPORT fmi2Status exit_initialization_mode(wrapped_fmu *wrapper)
{
    return wrapper->exit_initialization_mode(wrapper->component);
}

PUBLIC_EXPORT fmi2Status terminate(wrapped_fmu *wrapper)
{
    return wrapper->terminate(wrapper->component);
}

PUBLIC_EXPORT fmi2Status reset(wrapped_fmu *wrapper)
{
    return wrapper->reset(wrapper->component);
}

/* Getting and setting variable values */
PUBLIC_EXPORT fmi2Status get_real(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[])
{
    return wrapper->get_real(wrapper->component, vr, nvr, value);
}

PUBLIC_EXPORT fmi2Status get_integer(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, int value[])
{
    return wrapper->get_integer(wrapper->component, vr, nvr, value);
}

PUBLIC_EXPORT fmi2Status get_boolean(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[])
{
    return wrapper->get_boolean(wrapper->component, vr, nvr, value);
}

PUBLIC_EXPORT fmi2Status get_string(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, fmi2String value[])
{
    return wrapper->get_string(wrapper->component, vr, nvr, value);
}

PUBLIC_EXPORT fmi2Status set_real(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[])
{
    return wrapper->set_real(wrapper->component, vr, nvr, value);
}

PUBLIC_EXPORT fmi2Status set_integer(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, const int value[])
{
    return wrapper->set_integer(wrapper->component, vr, nvr, value);
}

PUBLIC_EXPORT fmi2Status set_boolean(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[])
{
    return wrapper->set_boolean(wrapper->component, vr, nvr, value);
}

PUBLIC_EXPORT fmi2Status set_string(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, const fmi2String value[])
{
    return wrapper->set_string(wrapper->component, vr, nvr, value);
}

/* Getting and setting the internal FMU state */
PUBLIC_EXPORT fmi2Status get_fmu_state(wrapped_fmu *wrapper, fmi2FMUstate *fmu_state)
{
    return wrapper->get_fmu_state(wrapper->component, fmu_state);
}
PUBLIC_EXPORT fmi2Status set_fmu_state(wrapped_fmu *wrapper, fmi2FMUstate fmu_state)
{
    return wrapper->set_fmu_state(wrapper->component, fmu_state);
}
PUBLIC_EXPORT fmi2Status free_fmu_state(wrapped_fmu *wrapper, fmi2FMUstate *fmu_state)
{
    return wrapper->free_fmu_state(wrapper->component, fmu_state);
}
PUBLIC_EXPORT fmi2Status serialized_fmu_state_size(wrapped_fmu *wrapper, fmi2FMUstate fmu_state, size_t *size)
{
    return wrapper->serialized_fmu_state_size(wrapper->component, fmu_state, size);
}
PUBLIC_EXPORT fmi2Status serialize_fmu_state(wrapped_fmu *wrapper, fmi2FMUstate fmu_state, fmi2Byte serialized_state[], size_t size)
{
    return wrapper->serialize_fmu_state(wrapper->component, fmu_state, serialized_state, size);
}

PUBLIC_EXPORT fmi2Status deserialize_fmu_state(wrapped_fmu *wrapper, const fmi2Byte serialized_state[], size_t size,fmi2FMUstate *fmu_state)
{
    return wrapper->deserialize_fmu_state(wrapper->component, serialized_state, size, fmu_state);
}

/* Getting partial derivatives */
PUBLIC_EXPORT fmi2Status get_directional_derivative(wrapped_fmu *wrapper, const fmi2ValueReference v_unknown_ref[], size_t n_unknown,
                                                    const fmi2ValueReference v_known_ref[], size_t n_known,
                                                    const fmi2Real dv_known[], fmi2Real dv_unknown[])
{
    return wrapper->get_directional_derivative(wrapper->component, v_unknown_ref, n_unknown, v_known_ref, n_known, dv_known, dv_unknown);
}

/* **************************************************
Typess for Functions for FMI2 for Model Exchange
****************************************************/

/* Enter and exit the different modes */
PUBLIC_EXPORT fmi2Status enter_event_mode(wrapped_fmu *wrapper)
{
    return wrapper->enter_event_mode(wrapper->component);
}

PUBLIC_EXPORT fmi2Status new_discrete_states(wrapped_fmu *wrapper, fmi2Boolean *new_discrete_states_needed, fmi2Boolean *terminate_simulation,
                                             fmi2Boolean *nominals_of_continuous_states_changed, fmi2Boolean *values_of_continuous_states_changed,
                                             fmi2Boolean *next_event_time_defined, fmi2Real *next_event_time)
{
    // The struct is a pain to marshal so provide it here and update the reference values.
    fmi2EventInfo info = { 0 };
    fmi2Status result = wrapper->new_discrete_states(wrapper->component, &info);
    *new_discrete_states_needed = info.newDiscreteStatesNeeded;
    *terminate_simulation = info.terminateSimulation;
    *nominals_of_continuous_states_changed = info.nominalsOfContinuousStatesChanged;
    *values_of_continuous_states_changed = info.valuesOfContinuousStatesChanged;
    *next_event_time_defined = info.nextEventTimeDefined;
    *next_event_time = info.nextEventTime;
    return result;
}

PUBLIC_EXPORT fmi2Status enter_continuous_time_mode(wrapped_fmu *wrapper)
{
    return wrapper->enter_continuous_time_mode(wrapper->component);
}

PUBLIC_EXPORT fmi2Status completed_integrator_step(wrapped_fmu *wrapper, fmi2Boolean no_set_fmu_state_prior_to_current_point, fmi2Boolean *enter_event_mode, fmi2Boolean *terminate_simulation)
{
    fmi2Status result = wrapper->completed_integrator_step(wrapper->component, no_set_fmu_state_prior_to_current_point, enter_event_mode, terminate_simulation);
    return result;
}

/* Providing independent variables and re-initialization of caching */
PUBLIC_EXPORT fmi2Status set_time(wrapped_fmu *wrapper, fmi2Real time)
{
    return wrapper->set_time(wrapper->component, time);
}

PUBLIC_EXPORT fmi2Status set_continuous_states(wrapped_fmu *wrapper, const fmi2Real x[], size_t nx)
{
    return wrapper->set_continuous_states(wrapper->component, x, nx);
}

/* Evaluation of the model equations */
PUBLIC_EXPORT fmi2Status get_derivatives(wrapped_fmu *wrapper, fmi2Real derivatives[], size_t nx)
{
    return wrapper->get_derivatives(wrapper->component, derivatives, nx);
}

PUBLIC_EXPORT fmi2Status get_event_indicators(wrapped_fmu *wrapper, fmi2Real eventIndicators[], size_t ni)
{
    return wrapper->get_event_indicators(wrapper->component, eventIndicators, ni);
}

PUBLIC_EXPORT fmi2Status get_continuous_states(wrapped_fmu *wrapper, fmi2Real x[], size_t nx)
{
    return wrapper->get_continuous_states(wrapper->component, x, nx);
}

PUBLIC_EXPORT fmi2Status get_nominals_of_continuous_states(wrapped_fmu *wrapper, fmi2Real x_nominal[], size_t nx)
{
    return wrapper->get_nominals_of_continuous_states(wrapper->component, x_nominal, nx);
}

/* **************************************************
Types for Functions for FMI2 for Co-Simulation
****************************************************/

/* Simulating the slave */
PUBLIC_EXPORT fmi2Status set_real_input_derivatives(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, const int order[], const fmi2Real value[])
{
    return wrapper->set_real_input_derivatives(wrapper->component, vr, nvr, order, value);
}

PUBLIC_EXPORT fmi2Status get_real_output_derivatives(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, const int order[], fmi2Real value[])
{
    return wrapper->get_real_output_derivatives(wrapper->component, vr, nvr, order, value);
}

PUBLIC_EXPORT fmi2Status do_step(wrapped_fmu *wrapper, fmi2Real current_communication_point, fmi2Real communication_step_size, fmi2Boolean no_set_fmu_state_prior_to_current_point)
{
    return wrapper->do_step(wrapper->component, current_communication_point, communication_step_size, no_set_fmu_state_prior_to_current_point);
}

PUBLIC_EXPORT fmi2Status cancel_step(wrapped_fmu *wrapper)
{
    return wrapper->cancel_step(wrapper->component);
}

/* Inquire slave status */
PUBLIC_EXPORT fmi2Status get_status(wrapped_fmu *wrapper, const int status_kind, fmi2Status *value)
{
    // Pass in the original enum and then cast it to int
    fmi2Status val;
    fmi2Status result = wrapper->get_status(wrapper->component, status_kind, &val);
    *value = val;
    return result;
}

PUBLIC_EXPORT fmi2Status get_real_status(wrapped_fmu *wrapper, const int status_kind, fmi2Real *value)
{
    return wrapper->get_real_status(wrapper->component, status_kind, value);
}

PUBLIC_EXPORT fmi2Status get_integer_status(wrapped_fmu *wrapper, const int status_kind, int *value)
{
    return wrapper->get_integer_status(wrapper->component, status_kind, value);
}

PUBLIC_EXPORT fmi2Status get_boolean_status(wrapped_fmu *wrapper, const int status_kind, fmi2Boolean *value)
{
    return wrapper->get_boolean_status(wrapper->component, status_kind, value);
}
PUBLIC_EXPORT fmi2Status get_string_status(wrapped_fmu *wrapper, const int status_kind, fmi2String *value)
{
    return wrapper->get_string_status(wrapper->component, status_kind, value);
}
