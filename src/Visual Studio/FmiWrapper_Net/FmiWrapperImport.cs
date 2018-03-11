using System;
using System.Runtime.InteropServices;

namespace FmiWrapper_Net
{
    /// <summary>
    /// This class provides static functions that call the C DLL
    /// </summary>
    public static class FmiWrapperImport
    {
        // Callback delegate definitions

        /// <summary>
        /// Fmu logs to the enviroment using this callback.
        /// </summary>
        /// <param name="instanceName"></param>
        /// <param name="status"></param>
        /// <param name="category"></param>
        /// <param name="message"></param>
        public delegate void LogCallback(string instanceName, int status, string category, string message);

        /// <summary>
        /// Gets called after an asynchronous simulation step finished.
        /// </summary>
        /// <param name="status"></param>
        public delegate void StepFinishedCallback(int status);

        /* Creation and destruction of FMU instances and setting debug status */

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
        [DllImport("FmiWrapper.dll", EntryPoint = "fmi2Instantiate", CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr Fmi2Instantiate([MarshalAs(UnmanagedType.LPStr)] string fileName,
            LogCallback logCallback, StepFinishedCallback stepFinishedCallback,
            [MarshalAs(UnmanagedType.LPStr)] string instanceName, int fmuType, [MarshalAs(UnmanagedType.LPStr)] string guid,
            [MarshalAs(UnmanagedType.LPStr)] string resourceLocation, bool visible, bool loggingOn);

        /// <summary>
        /// Frees all the resoureces used by this instance.
        /// </summary>
        /// <param name="wrapper"></param>
        [DllImport("FmiWrapper.dll", EntryPoint = "fmi2FreeInstance", CallingConvention = CallingConvention.Cdecl)]
        internal static extern void Fmi2FreeInstance(IntPtr wrapper);

        [DllImport("FmiWrapper.dll", EntryPoint = "fmi2SetDebugLogging", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int Fmi2SetDebugLogging(IntPtr wrapper, bool loggingOn, int nCategories, [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr)] string[] categories);
        
        /* Inquire version numbers of header files and setting logging status */

        /// <summary>
        /// Returns a IntPtr that points to the string that is returned bythe fmu function.
        /// </summary>
        /// <param name="wrapper"></param>
        /// <returns></returns>
        [DllImport("FmiWrapper.dll", EntryPoint = "fmi2GetTypesPlatform", CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr Fmi2GetTypesPlatform(IntPtr wrapper);

        /// <summary>
        /// Returns a IntPtr that points to the string that is returned bythe fmu function.
        /// </summary>
        /// <param name="wrapper"></param>
        /// <returns></returns>
        [DllImport("FmiWrapper.dll", EntryPoint = "fmi2GetVersion", CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr Fmi2GetVersion(IntPtr wrapper);

        /* Enter and exit initialization mode, terminate and reset */

        [DllImport("FmiWrapper.dll", EntryPoint = "fmi2SetupExperiment", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int Fmi2SetupExperiment(IntPtr wrapper, bool toleranceDefined, double tolerance, double startTime, bool stopTimeDefined, double stopTime);

        [DllImport("FmiWrapper.dll", EntryPoint = "fmi2EnterInitializationMode", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int Fmi2EnterInitializationMode(IntPtr wrapper);

        [DllImport("FmiWrapper.dll", EntryPoint = "fmi2ExitInitializationMode", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int Fmi2ExitInitializationMode(IntPtr wrapper);

        [DllImport("FmiWrapper.dll", EntryPoint = "fmi2Terminate", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int Fmi2Terminate(IntPtr wrapper);

        [DllImport("FmiWrapper.dll", EntryPoint = "fmi2Reset", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int Fmi2Reset(IntPtr wrapper);
    }
}
