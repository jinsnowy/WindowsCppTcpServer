#include "stdafx.h"

#include <networkengine\NetworkEngine.h>
#include <networkengine\TcpConnector.h>
#include <networkengine\PacketHandler.h>
#include <networkengine\TcpConnectorSocket.h>
#include <networkengine\Logger.h>
#include <networkengine\DefaultPacketType.h>
#include <networkengine\ConnectorFactory.h>
#include <networkengine\GeneratedPacketFactory.h>

using namespace std;

class MyPacketHandler : public PacketHandler<MyPacketHandler>
{
private:
    ISender* mSender;

public:
    MyPacketHandler(ISender* sender) : mSender(sender)
    {
    }
};

Logger* logger;
std::string ipAddress;
USHORT port = 0;
std::unique_ptr<TcpConnector> connector;

static bool connect()
{
    ConnectorFactory factory([]() {return new TcpConnectorSocket(); },
                     [](ISender* sender) { return new MyPacketHandler(sender); });

    connector.reset(new TcpConnector(factory));

    Logger* logger = Logger::getInstance();
    if (!connector->initialize())
    {
        LOG_ERROR(logger, "initialize connector failed");
        return 0;
    }

    EndPoint endPoint(ipAddress.c_str(), port);

    return connector->connect(endPoint);
}

using namespace chrono_literals;
using namespace pkt;

static void packets()
{
    LOG_INFO(logger, "send starts");
    using PacketPtr = std::shared_ptr<Packet>;

    int i = 0;
    while (i++ < 10)
    {
        string msg = Format::format("hello world %d", i);
        
        std::unordered_set<int> s{ 1,2,3,4,5 };

        vector<PacketPtr> pkts;
        auto pktBasic = make_shared<MyPacket>();
        pktBasic->setFloat(1.0f * (float)i);
        pktBasic->setInteger(25 * i);
        pktBasic->setName(msg);
        pktBasic->setDouble(1.25 * (double)i);
        pkts.push_back(pktBasic);

        vector<string> list_str = { "abc", "def", "ghk" };
        auto pktListStr = make_shared<MyListString>();
        pktListStr->setNames(list_str);
        pkts.push_back(pktListStr);

        unordered_set<string> set_str = { "abc", "def", "ghk" };
        auto pktSetStr = make_shared<MySetString>();
        pktSetStr->setNames(set_str);
        pkts.push_back(pktSetStr);

        unordered_map<int, int> map_int_int = { {1, 10}, {2, 20}, {3, 30} };
        auto pktMapIntInt = make_shared<MyMapIntInt>();
        pktMapIntInt->setContainer(map_int_int);
        pkts.push_back(pktMapIntInt);

        unordered_map<int, string> map_int_str = { {1, "one"}, {2, "two"}, {3, "three"} };
        auto pktMapIntStr = make_shared<MyMapIntString>();
        pktMapIntStr->setContainer(map_int_str);
        pkts.push_back(pktMapIntStr);

        unordered_map<string, int> map_str_int = { {"one", 1}, {"two", 2}, {"three", 3} };
        auto pktMapStrInt = make_shared<MyMapStringInt>();
        pktMapStrInt->setContainer(map_str_int);
        pkts.push_back(pktMapStrInt);

        unordered_map<string, string> map_str_str = { {"one", "1"}, {"two", "2"}, {"three", "3"} };
        auto pktMapStrStr = make_shared<MyMapStringString>();
        pktMapStrStr->setContainer(map_str_str);
        pkts.push_back(pktMapStrStr);

        if (!connector->writeMessage(pkts))
        {
            LOG_ERROR(logger, "send message failed");
        }
        else
        {
            cout << "message sent ! : " << msg.c_str() << endl;
        }

        connector->handleNetworkEvent();

        std::this_thread::sleep_for(10ms);
    }

    for (int i = 0; i < 100; ++i)
    {
        std::this_thread::sleep_for(10s);
    }
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        cout << "arguments invalid." << endl;
        cout << "usage: program <ip> <port>" << endl;;
        return 0;
    }

    logger = Logger::getInstance();
    logger->setConsoleLog(true);
    logger->setProgramName("Client");

    try
    {
        ipAddress = argv[1];
        port = (USHORT)std::strtoul(argv[2], NULL, 0);
    }
    catch (std::exception e)
    {
        cout << "arguments invalid." << endl;
        cout << "usage: program <ip> <port>" << endl;;
        return 0;
    }

    NetworkEngine::getInstance()->initialize();
    PacketManager::getInstance()->registerImpl(new GeneratedPacketFactory());

    if (!connect())
    {
        return 0;
    }
    
    try
    {
        packets();
    }
    catch (std::exception e)
    {
        LOG_ERROR(logger, "exception = %s", e.what());
    }

    return 0;
}