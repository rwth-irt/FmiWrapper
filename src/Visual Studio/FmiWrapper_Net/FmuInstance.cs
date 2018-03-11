using System;
using System.Collections.Generic;
using System.Text;
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
        public event FmiWrapperImport.LogCallback Log;
        private void OnLog(string instanceName, int status, string category, string message) =>
            Log?.Invoke(instanceName, status, category, message);
        // StepFinished event to wrap callback
        public event FmiWrapperImport.StepFinishedCallback StepFinished;
        private void OnStepFinished(int status) =>
            StepFinished?.Invoke(status);

        // Expose the wraped model functions

        /// <summary>
        /// The path to the binary of the fmu.
        /// </summary>
        /// <param name="fileName"></param>
        public FmuInstance(string fileName)
        {
            this.fileName = fileName;
        }

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
        public void Instantiate(string instanceName, int fmuType, string guid, string resourceLocation, bool visible, bool loggingOn)
        {
            // Only instantiate if no instance is stored
            if (wrapper == IntPtr.Zero)
            {
                // Load the model instance
                wrapper = FmiWrapperImport.Fmi2Instantiate(fileName, new FmiWrapperImport.LogCallback(OnLog), new FmiWrapperImport.StepFinishedCallback(OnStepFinished),
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
                FmiWrapperImport.Fmi2FreeInstance(wrapper);
            wrapper = IntPtr.Zero;
        }

        public string GetTypesPlatform()
        {
            if (wrapper != IntPtr.Zero)
                return Marshal.PtrToStringAnsi(FmiWrapperImport.Fmi2GetTypesPlatform(wrapper));
            else
                return "";
        }

        public string GetVersion()
        {
            if (wrapper != IntPtr.Zero)
                return Marshal.PtrToStringAnsi(FmiWrapperImport.Fmi2GetVersion(wrapper));
            else
                return "";
        }

        public Fmi2Status SetupExperiment(bool toleranceDefined, double tolerance, double startTime, bool stopTimeDefined, double stopTime)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiWrapperImport.Fmi2SetupExperiment(wrapper, toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime);
            else
                return Fmi2Status.fmi2Fatal;
        }


        public Fmi2Status EnterInitializationMode()
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiWrapperImport.Fmi2EnterInitializationMode(wrapper);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status ExitInitializationMode()
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiWrapperImport.Fmi2ExitInitializationMode(wrapper);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status Terminate()
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiWrapperImport.Fmi2Terminate(wrapper);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status Reset()
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiWrapperImport.Fmi2Reset(wrapper);
            else
                return Fmi2Status.fmi2Fatal;

        }

        #region IDisposable Support
        private bool disposedValue = false; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    // TODO: dispose managed state (managed objects).
                }

                // TODO: free unmanaged resources (unmanaged objects) and override a finalizer below.
                // TODO: set large fields to null.
                FreeInstance();

                disposedValue = true;
            }
        }

        // TODO: override a finalizer only if Dispose(bool disposing) above has code to free unmanaged resources.
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
            // TODO: uncomment the following line if the finalizer is overridden above.
            GC.SuppressFinalize(this);
        }
        #endregion
    }
}
