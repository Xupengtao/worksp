#include "../../../Tools/ZmqSocket.hpp"
#include <boost/python.hpp>
using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ZmqSocket_Init_overloads, Init, 3, 4);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ZmqSocket_Filter_overloads, Filter, 0, 1);
BOOST_PYTHON_MODULE(ZmqSocket)
{
    enum_<_CommProtocol>("_CommProtocol")
        .value("ZmqThread", ZmqThread)
        .value("ZmqProcess", ZmqProcess)
        .value("ZmqTcpIp", ZmqTcpIp);
    enum_<_MsgProtocol>("_MsgProtocol")
        .value("RepReq", RepReq)
        .value("PubSub", PubSub)
        .value("PushPull", PushPull);
    enum_<_ZmqType>("_ZmqType")
        .value("ZmqRep", ZmqRep)
        .value("ZmqReq", ZmqReq)
        .value("ZmqPub", ZmqPub)
        .value("ZmqSub", ZmqSub)
        .value("ZmqPush", ZmqPush)
        .value("ZmqPull", ZmqPull);
    enum_<_ZmqBindType>("_ZmqBindType")
        .value("ZmqBind", ZmqBind)
        .value("ZmqConnect", ZmqConnect);
    class_<_ZmqSocket>("_ZmqSocket", init<_CommProtocol, string, _ZmqType, optional<_ZmqBindType, int> >())
        .def("Init", &_ZmqSocket::Init, ZmqSocket_Init_overloads())
        .def("Filter", &_ZmqSocket::Filter, ZmqSocket_Filter_overloads())
        .def("Send", &_ZmqSocket::Send)
        .def("SendMore", &_ZmqSocket::SendMore)
        .def("Recv", &_ZmqSocket::Recv)
        .def("Dump", &_ZmqSocket::Dump);
}