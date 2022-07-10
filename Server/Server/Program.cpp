#include "stdafx.h"

#include <networkengine\NetworkEngine.h>
#include <networkengine\TcpAcceptorSocket.h>
#include <networkengine\TcpListener.h>
#include <networkengine\PacketHandler.h>
#include <networkengine\Logger.h>
#include <networkengine\PathManager.h>
#include <networkengine\AcceptorFactory.h>
#include <networkengine\GeneratedPacketFactory.h>

using namespace std;
using namespace pkt;
Logger* logger;

class MyPacketHandler : public PacketHandler<MyPacketHandler>
{
    template<typename T>
    using Ptr = std::shared_ptr<T>;
private:
    ISender* mSender;
    Logger* mLogger;

private:
    template<typename Key, typename Value>
    string toString(const std::unordered_map<Key, Value>& container)
    {
        std::stringstream ss;
        ss << "{ ";
        int i = 0; int last = (int)container.size();
        for (const auto& pair : container)
        {
            ss << pair.first << " : " << pair.second; if (++i != last) ss << ", ";
        }
        ss << " }";

        return ss.str();
    }

public:
    MyPacketHandler(ISender* sender)
        :
        mSender(sender),
        mLogger(Logger::getInstance())
    {
        registerHandler(&MyPacketHandler::onEchoMessage);
        registerHandler(&MyPacketHandler::onMyPacket);
        registerHandler(&MyPacketHandler::onMyPacketListStr);
        registerHandler(&MyPacketHandler::onMyPacketSetStr);
        registerHandler(&MyPacketHandler::onMyPacketMapIntInt);
        registerHandler(&MyPacketHandler::onMyPacketMapIntString);
        registerHandler(&MyPacketHandler::onMyPacketMapStringInt);
        registerHandler(&MyPacketHandler::onMyPacketMapStringString);
    }

    void onEchoMessage(const Ptr<pkt::EchoMessage>& message)
    {
        LOG_INFO(mLogger, "Echo : %s", message->getMessage().c_str());
    }

    void onMyPacket(const Ptr<pkt::MyPacket>& message)
    {
        LOG_INFO(mLogger, "Echo : %s, %d, %f, %lf", message->getName().c_str(), message->getInteger(), message->getFloat(), message->getDouble());
    }

    void onMyPacketListStr(const Ptr<pkt::MyListString>& message)
    {
        const auto& values = message->getNames();
        string str = Format::join(values.begin(), values.end(), ",");
        LOG_INFO(mLogger, "List : %s", str.c_str());
    }

    void onMyPacketSetStr(const Ptr<pkt::MySetString>& message)
    {
        const auto& values = message->getNames();
        string str = Format::join(values.begin(), values.end(), ",");
        LOG_INFO(mLogger, "Set : %s", str.c_str());
    }

    void onMyPacketMapIntInt(const Ptr<pkt::MyMapIntInt>& message)
    {
        const auto& container = message->getContainer();
        string str = toString(container);
        LOG_INFO(mLogger, "Map<int,int> : %s", str.c_str());
    }

    void onMyPacketMapIntString(const Ptr<pkt::MyMapIntString>& message)
    {
        const auto& container = message->getContainer();
        string str = toString(container);
        LOG_INFO(mLogger, "Map<int,string> : %s", str.c_str());
    }

    void onMyPacketMapStringInt(const Ptr<pkt::MyMapStringInt>& message)
    {
        const auto& container = message->getContainer();
        string str = toString(container);
        LOG_INFO(mLogger, "Map<string,int> : %s", str.c_str());
    }

    void onMyPacketMapStringString(const Ptr<pkt::MyMapStringString>& message)
    {
        const auto& container = message->getContainer();
        string str = toString(container);
        LOG_INFO(mLogger, "Map<string,string> : %s", str.c_str());
    }
};

int main(int argc, char ** argv)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    if (argc < 2)
    {
        cout << "arguments invalid." << endl;
        cout << "usage: program <port>" << endl;;
        return 0;
    }

    logger = Logger::getInstance();
    logger->setConsoleLog(true);
    logger->setProgramName("Server");

    USHORT port = 0;
    try 
    {
        port = (USHORT)std::strtoul(argv[1], NULL, 0);
    }
    catch (std::exception e)
    {
        cout << "arguments invalid." << endl;
        cout << "usage: program <port>" << endl;;
        return 0;
    }

    if (!NetworkEngine::getInstance()->initialize())
        return 0;

    PacketManager::getInstance()->registerImpl(new GeneratedPacketFactory());

    AcceptorFactory factory(
        [](SOCKET socket) { return new TcpAcceptorSocket(socket); },
        [](ISender* sender) { return new MyPacketHandler(sender); }
    );

    std::unique_ptr<TcpListener> server;
    server.reset(new TcpListener(factory));

    if (!server->initialize())
    {
        LOG_ERROR(logger, "server initialize failed");
    }

    server->start(port);
    server->shutdown();

    return 0;
}