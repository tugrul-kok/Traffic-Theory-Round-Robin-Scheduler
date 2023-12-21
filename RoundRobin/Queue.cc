#include <omnetpp.h>

using namespace omnetpp;


class Queue : public cSimpleModule
{
protected:
    cMessage *msgInServer;
    cMessage *endOfServiceMsg;

    cQueue queue;

    simsignal_t qlenSignal;
    simsignal_t busySignal;
    simsignal_t queueingTimeSignal;
    simsignal_t responseTimeSignal;

    double avgServiceTime;
    bool isRoundRobin;

    bool serverBusy;

public:
    Queue();
    virtual ~Queue();
    simtime_t serviceTime;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void startPacketService(cMessage *msg);
    void putPacketInQueue(cMessage *msg);
};

Define_Module(Queue);


Queue::Queue()
{
    msgInServer = endOfServiceMsg = nullptr;
}

Queue::~Queue()
{
    delete msgInServer;
    cancelAndDelete(endOfServiceMsg);
}

void Queue::initialize()
{


    endOfServiceMsg = new cMessage("end-service");
    queue.setName("queue");
    serverBusy = false;

    //signal registering
    qlenSignal = registerSignal("qlen");
    busySignal = registerSignal("busy");
    queueingTimeSignal = registerSignal("queueingTime");
    responseTimeSignal = registerSignal("responseTime");

    //initial messages
    emit(qlenSignal, queue.getLength());
    emit(busySignal, serverBusy);

    //get avgServiceTime parameter
    avgServiceTime = par("avgServiceTime").doubleValue();
    isRoundRobin = par("isRoundRobin").boolValue();
}

void Queue::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    { //Packet in server has been processed

        if( msgInServer->par("MsgSize").longValue() == 0){
        //log service completion
        EV << "Completed service of " << msgInServer->getName() << endl;
        EV << "The MsgSize is" << msgInServer->par("MsgSize").longValue()<< endl;
        //Send processed packet to sink
        send(msgInServer, "out");

        //emit response time signal
        emit(responseTimeSignal, simTime() - msgInServer->getTimestamp());

        } else if(isRoundRobin)
        {
            if (serverBusy)
            {
                putPacketInQueue(msgInServer);

            }else
            { //server idle, start service right away
                //Put the message in server and start service
                startPacketService(msgInServer);

                //server is now busy
                serverBusy=true;
                emit(busySignal, serverBusy);
            }
        }


        //start next packet processing if queue not empty
        if (!queue.isEmpty()) {
            //Put the next message from the queue inside the server
            msgInServer = (cMessage *)queue.pop();

            //Emit queue len and queuing time for this packet
            emit(qlenSignal, queue.getLength());
            emit(queueingTimeSignal, simTime() - msgInServer->getTimestamp());

            //start service
            startPacketService(msg);
        } else {
            //server is not busy anymore
            msgInServer = nullptr;
            serverBusy = false;
            emit(busySignal, serverBusy);

            //log idle server
            EV << "Empty queue, server goes IDLE" <<endl;
        }

    }
    else { //packet from source has arrived

        //Setting arrival timestamp as msg field
        msg->setTimestamp();

        if (serverBusy) {
            putPacketInQueue(msg);
        }
        else { //server idle, start service right away
            //Put the message in server and start service
            msgInServer = msg;
            startPacketService(msg);

            //server is now busy
            serverBusy=true;
            emit(busySignal, serverBusy);

            //queueing time was ZERO
            emit(queueingTimeSignal, SIMTIME_ZERO);
        }
    }
}

void Queue::startPacketService(cMessage *msq)
{

    //generate service time and schedule completion accordingly
    if(isRoundRobin)
    {
        serviceTime = exponential(avgServiceTime);
        EV << "Starting service of " << msgInServer->getName() << endl;
        EV << "MsgSize before processing: " <<     msgInServer->par("MsgSize").longValue() << endl;
        msgInServer->par("MsgSize") = msgInServer->par("MsgSize").longValue()- 1;
        EV << "MsgSize after processing: " <<     msgInServer->par("MsgSize").longValue() << endl;
    }
    else if(!isRoundRobin)
    {
         serviceTime = msgInServer->par("MsgSize").longValue()*exponential(avgServiceTime);
         EV << "Starting service of " << msgInServer->getName() << endl;
         EV << "MsgSize before processing: " <<     msgInServer->par("MsgSize").longValue() << endl;
         msgInServer->par("MsgSize") = 0;
         EV << "MsgSize after processing: " <<     msgInServer->par("MsgSize").longValue() << endl;
     }

    scheduleAt(simTime()+serviceTime, endOfServiceMsg);
    //log service start
}

void Queue::putPacketInQueue(cMessage *msg)
{
    queue.insert(msg);
    emit(qlenSignal, queue.getLength());

    //log new message in queue
    EV << msg->getName() << " enters queue"<< endl;
}
