using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace FmiWrapper_Net
{
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct Fmi2EventInfo
    {
        [MarshalAs(UnmanagedType.Bool)] bool newDiscreteStatesNeeded;
        [MarshalAs(UnmanagedType.Bool)] bool terminateSimulation;
        [MarshalAs(UnmanagedType.Bool)] bool nominalsOfContinuousStatesChanged;
        [MarshalAs(UnmanagedType.Bool)] bool valuesOfContinuousStatesChanged;
        [MarshalAs(UnmanagedType.Bool)] bool nextEventTimeDefined;
        double nextEventTime;
    }
}
