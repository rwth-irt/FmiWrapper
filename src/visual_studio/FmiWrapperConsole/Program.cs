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
                fmu.Log += Fmu_Log;
                fmu.StepFinished += Fmu_StepFinished;
                fmu.Instantiate("TestInstance", Fmi2Type.fmi2CoSimulation, "{d469a761-6eeb-4434-be44-77019e248cbe}", "", false, true);
                Console.WriteLine("Types platform: " + fmu.GetTypesPlatform());
                Console.WriteLine("Version: " + fmu.GetVersion());
                fmu.SetupExperiment(false, 0, 0, false, int.MaxValue);
                fmu.EnterInitializationMode();
                fmu.ExitInitializationMode();
                uint[] realVr = { 16777216, 905969664 };
                fmu.GetReal(realVr, out double[] realValues);
                Console.WriteLine("Real values: " + String.Join("; ", realValues.Select(p => p.ToString()).ToArray()));
                uint[] stringVr = { 42, 666 };
                Console.WriteLine("Press enter to continue");
                fmu.GetString(stringVr, out string[] stringValues);
                Console.WriteLine("String values: " + String.Join("; ", stringValues));
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
