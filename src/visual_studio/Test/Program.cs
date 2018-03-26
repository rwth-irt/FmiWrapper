using System;
using FmiWrapper_Net;

namespace TestFmiWrapper
{
    class Program
    {
        static void Main(string[] args)
        {
            FmuInstance fmu = new FmuInstance("SimplePendulum", "TestInstance", 1, "{d469a761-6eeb-4434-be44-77019e248cbe}", "", false, true);
            fmu.Log += Fmu_Log;
            fmu.StepFinished += Fmu_StepFinished;
            Console.WriteLine("Types platform: " + fmu.GetTypesPlatform());
            Console.WriteLine("Version: " + fmu.GetVersion());
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
