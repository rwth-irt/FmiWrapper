namespace FmiWrapper_Net
{
    public enum Fmi2Status
    {
        fmi2OK,
        fmi2Warning,
        fmi2Discard,
        fmi2Error,
        fmi2Fatal,
        fmi2Pending
    };

    public enum Fmi2Type
    {
        fmi2ModelExchange,
        fmi2CoSimulation
    };

    public enum Fmi2StatusKind
    {
        fmi2DoStepStatus,
        fmi2PendingStatus,
        fmi2LastSuccessfulTime,
        fmi2Terminated
    };
}
