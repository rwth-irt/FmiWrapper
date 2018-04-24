#pragma once
#include "fmi2FunctionTypes.h"
#include <stdbool.h>
#include <stddef.h>

/*!
    \brief A wrapper to simplify using fmus in other languages. The API is supposed to stay as close the fmi2 standard API as possible.

    Especially the variadic parameter of fmi2CallbackFunctions are troublesome:
    The allocation and freeing of raw memory usually isn't supported by higher languages.
    The logger callback uses variadic arguments which only exist in C.
    This library wraps those functions by supplying simplified callbacks.
*/

/* Definition of macro to export symbols. */
#if !defined(PUBLIC_EXPORT)
#if defined _WIN32 || defined __CYGWIN__
#define PUBLIC_EXPORT __declspec(dllexport)
#else
#if __GNUC__ >= 4
#define PUBLIC_EXPORT __attribute__((visibility("default")))
#else
#define PUBLIC_EXPORT
#endif
#endif
#else
#define PUBLIC_EXPORT
#endif

/*! The state struct that contains enviroment callbacks and the fmu functions. */
typedef struct wrapped_fmu wrapped_fmu;

/*! A simplified log callback for the fmu. */
typedef void (*log_t)(fmi2String instance_name, fmi2Status status, fmi2String category, fmi2String message);
/*! A simplified stepFinished callback for the fmu. */
typedef void (*step_finished_t)(fmi2Status status);

/* Creation and destruction of FMU instances and setting debug status */

/*!
    \brief Create a instance of a fmu that uses the simplified callbacks.
    \param fileName The filename of the binary. Can be a relative or ideally a full path.
    \param logCallback This function will be called when the fmu logs.
    \param stepFinishedCallback This function will be called when a simulation step has finished.
    \param other These parameters match the ones from the fmi2 standard.
    \return A pointer to a component that wraps the fmu functions. Pass this pointer to the fmi2 function calls. Returns NULL if it failed.
*/
PUBLIC_EXPORT wrapped_fmu *instantiate(const char *file_name, log_t log, step_finished_t step_finished,
                                       fmi2String instance_name, fmi2Type fmu_type, fmi2String guid, fmi2String resource_location, fmi2Boolean visible, fmi2Boolean logging_on);
/*!
    \brief Releases the underlying fmu instance and the wrapper.
    \param wrapper Pointer returned by create_wrapper.
*/
PUBLIC_EXPORT void free_instance(wrapped_fmu *wrapper);
PUBLIC_EXPORT fmi2Status set_debug_logging(wrapped_fmu *wrapper, fmi2Boolean logging_on, size_t n_categories, fmi2String categories[]);

/* Inquire version numbers of header files and setting logging status */
PUBLIC_EXPORT const char *get_types_platform(wrapped_fmu *wrapper);
PUBLIC_EXPORT const char *get_version(wrapped_fmu *wrapper);

/* Enter and exit initialization mode, terminate and reset */
PUBLIC_EXPORT fmi2Status setup_experiment(wrapped_fmu *wrapper, fmi2Boolean tolerance_defined, fmi2Real tolerance, fmi2Real startTime, fmi2Boolean stop_time_defined, fmi2Real stop_time);
PUBLIC_EXPORT fmi2Status enter_initialization_mode(wrapped_fmu *wrapper);
PUBLIC_EXPORT fmi2Status exit_initialization_mode(wrapped_fmu *wrapper);
PUBLIC_EXPORT fmi2Status terminate(wrapped_fmu *wrapper);
PUBLIC_EXPORT fmi2Status reset(wrapped_fmu *wrapper);

/* Getting and setting variable values */
PUBLIC_EXPORT fmi2Status get_real(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[]);
PUBLIC_EXPORT fmi2Status get_integer(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[]);
PUBLIC_EXPORT fmi2Status get_boolean(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[]);
PUBLIC_EXPORT fmi2Status get_string(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, fmi2String value[]);

PUBLIC_EXPORT fmi2Status set_real(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[]);
PUBLIC_EXPORT fmi2Status set_integer(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[]);
PUBLIC_EXPORT fmi2Status set_boolean(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[]);
PUBLIC_EXPORT fmi2Status set_string(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, const fmi2String value[]);

