using System;
using FmiWrapper_Net;
using System.Linq;

namespace FmiWrapperConsole
{
    class Program
    {
        static void Main(string[] args)
        {
            using (var fmu = new FmuInstance("SimplePendulum.dll"))
            {
                // Setup
                fmu.Log += Fmu_Log;
                fmu.StepFinished += Fmu_StepFinished;
                fmu.Instantiate("TestInstance", Fmi2Type.fmi2CoSimulation, "{d469a761-6eeb-4434-be44-77019e248cbe}", "", false, true);
                Console.WriteLine("Types platform: " + fmu.GetTypesPlatform());
                Console.WriteLine("Version: " + fmu.GetVersion());
                fmu.SetupExperiment(false, 0, 0, false, int.MaxValue);
                fmu.EnterInitializationMode();
                fmu.ExitInitializationMode();
                // Get variables
                uint[] realVr = { 16777216, 905969664 };
                var realValues = new double[realVr.Length];
                fmu.GetReal(realVr, realValues);
                Console.WriteLine("Real values: " + String.Join("; ", realValues.Select(p => p.ToString()).ToArray()));
                uint[] otherVr = { 42, 666, 52062 };
                var intValues = new int[otherVr.Length];
                fmu.GetInteger(otherVr, intValues);
                Console.WriteLine("Integer values: " + String.Join("; ", intValues));
                var boolValues = new bool[otherVr.Length];
                fmu.GetBoolean(otherVr, boolValues);
                Console.WriteLine("Boolean values: " + String.Join("; ", boolValues));
                var stringValues = new string[otherVr.Length];
                fmu.GetString(otherVr, stringValues);
                Console.WriteLine("String values: " + String.Join("; ", stringValues));
                // Set variables
                uint[] setRealVr = { 16777216, 16777217 };
                double[] setRealValues = { 19.81, 0.5 };
                fmu.SetReal(setRealVr, setRealValues);
                int[] setIntegerValues = { 1, 2, 3 };
                fmu.SetInteger(otherVr, setIntegerValues);
                bool[] setBoolValues = { true, false, true };
                fmu.SetBoolean(setRealVr, setBoolValues);
                string[] setStringValues = { "1", "2", "3" };
                fmu.SetString(otherVr, setStringValues);
                // Compute a simulation step
                fmu.DoStep(0.0, 0.5, true);
                // Get new values
                fmu.GetReal(realVr, realValues);
                Console.WriteLine("Real values: " + String.Join("; ", realValues.Select(p => p.ToString()).ToArray()));
                fmu.GetInteger(otherVr, intValues);
                Console.WriteLine("Integer values: " + String.Join("; ", intValues));
                fmu.GetBoolean(otherVr, boolValues);
                Console.WriteLine("Boolean values: " + String.Join("; ", boolValues));
                fmu.GetString(otherVr, stringValues);
                Console.WriteLine("String values: " + String.Join("; ", stringValues));
                // Terminate the fmu
                fmu.Reset();
                fmu.Terminate();
            }
            Console.WriteLine("Press enter to exit");
            Console.ReadLine();
        }

        private static void Fmu_Log(string instanceName, int status, string category, string message)
        {
            Console.WriteLine("Instance name: " + instanceName + ", status: " + status + ", category: " + category + ", message: " + message);
        }

        private static void Fmu_StepFinished(int status)
        {
            Console.WriteLine("Step finished " + status);
        }
    }
}
