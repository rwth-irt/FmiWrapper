using System;
using System.Linq;
using System.Runtime.InteropServices;

namespace FmiWrapper_Net
{
    /// <summary>
    /// Disposable pattern ensures that the unmanged resources are freed.
    /// Instantiate is separated from constructor so the caller can register the callbacks before instantiation.
    /// </summary>
    public class FmuInstance : IDisposable
    {
        private string fileName;
        private IntPtr wrapper;

        // Log event to wrap callback
        public event FmiFunctions.LogCallback Log;
        private void OnLog(string instanceName, int status, string category, string message) =>
            Log?.Invoke(instanceName, status, category, message);
        // StepFinished event to wrap callback
        public event FmiFunctions.StepFinishedCallback StepFinished;
        private void OnStepFinished(int status) =>
            StepFinished?.Invoke(status);

        /// <summary>
        /// The path to the binary of the fmu.
        /// </summary>
        /// <param name="fileName"></param>
        public FmuInstance(string fileName)
        {
            this.fileName = fileName;
        }

        #region Creation and destruction of FMU instances and setting debug status

        /// <summary>
        /// Instantiates the fmu if it has not been instantiated yet.
        /// For reinstantiation call Reset or FreeInstance and then Instantiate.
        /// Throws an exception if the instantiation failed.
        /// </summary>
        /// <param name="instanceName"></param>
        /// <param name="fmuType"></param>
        /// <param name="guid"></param>
        /// <param name="resourceLocation"></param>
        /// <param name="visible"></param>
        /// <param name="loggingOn"></param>
        public void Instantiate(string instanceName, Fmi2Type fmuType, string guid, string resourceLocation, bool visible, bool loggingOn)
        {
            // Only instantiate if no instance is stored
            if (wrapper == IntPtr.Zero)
            {
                // Load the model instance
                wrapper = FmiFunctions.Instantiate(fileName, new FmiFunctions.LogCallback(OnLog), new FmiFunctions.StepFinishedCallback(OnStepFinished),
                    instanceName, fmuType, guid, resourceLocation, visible, loggingOn);
                if (wrapper == IntPtr.Zero)
                {
                    // Zero is returned when instantiation failed.
                    throw new Exception("Failed to instantiate the fmu instance " + instanceName);
                }
            }
        }

        public void FreeInstance()
        {
            if (wrapper != IntPtr.Zero)
                FmiFunctions.FreeInstance(wrapper);
            wrapper = IntPtr.Zero;
        }

        #endregion

        #region Inquire version numbers of header files and setting logging status

        public string GetTypesPlatform()
        {
            if (wrapper != IntPtr.Zero)
                return Marshal.PtrToStringAnsi(FmiFunctions.GetTypesPlatform(wrapper));
            else
                return "";
        }

        public string GetVersion()
        {
            if (wrapper != IntPtr.Zero)
                return Marshal.PtrToStringAnsi(FmiFunctions.GetVersion(wrapper));
            else
                return "";
        }

        #endregion

        #region Enter and exit initialization mode, terminate and reset

        public Fmi2Status SetupExperiment(bool toleranceDefined, double tolerance, double startTime, bool stopTimeDefined, double stopTime)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.SetupExperiment(wrapper, toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime);
            else
                return Fmi2Status.fmi2Fatal;
        }


        public Fmi2Status EnterInitializationMode()
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.EnterInitializationMode(wrapper);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status ExitInitializationMode()
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.ExitInitializationMode(wrapper);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status Terminate()
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.Terminate(wrapper);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status Reset()
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.Reset(wrapper);
            else
                return Fmi2Status.fmi2Fatal;

        }

        #endregion

        #region Getting and setting variables values 

        public Fmi2Status GetReal(uint[] vr, out double[] value)
        {
            value = new double[vr.Length];
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetReal(wrapper, vr, new UIntPtr((ulong)vr.Length), value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status GetInteger(uint[] vr, out int[] value)
        {
            value = new int[vr.Length];
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetInteger(wrapper, vr, new UIntPtr((ulong)vr.Length), value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status GetBoolean(uint[] vr, out bool[] value)
        {
            value = new bool[vr.Length];
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetBoolean(wrapper, vr, new UIntPtr((ulong)vr.Length), value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status GetString(uint[] vr, out string[] value)
        {
            value = new string[vr.Length];
            if (wrapper != IntPtr.Zero)
            {
                var valuePtrs = new IntPtr[vr.Length];
                var status = (Fmi2Status)FmiFunctions.GetString(wrapper, vr, new UIntPtr((ulong)vr.Length), valuePtrs);
                value = valuePtrs
                    .Select((p) => Marshal.PtrToStringAnsi(p))
                    .ToArray();
                return status;
            }
            else
                return Fmi2Status.fmi2Fatal;
        }


        public Fmi2Status SetReal(uint[] vr, double[] value)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.SetReal(wrapper, vr, new UIntPtr((ulong)vr.Length), value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status SetInteger(uint[] vr, int[] value)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.SetInteger(wrapper, vr, new UIntPtr((ulong)vr.Length), value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status SetBoolean(uint[] vr, bool[] value)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.SetBoolean(wrapper, vr, new UIntPtr((ulong)vr.Length), value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status SetString(uint[] vr, string[] value)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.SetString(wrapper, vr, new UIntPtr((ulong)vr.Length), value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        #endregion

        #region IDisposable Support
        private bool disposedValue = false; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    // Free managed resources
                }
                // Free unmanaged resources
                FreeInstance();
                // Avoid disposing multiple times
                disposedValue = true;
            }
        }

        ~FmuInstance()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            Dispose(false);
        }

        // This code added to correctly implement the disposable pattern.
        public void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        #endregion
    }
}
