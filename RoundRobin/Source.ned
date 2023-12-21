simple Source
{
    parameters:
        volatile double avgInterArrivalTime @unit(s);
        volatile double meanofMsgSize;
        @display("i=block/source");
    gates:
        output out;
}