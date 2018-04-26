using System;
using System.Runtime.InteropServices;

namespace FmiWrapper_Net
{
    /// <summary>
    /// This class provides static functions that call the C DLL
    /// </summary>
    public static class FmiFunctions
    {
        // Callback delegate definitions

        /// <summary>
        /// Fmu logs to the enviroment using this callback.
        /// </summary>
        /// <param name="instanceName"></param>
        /// <param name="status"></param>
        /// <param name="category"></param>
        /// <param name="message"></param>
        public delegate void LogCallback(string instanceName, Fmi2Status status, string category, string message);

        /// <summary>
        /// Gets called after an asynchronous simulation step finished.
        /// </summary>
        /// <param name="status"></param>
        public delegate void StepFinishedCallback(Fmi2Status status);

        #region Creation and destruction of FMU instances and setting debug status

        /// <summary>
        /// Creates a new instance of a fmu.
        /// </summary>
        /// <param name="fileName">The path to the binary.</param>
        /// <param name="logCallback"></param>
        /// <param name="stepFinishedCallback"></param>
        /// <param name="instanceName"></param>
        /// <param name="fmuType"></param>
        /// <param name="guid"></param>
        /// <param name="resourceLocation"></param>
        /// <param name="visible"></param>
        /// <param name="loggingOn"></param>
        /// <returns>A pointer to the instance. NULL if the instantiation failed.</returns>
        [DllImport("FmiWrapper.dll", EntryPoint = "instantiate", CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr Instantiate(
            [MarshalAs(UnmanagedType.LPStr)] string fileName,
            LogCallback logCallback, StepFinishedCallback stepFinishedCallback,
            [MarshalAs(UnmanagedType.LPStr)] string instanceName, Fmi2Type fmuType,
            [MarshalAs(UnmanagedType.LPStr)] string guid,
            [MarshalAs(UnmanagedType.LPStr)] string resourceLocation,
            [MarshalAs(UnmanagedType.Bool)] bool visible,
            [MarshalAs(UnmanagedType.Bool)] bool loggingOn);

        /// <summary>
        /// Frees all the resoureces used by this instance.
        /// </summary>
        /// <param name="wrapper"></param>
        [DllImport("FmiWrapper.dll", EntryPoint = "free_instance", CallingConvention = CallingConvention.Cdecl)]
        internal static extern void FreeInstance(IntPtr wrapper);

        [DllImport("FmiWrapper.dll", EntryPoint = "set_debug_logging", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int SetDebugLogging(IntPtr wrapper,
            [MarshalAs(UnmanagedType.Bool)] bool loggingOn, int nCategories,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr)] string[] categories);

        #endregion

        #region Inquire version numbers of header files and setting logging status

        /// <summary>
        /// Returns a IntPtr that points to the string that is returned bythe fmu function.
        /// </summary>
        /// <param name="wrapper"></param>
        /// <returns></returns>
        [DllImport("FmiWrapper.dll", EntryPoint = "get_types_platform", CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr GetTypesPlatform(IntPtr wrapper);

        /// <summary>
        /// Returns a IntPtr that points to the string that is returned bythe fmu function.
        /// </summary>
        /// <param name="wrapper"></param>
        /// <returns></returns>
        [DllImport("FmiWrapper.dll", EntryPoint = "get_version", CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr GetVersion(IntPtr wrapper);

        #endregion

        #region Enter and exit initialization mode, terminate and reset

        [DllImport("FmiWrapper.dll", EntryPoint = "setup_experiment", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status SetupExperiment(IntPtr wrapper,
            [MarshalAs(UnmanagedType.Bool)] bool toleranceDefined, double tolerance,
            double startTime, [MarshalAs(UnmanagedType.Bool)] bool stopTimeDefined, double stopTime);

        [DllImport("FmiWrapper.dll", EntryPoint = "enter_initialization_mode", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status EnterInitializationMode(IntPtr wrapper);

        [DllImport("FmiWrapper.dll", EntryPoint = "exit_initialization_mode", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status ExitInitializationMode(IntPtr wrapper);

        [DllImport("FmiWrapper.dll", EntryPoint = "terminate", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status Terminate(IntPtr wrapper);

        [DllImport("FmiWrapper.dll", EntryPoint = "reset", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status Reset(IntPtr wrapper);

        #endregion

        #region Getting and setting variables values 

        // Get values

        [DllImport("FmiWrapper.dll", EntryPoint = "get_real", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status GetReal(IntPtr wrapper, uint[] vr, UIntPtr nvr, [Out] double[] value);

        [DllImport("FmiWrapper.dll", EntryPoint = "get_integer", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status GetInteger(IntPtr wrapper, uint[] vr, UIntPtr nvr, [Out] int[] value);

        [DllImport("FmiWrapper.dll", EntryPoint = "get_boolean", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status GetBoolean(IntPtr wrapper, uint[] vr, UIntPtr nvr,
            [Out] [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.Bool)] bool[] value);

        /// <summary>
        /// Get string values from the fmu.
        /// </summary>
        /// <param name="wrapper"></param>
        /// <param name="vr"></param>
        /// <param name="nvr"></param>
        /// <param name="value">The elements of this array have to be converted to strings via Marshal.PtrToStringAnsi(IntPtr value).</param>
        /// <returns></returns>
        [DllImport("FmiWrapper.dll", EntryPoint = "get_string", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status GetString(IntPtr wrapper, uint[] vr, UIntPtr nvr, [Out] IntPtr[] value);

        // Set values

        [DllImport("FmiWrapper.dll", EntryPoint = "set_real", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status SetReal(IntPtr wrapper, uint[] vr, UIntPtr nvr, double[] value);

        [DllImport("FmiWrapper.dll", EntryPoint = "set_integer", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status SetInteger(IntPtr wrapper, uint[] vr, UIntPtr nvr, int[] value);

        [DllImport("FmiWrapper.dll", EntryPoint = "set_boolean", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status SetBoolean(IntPtr wrapper, uint[] vr, UIntPtr nvr,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.Bool)] bool[] value);

        [DllImport("FmiWrapper.dll", EntryPoint = "set_string", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status SetString(IntPtr wrapper, uint[] vr, UIntPtr nvr,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr)] string[] value);

        #endregion

        #region Types for Functions for FMI2 for Model Exchange

        #region Enter and exit the different modes

        [DllImport("FmiWrapper.dll", EntryPoint = "enter_event_mode", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status EnterEventMode(IntPtr wrapper);

        [DllImport("FmiWrapper.dll", EntryPoint = "new_discrete_states", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status NewDiscreteStates(IntPtr wrapper, ref Fmi2EventInfo eventInfo);

        [DllImport("FmiWrapper.dll", EntryPoint = "enter_continuous_time_mode", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status EnterContinuousTimeMode(IntPtr wrapper);

        [DllImport("FmiWrapper.dll", EntryPoint = "completed_integrator_step", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status CompletedIntegratorStep(IntPtr wrapper,
            [MarshalAs(UnmanagedType.Bool)] bool noSetFmuStatePriorToCurrentPoint,
            [MarshalAs(UnmanagedType.Bool)] ref bool enterEventMode,
            [MarshalAs(UnmanagedType.Bool)] ref bool terminateSimulation);

        #endregion

        #region Providing independent variables and re-initialization of caching

        [DllImport("FmiWrapper.dll", EntryPoint = "set_time", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status SetTime(IntPtr wrapper, double time);

        [DllImport("FmiWrapper.dll", EntryPoint = "set_continuous_states", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status SetContinuousStates(IntPtr wrapper, double[] x, UIntPtr nx);

        #endregion

        #region Evaluation of the model equations

        [DllImport("FmiWrapper.dll", EntryPoint = "get_derivatives", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status GetDerivatives(IntPtr wrapper, [Out] double[] derivatives, UIntPtr nx);

        [DllImport("FmiWrapper.dll", EntryPoint = "get_event_indicators", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status GetEventIndicators(IntPtr wrapper, [Out] double[] eventIndicators, UIntPtr ni);

        [DllImport("FmiWrapper.dll", EntryPoint = "get_continuous_states", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status GetContinuousStates(IntPtr wrapper, [Out] double[] x, UIntPtr nx);

        [DllImport("FmiWrapper.dll", EntryPoint = "get_nominals_of_continuous_states", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status GetNominalsOfContinuousStates(IntPtr wrapper, [Out] double[] x_nominal, UIntPtr nx);

        #endregion



        #endregion

        #region Types for Functions for FMI2 for Co-Simulation

        #region Simulating the slave

        [DllImport("FmiWrapper.dll", EntryPoint = "set_real_input_derivatives", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status SetRealInputDerivatives(IntPtr wrapper, uint[] vr, UIntPtr nvr, int[] order, double[] value);

        [DllImport("FmiWrapper.dll", EntryPoint = "get_real_output_derivatives", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status GetRealOutputDerivatives(IntPtr wrapper, uint[] vr, UIntPtr nvr, int[] order, [Out] double[] value);

        [DllImport("FmiWrapper.dll", EntryPoint = "do_step", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status DoStep(IntPtr wrapper,
            double currentCommunicationPoint, double communicationStepSize,
            [MarshalAs(UnmanagedType.Bool)] bool noSetFmuStatePriorToCurrentPoint);

        [DllImport("FmiWrapper.dll", EntryPoint = "cancel_step", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status CancelStep(IntPtr wrapper);

        #endregion

        #region Inquire slave status

        [DllImport("FmiWrapper.dll", EntryPoint = "get_status", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status GetStatus(IntPtr wrapper, Fmi2StatusKind statusKind, ref Fmi2Status value);

        [DllImport("FmiWrapper.dll", EntryPoint = "get_real_status", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status GetRealStatus(IntPtr wrapper, Fmi2StatusKind statusKind, ref double value);

        [DllImport("FmiWrapper.dll", EntryPoint = "get_integer_status", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status GetIntegerStatus(IntPtr wrapper, Fmi2StatusKind statusKind, ref int value);

        [DllImport("FmiWrapper.dll", EntryPoint = "get_boolean_status", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status GetBooleanStatus(IntPtr wrapper, Fmi2StatusKind statusKind, [MarshalAs(UnmanagedType.Bool)] ref bool value);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="wrapper"></param>
        /// <param name="statusKind"></param>
        /// <param name="value">Evaluteate with Marshal.PtrToStringAnsi.</param>
        /// <returns></returns>
        [DllImport("FmiWrapper.dll", EntryPoint = "get_string_status", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Fmi2Status GetStringStatus(IntPtr wrapper, Fmi2StatusKind statusKind, IntPtr value);

        #endregion

        #endregion

    }
}
