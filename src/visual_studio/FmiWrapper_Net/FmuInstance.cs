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
        private readonly string fileName;
        private IntPtr wrapper;

        // Log event to wrap callback
        private FmiFunctions.LogCallback logCallback;
        public event FmiFunctions.LogCallback Log;
        private void OnLog(string instanceName, Fmi2Status status, string category, string message) =>
            Log?.Invoke(instanceName, status, category, message);
        // StepFinished event to wrap callback
        private FmiFunctions.StepFinishedCallback stepFinishedCallback;
        public event FmiFunctions.StepFinishedCallback StepFinished;
        private void OnStepFinished(Fmi2Status status) =>
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
                logCallback = new FmiFunctions.LogCallback(OnLog);
                stepFinishedCallback = new FmiFunctions.StepFinishedCallback(OnStepFinished);
                wrapper = FmiFunctions.Instantiate(fileName, logCallback, stepFinishedCallback,
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

        public Fmi2Status GetReal(uint[] vr, double[] value)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetReal(wrapper, vr, new UIntPtr((uint)vr.Length), value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status GetInteger(uint[] vr, int[] value)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetInteger(wrapper, vr, new UIntPtr((uint)vr.Length), value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status GetBoolean(uint[] vr, bool[] value)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetBoolean(wrapper, vr, new UIntPtr((uint)vr.Length), value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status GetString(uint[] vr, string[] value)
        {
            if (wrapper != IntPtr.Zero)
            {
                var valuePtrs = new IntPtr[vr.Length];
                var status = (Fmi2Status)FmiFunctions.GetString(wrapper, vr, new UIntPtr((uint)vr.Length), valuePtrs);
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
                return (Fmi2Status)FmiFunctions.SetReal(wrapper, vr, new UIntPtr((uint)vr.Length), value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status SetInteger(uint[] vr, int[] value)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.SetInteger(wrapper, vr, new UIntPtr((uint)vr.Length), value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status SetBoolean(uint[] vr, bool[] value)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.SetBoolean(wrapper, vr, new UIntPtr((uint)vr.Length), value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status SetString(uint[] vr, string[] value)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.SetString(wrapper, vr, new UIntPtr((uint)vr.Length), value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        #endregion

        #region Types for Functions for FMI2 for Model Exchange

        #region Enter and exit the different modes

        public Fmi2Status EnterEventMode()
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.EnterEventMode(wrapper);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status NewDiscreteStates(out Fmi2EventInfo eventInfo)
        {
            eventInfo = new Fmi2EventInfo();
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.NewDiscreteStates(wrapper, ref eventInfo);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status EnterContinuousTimeMode()
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.EnterContinuousTimeMode(wrapper);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status CompletedIntegratorStep(bool noSetFmuStatePriorToCurrentPoint, out bool enterEventMode, out bool terminateSimulation)
        {
            enterEventMode = false;
            terminateSimulation = false;
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.CompletedIntegratorStep(wrapper, noSetFmuStatePriorToCurrentPoint, ref enterEventMode, ref terminateSimulation);
            else
                return Fmi2Status.fmi2Fatal;
        }

        #endregion

        #region Providing independent variables and re-initialization of caching

        public Fmi2Status SetTime(double time)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.SetTime(wrapper, time);
            else
                return Fmi2Status.fmi2Fatal;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="x">Array with length of the number of states.</param>
        /// <returns></returns>
        public Fmi2Status SetContinuousStates(double[] x)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetEventIndicators(wrapper, x, new UIntPtr((uint)x.Length));
            else
                return Fmi2Status.fmi2Fatal;
        }

        #endregion

        #region Evaluation of the model equations

        /// <summary>
        /// 
        /// </summary>
        /// <param name="derivatives">Array with length of the number of states.</param>
        /// <returns></returns>
        public Fmi2Status GetDerivatives(double[] derivatives)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetDerivatives(wrapper, derivatives, new UIntPtr((uint)derivatives.Length));
            else
                return Fmi2Status.fmi2Fatal;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="eventIndicators">Array with length of the number of event indicators.</param>
        /// <returns></returns>
        public Fmi2Status GetEventIndicators(double[] eventIndicators)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetEventIndicators(wrapper, eventIndicators, new UIntPtr((uint)eventIndicators.Length));
            else
                return Fmi2Status.fmi2Fatal;
        }


        /// <summary>
        /// 
        /// </summary>
        /// <param name="x">Array with length of the number of states.</param>
        /// <returns></returns>
        public Fmi2Status GetContinuousStates(double[] x)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetContinuousStates(wrapper, x, new UIntPtr((uint)x.Length));
            else
                return Fmi2Status.fmi2Fatal;

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="x_nominal">Array with length of the number of states.</param>
        /// <returns></returns>
        public Fmi2Status GetNominalsOfContinuousStates(double[] x_nominal)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetNominalsOfContinuousStates(wrapper, x_nominal, new UIntPtr((uint)x_nominal.Length));
            else
                return Fmi2Status.fmi2Fatal;
        }

        #endregion



        #endregion

        #region Types for Functions for FMI2 for Co-Simulation

        #region Simulating the slave

        public Fmi2Status SetRealInputDerivatives(uint[] vr, int[] order, double[] value)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.SetRealInputDerivatives(wrapper, vr, new UIntPtr((uint)vr.Length), order, value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status GetRealOutputDerivatives(uint[] vr, int[] order, double[] value)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetRealOutputDerivatives(wrapper, vr, new UIntPtr((uint)vr.Length), order, value);
            else
                return Fmi2Status.fmi2Fatal;
        }


        public Fmi2Status DoStep(double currentCommunicationPoint, double communicationStepSize, bool noSetFmuStatePriorToCurrentPoint)
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.DoStep(wrapper, currentCommunicationPoint, communicationStepSize, noSetFmuStatePriorToCurrentPoint);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status CancelStep()
        {
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.CancelStep(wrapper);
            else
                return Fmi2Status.fmi2Fatal;
        }

        #endregion

        #region Inquire slave status

        public Fmi2Status GetStatus(Fmi2StatusKind statusKind, out Fmi2Status value)
        {
            value = Fmi2Status.fmi2Error;
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetStatus(wrapper, statusKind, ref value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status GetRealStatus(Fmi2StatusKind statusKind, out double value)
        {
            value = 0;
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetRealStatus(wrapper, statusKind, ref value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status GetIntegerStatus(Fmi2StatusKind statusKind, out int value)
        {
            value = 0;
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetIntegerStatus(wrapper, statusKind, ref value);
            else
                return Fmi2Status.fmi2Fatal;
        }

        public Fmi2Status GetBooleanStatus(Fmi2StatusKind statusKind, out bool value)
        {
            value = false;
            if (wrapper != IntPtr.Zero)
                return (Fmi2Status)FmiFunctions.GetBooleanStatus(wrapper, statusKind, ref value);
            else
                return Fmi2Status.fmi2Fatal;

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="statusKind"></param>
        /// <param name="value">Evaluteate with Marshal.PtrToStringAnsi.</param>
        /// <returns></returns>
        public Fmi2Status GetStringStatus(Fmi2StatusKind statusKind, out string value)
        {
            value = "";
            if (wrapper != IntPtr.Zero)
            {
                var valuePtr = new IntPtr();
                var result = (Fmi2Status)FmiFunctions.GetStringStatus(wrapper, statusKind, valuePtr);
                value = Marshal.PtrToStringAnsi(valuePtr);
                return result;
            }
            else
                return Fmi2Status.fmi2Fatal;
        }

        #endregion

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