/* Getting and setting the internal FMU state */
PUBLIC_EXPORT fmi2Status get_fmu_state(wrapped_fmu *wrapper, fmi2FMUstate *fmu_state);
PUBLIC_EXPORT fmi2Status set_fmu_state(wrapped_fmu *wrapper, fmi2FMUstate fmu_state);
PUBLIC_EXPORT fmi2Status free_fmu_state(wrapped_fmu *wrapper, fmi2FMUstate *fmu_state);
PUBLIC_EXPORT fmi2Status serialized_fmu_state_size(wrapped_fmu *wrapper, fmi2FMUstate fmu_state, size_t *size);
PUBLIC_EXPORT fmi2Status serialize_fmu_state(wrapped_fmu *wrapper, fmi2FMUstate fmu_state, fmi2Byte serialized_state[], size_t size);
PUBLIC_EXPORT fmi2Status deserialize_fmu_state(wrapped_fmu *wrapper, const fmi2Byte serialized_state[], size_t size, fmi2FMUstate *fmu_state);

/* Getting partial derivatives */
PUBLIC_EXPORT fmi2Status get_directional_derivative(wrapped_fmu *wrapper, const fmi2ValueReference v_unknown_ref[], size_t n_unknown,
                                                    const fmi2ValueReference v_known_ref[], size_t n_known,
                                                    const fmi2Real dv_known[], fmi2Real dv_unknown[]);

/* **************************************************
Types for Functions for FMI2 for Model Exchange
****************************************************/

/* Enter and exit the different modes */
PUBLIC_EXPORT fmi2Status enter_event_mode(wrapped_fmu *wrapper);
PUBLIC_EXPORT fmi2Status new_discrete_states(wrapped_fmu *wrapper, fmi2EventInfo *fmi2eventInfo);
PUBLIC_EXPORT fmi2Status enter_continuous_time_mode(wrapped_fmu *wrapper);
PUBLIC_EXPORT fmi2Status completed_integrator_step(wrapped_fmu *wrapper, fmi2Boolean no_set_fmu_state_prior_to_current_point, fmi2Boolean *enter_event_mode, fmi2Boolean *terminate_simulation);

/* Providing independent variables and re-initialization of caching */
PUBLIC_EXPORT fmi2Status set_time(wrapped_fmu *wrapper, fmi2Real time);
PUBLIC_EXPORT fmi2Status set_continuous_states(wrapped_fmu *wrapper, const fmi2Real x[], size_t nx);

/* Evaluation of the model equations */
PUBLIC_EXPORT fmi2Status get_derivatives(wrapped_fmu *wrapper, fmi2Real derivatives[], size_t nx);
PUBLIC_EXPORT fmi2Status get_event_indicators(wrapped_fmu *wrapper, fmi2Real event_indicators[], size_t ni);
PUBLIC_EXPORT fmi2Status get_continuous_states(wrapped_fmu *wrapper, fmi2Real x[], size_t nx);
PUBLIC_EXPORT fmi2Status get_nominals_of_continuous_states(wrapped_fmu *wrapper, fmi2Real x_nominal[], size_t nx);

/* **************************************************
Types for Functions for FMI2 for Co-Simulation
****************************************************/

/* Simulating the slave */
PUBLIC_EXPORT fmi2Status set_real_input_derivatives(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer order[], const fmi2Real value[]);
PUBLIC_EXPORT fmi2Status get_real_output_derivatives(wrapped_fmu *wrapper, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer order[], fmi2Real value[]);

PUBLIC_EXPORT fmi2Status do_step(wrapped_fmu *wrapper, fmi2Real current_communication_point, fmi2Real communication_step_size, fmi2Boolean no_set_fmu_state_prior_to_current_point);
PUBLIC_EXPORT fmi2Status cancel_step(wrapped_fmu *wrapper);

/* Inquire slave status */
PUBLIC_EXPORT fmi2Status get_status(wrapped_fmu *wrapper, const fmi2StatusKind status_kind, fmi2Status *value);
PUBLIC_EXPORT fmi2Status get_real_status(wrapped_fmu *wrapper, const fmi2StatusKind status_kind, fmi2Real *value);
PUBLIC_EXPORT fmi2Status get_integer_status(wrapped_fmu *wrapper, const fmi2StatusKind status_kind, fmi2Integer *value);
PUBLIC_EXPORT fmi2Status get_boolean_status(wrapped_fmu *wrapper, const fmi2StatusKind status_kind, fmi2Boolean *value);
PUBLIC_EXPORT fmi2Status get_string_status(wrapped_fmu *wrapper, const fmi2StatusKind status_kind, fmi2String *value);
