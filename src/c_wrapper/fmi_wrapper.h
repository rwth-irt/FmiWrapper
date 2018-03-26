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
typedef struct wrapped_fmu wrapped_fmu;

/*! A simplified log callback for the fmu. */
typedef void (*log_t)(const char *instance_name, int status, const char *category, const char *message);
/*! A simplified stepFinished callback for the fmu. */
typedef void (*step_finished_t)(int status);

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
                                            const char *instance_name, int fmu_type, const char *guid, const char *resource_location, bool visible, bool logging_on);
/*!
    \brief Releases the underlying fmu instance and the wrapper.
    \param wrapper Pointer returned by create_wrapper.
*/
PUBLIC_EXPORT void free_instance(wrapped_fmu *wrapper);
PUBLIC_EXPORT int set_debug_logging(wrapped_fmu *wrapper, bool logging_on, size_t n_categories, const char *categories[]);

/* Inquire version numbers of header files and setting logging status */
PUBLIC_EXPORT const char *get_types_platform(wrapped_fmu *wrapper);
PUBLIC_EXPORT const char *get_version(wrapped_fmu *wrapper);

/* Enter and exit initialization mode, terminate and reset */
PUBLIC_EXPORT int setup_experiment(wrapped_fmu *wrapper, bool tolerance_defined, double tolerance, double startTime, bool stop_time_defined, double stop_time);
PUBLIC_EXPORT int enter_initialization_mode(wrapped_fmu *wrapper);
PUBLIC_EXPORT int exit_initialization_mode(wrapped_fmu *wrapper);
PUBLIC_EXPORT int terminate(wrapped_fmu *wrapper);
PUBLIC_EXPORT int reset(wrapped_fmu *wrapper);

/* Getting and setting variable values */
PUBLIC_EXPORT int get_real(wrapped_fmu *wrapper, const unsigned int vr[], size_t nvr, double value[]);
PUBLIC_EXPORT int get_integer(wrapped_fmu *wrapper, const unsigned int vr[], size_t nvr, int value[]);
PUBLIC_EXPORT int get_boolean(wrapped_fmu *wrapper, const unsigned int vr[], size_t nvr, bool value[]);
PUBLIC_EXPORT int get_string(wrapped_fmu *wrapper, const unsigned int vr[], size_t nvr, const char *value[]);

PUBLIC_EXPORT int set_real(wrapped_fmu *wrapper, const unsigned int vr[], size_t nvr, const double value[]);
PUBLIC_EXPORT int set_integer(wrapped_fmu *wrapper, const unsigned int vr[], size_t nvr, const int value[]);
PUBLIC_EXPORT int set_boolean(wrapped_fmu *wrapper, const unsigned int vr[], size_t nvr, const bool value[]);
PUBLIC_EXPORT int set_string(wrapped_fmu *wrapper, const unsigned int vr[], size_t nvr, const char *value[]);

/* Getting and setting the internal FMU state */
PUBLIC_EXPORT int get_fmu_state(wrapped_fmu *wrapper, void **fmu_state);
PUBLIC_EXPORT int set_fmu_state(wrapped_fmu *wrapper, void *fmu_state);
PUBLIC_EXPORT int free_fmu_state(wrapped_fmu *wrapper, void **fmu_state);
PUBLIC_EXPORT int serialized_fmu_state_size(wrapped_fmu *wrapper, void *fmu_state, size_t *size);
PUBLIC_EXPORT int serialize_fmu_state(wrapped_fmu *wrapper, void *fmu_state, char serialized_state[], size_t size);
PUBLIC_EXPORT int deserialize_fmu_state(wrapped_fmu *wrapper, const char serialized_state[], size_t size, void **fmu_state);

/* Getting partial derivatives */
PUBLIC_EXPORT int get_directional_derivative(wrapped_fmu *wrapper, const unsigned int v_unknown_ref[], size_t n_unknown,
                                               const unsigned int v_known_ref[], size_t n_known,
                                               const double dv_known[], double dv_unknown[]);

/* **************************************************
Typess for Functions for FMI2 for Model Exchange
****************************************************/

/* Enter and exit the different modes */
PUBLIC_EXPORT int enter_event_mode(wrapped_fmu *wrapper);
PUBLIC_EXPORT int new_discrete_states(wrapped_fmu *wrapper, bool *new_discrete_states_needed, bool *terminate_simulation, 
                                      bool *nominals_of_continuous_states_changed, bool *values_of_continuous_states_changed, 
                                      bool *next_event_time_defined, double *next_event_time);
PUBLIC_EXPORT int enter_continuous_time_mode(wrapped_fmu *wrapper);
/*! All of the int & int * parameters are fmi2Boolean. */
PUBLIC_EXPORT int completed_integrator_step(wrapped_fmu *wrapper, bool no_set_fmu_state_prior_to_current_point, bool *enter_event_mode, bool *terminate_simulation);

/* Providing independent variables and re-initialization of caching */
PUBLIC_EXPORT int set_time(wrapped_fmu *wrapper, double time);
PUBLIC_EXPORT int set_continuous_states(wrapped_fmu *wrapper, const double x[], size_t nx);

/* Evaluation of the model equations */
PUBLIC_EXPORT int get_derivatives(wrapped_fmu *wrapper, double derivatives[], size_t nx);
PUBLIC_EXPORT int get_event_indicators(wrapped_fmu *wrapper, double event_indicators[], size_t ni);
PUBLIC_EXPORT int get_continuous_states(wrapped_fmu *wrapper, double x[], size_t nx);
PUBLIC_EXPORT int get_nominals_of_continuous_states(wrapped_fmu *wrapper, double x_nominal[], size_t nx);

/* **************************************************
Types for Functions for FMI2 for Co-Simulation
****************************************************/

/* Simulating the slave */
PUBLIC_EXPORT int set_real_input_derivatives(wrapped_fmu *wrapper, const unsigned int vr[], size_t nvr, const int order[], const double value[]);
PUBLIC_EXPORT int get_real_output_derivatives(wrapped_fmu *wrapper, const unsigned int vr[], size_t nvr, const int order[], double value[]);

PUBLIC_EXPORT int do_step(wrapped_fmu *wrapper, double current_communication_point, double communication_step_size, bool no_set_fmu_state_prior_to_current_point);
PUBLIC_EXPORT int cancel_step(wrapped_fmu *wrapper);

/* Inquire slave status */
PUBLIC_EXPORT int get_status(wrapped_fmu *wrapper, const int status_kind, int *value);
PUBLIC_EXPORT int get_real_status(wrapped_fmu *wrapper, const int status_kind, double *value);
PUBLIC_EXPORT int get_integer_status(wrapped_fmu *wrapper, const int status_ind, int *value);
PUBLIC_EXPORT int get_boolean_status(wrapped_fmu *wrapper, const int status_kind, int *value);
PUBLIC_EXPORT int get_string_status(wrapped_fmu *wrapper, const int status_kind, const char **value);
