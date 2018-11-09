using System;
using FmiWrapper_Net;
using System.IO;
using System.IO.Compression;
using System.Xml;

namespace FmiWrapperConsole
{
    /// <summary>
    /// Simulates a test fmu (test_sf.fmu).
    /// Make sure to compile for x64 so the right DLL is beeing loaded.
    /// </summary>
    class Program
    {
        //private const string FMU = "SimplePendulum.fmu";
        private const string FMU = "test_sf.fmu";
        private const double END_TIME = 0.1;
        private const double STEP_SIZE = 0.01;

        private static (string modelName, string guid) LoadFmu(string fmuPath)
        {
            using (var fmuStream = File.OpenRead(fmuPath))
            {
                var fmuArchive = new ZipArchive(fmuStream, ZipArchiveMode.Read);
                var modelDescriptionEntry = fmuArchive.GetEntry("modelDescription.xml");
                using (var modelDescription = modelDescriptionEntry.Open())
                {
                    // Read the xml
                    var doc = new XmlDocument();
                    doc.Load(modelDescription);
                    var modelDescriptionNode = doc.SelectSingleNode("/fmiModelDescription");
                    var name = modelDescriptionNode.Attributes["modelName"].InnerText;
                    var guid = modelDescriptionNode.Attributes["guid"].InnerText;
                    // Extract the binary
                    var binaryEntry = fmuArchive.GetEntry("binaries/win64/" + name + ".dll");
                    using (var binary = binaryEntry.Open())
                    {
                        using (var target = File.Create(name + ".dll"))
                        {
                            binary.CopyTo(target);
                        }
                    }
                    return (name, guid);
                }
            }
        }

        private static void SetValues(FmuInstance fmu)
        {
            uint[] setRealVr = { 1342177280 };
            double[] setRealValues = { 42 };
            fmu.SetReal(setRealVr, setRealValues);
            uint[] setIntegerVr = { 1442840578 };
            int[] setIntegerValues = { 42 };
            fmu.SetInteger(setIntegerVr, setIntegerValues);
            uint[] setBooleanVr = { 1476395009 };
            bool[] setBooleanValues = { true };
            fmu.SetBoolean(setBooleanVr, setBooleanValues);
            uint[] setStringVr = { 1, 2, 3 };
            string[] setStringValues = { "1", "2", "3" };
            fmu.SetString(setStringVr, setStringValues);
        }

        private static void GetValues(FmuInstance fmu)
        {
            uint[] getRealVr = { 1073741824 };
            var getRealValues = new double[getRealVr.Length];
            fmu.GetReal(getRealVr, getRealValues);
            Console.WriteLine("Real values: " + String.Join("; ", getRealValues));
            uint[] getIntegerVr = { 1174405121 };
            var getIntegerValues = new int[getIntegerVr.Length];
            fmu.GetInteger(getIntegerVr, getIntegerValues);
            Console.WriteLine("Integer values: " + String.Join("; ", getIntegerValues));
            uint[] getBooleanVr = { 1207959554 };
            var getBooleanValues = new bool[getBooleanVr.Length];
            fmu.GetBoolean(getBooleanVr, getBooleanValues);
            Console.WriteLine("Boolean values: " + String.Join("; ", getBooleanValues));
            uint[] getStringVr = { 1, 2, 3 };
            string[] getStringValues = new string[getStringVr.Length];
            fmu.GetString(getStringVr, getStringValues);
            Console.WriteLine("String values: " + String.Join("; ", getStringValues));
        }

        private static void Simulate(FmuInstance fmu)
        {
            for (double time = 0; time < END_TIME; time += STEP_SIZE)
            {
                Console.WriteLine("\nGetValues, current time: " + time);
                GetValues(fmu);
                Console.WriteLine("DoStep, step size: " + STEP_SIZE);
                fmu.DoStep(time, STEP_SIZE, true);
            }
        }

        static void Main(string[] args)
        {
            // Load model description and extract binary
            (string modelName, string guid) = LoadFmu(FMU);
            // Create the instancr
            using (var fmu = new FmuInstance(modelName + ".dll"))
            {
                // Setup
                fmu.Log += Fmu_Log;
                fmu.StepFinished += Fmu_StepFinished;
                fmu.Instantiate(modelName + "_Instance", Fmi2Type.fmi2CoSimulation, guid, "", false, true);
                Console.WriteLine("Types platform: " + fmu.GetTypesPlatform());
                Console.WriteLine("Version: " + fmu.GetVersion());
                fmu.SetupExperiment(false, 0, 0, true,  END_TIME);
                fmu.EnterInitializationMode();
                fmu.ExitInitializationMode();

                // Simulate
                SetValues(fmu);
                Simulate(fmu);
                // Reset crashes the Matlab FMU
                fmu.FreeInstance();
                fmu.Instantiate(modelName + "_Instance", Fmi2Type.fmi2CoSimulation, guid, "", false, true);
                //fmu.Reset();
                fmu.SetupExperiment(false, 0, 0, true, END_TIME);
                fmu.EnterInitializationMode();
                fmu.ExitInitializationMode();


                // Simulate again
                SetValues(fmu);
                Simulate(fmu);
                // Terminate the fmu
                fmu.Terminate();
            }
            Console.WriteLine("Press enter to exit");
            Console.ReadLine();
        }

        private static void Fmu_Log(string instanceName, Fmi2Status status, string category, string message)
        {
            Console.WriteLine("Instance name: " + instanceName + ", status: " + status.ToString("g") + ", category: " + category + ", message: " + message);
        }

        private static void Fmu_StepFinished(Fmi2Status status)
        {
            Console.WriteLine("Step finished " + status.ToString("g"));
        }
    }
}
