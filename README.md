# WindowsCppTcpServer

## Project Description
### TCP/IP Network using Winsock
- Wrapping Socket according to Connector, Acceptor and Listener
- On server side, network events are handled by IOCP
- On client side, network events are Handled by Select
- Very simple packet transmit on serialized bytes

### PacketGenerator
- supports literal type field (bool, char, short, int, long long, float, double, unsigned types...)
- supports vector, set, map container

### Etc
- logger (check logs out from console or .log file in executable file path)
- boost support (optional, needs to add static-library)
- client-server examples

### Batch files
- CopyHeader.bat : copy header files from NetworkEngine project
- CopyGenerated.bat : copy generated header files from PacketGenerator project

## Usage
### Make Custom Packets with PacketGenerator
1. Check __PacketStruct.cs__ file to define custom Packet.
2. Define __NewPacket__ class by derive from 'Packet' class.
3. Execute or let it be executed or by after build event of PacketGenerator Project.
  - This project will generate following three files
  - GeneratedPacketStruct.h  : definition of packet struct using template
  - GeneratedPacketType.h    : defintiion of packet class for actual use
  - GeneratedPacketFactory.h : definition of packet handler and factory
4. Include 'GeneratedPacketFactory.h' in server or client code.

### Server Code
```cpp
// include packet definition that generated 
#include <networkengine\GeneratedPacketFactory.h>

int main()
{
    // 1. initialize network engine and register custom packet classes
    NetworkEngine::getInstance()->initialize();
    PacketManager::getInstance()->registerImpl(new GeneratedPacketFactory());

    // 2. define acceptor factory (acceptor socket and packet handler)
    AcceptorFactory factory(
    [](SOCKET socket) { return new TcpAcceptorSocket(socket); },
    [](ISender* sender) { return new MyPacketHandler(sender); }
    );

    // 3. create listener
    std::unique_ptr<TcpListener> server;
    server.reset(new TcpListener(factory));

    if (!server->initialize())
    {
        LOG_ERROR(logger, "server initialize failed");
    }

    // 4. start with port and wait until server program shutdown
    server->start(port);
    server->shutdown();

    return 0;
}
```

### Client Code
```cpp
// include packet definition that generated 
#include <networkengine\GeneratedPacketFactory.h>


int main()
{
    // 1. initialize network engine
    NetworkEngine::getInstance()->initialize();

    // 2. register custom packet classes
    PacketManager::getInstance()->registerImpl(new GeneratedPacketFactory());

    // 3. create connector
    ConnectorFactory factory([]() {return new TcpConnectorSocket(); },
                     [](ISender* sender) { return new MyPacketHandler(sender); });

    std::unique_ptr<TcpConnector> connector(new TcpConnector(factory));

    Logger* logger = Logger::getInstance();
    if (!connector->initialize())
    {
        LOG_ERROR(logger, "initialize connector failed");
        return 0;
    }

    // 4. connect
    EndPoint endPoint(ipAddress.c_str(), port);
    connector->connect(endPoint);

    // 5. packet events
    int i = 0;
    while (i++ < 10)
    {
        // send packets
        if (!connector->writeMessage(pkts))
        {
            LOG_ERROR(logger, "send message failed");
        }
        else
        {
            cout << "message sent ! : " << msg.c_str() << endl;
        }

        // recv packet handling
        connector->handleNetworkEvent();

        std::this_thread::sleep_for(10ms);
    }

    return 0;
}
```