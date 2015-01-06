/**
 * This file is generated by jsonrpcstub, DO NOT CHANGE IT MANUALLY!
 */

#ifndef JSONRPC_CPP_STUB_ABSTRACTSTUBSERVER_H_
#define JSONRPC_CPP_STUB_ABSTRACTSTUBSERVER_H_

#include <jsonrpccpp/server.h>

class AbstractStubServer : public jsonrpc::AbstractServer<AbstractStubServer>
{
    public:
        AbstractStubServer(jsonrpc::AbstractServerConnector &conn, jsonrpc::serverVersion_t type = jsonrpc::JSONRPC_SERVER_V2) : jsonrpc::AbstractServer<AbstractStubServer>(conn, type)
        {
            this->bindAndAddMethod(jsonrpc::Procedure("run", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_ARRAY, "pid",jsonrpc::JSON_INTEGER,"sid",jsonrpc::JSON_INTEGER,"submission",jsonrpc::JSON_ARRAY, NULL), &AbstractStubServer::runI);
            this->bindAndAddMethod(jsonrpc::Procedure("loadConf", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_ARRAY, "pid",jsonrpc::JSON_INTEGER, NULL), &AbstractStubServer::loadConfI);
            this->bindAndAddMethod(jsonrpc::Procedure("judgeStatus", jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_OBJECT,  NULL), &AbstractStubServer::judgeStatusI);
        }

        inline virtual void runI(const Json::Value &request, Json::Value &response)
        {
            response = this->run(request["pid"].asInt(), request["sid"].asInt(), request["submission"]);
        }
        inline virtual void loadConfI(const Json::Value &request, Json::Value &response)
        {
            response = this->loadConf(request["pid"].asInt());
        }
        inline virtual void judgeStatusI(const Json::Value &request, Json::Value &response)
        {
            (void)request;
            response = this->judgeStatus();
        }
        virtual Json::Value run(int pid, int sid, const Json::Value& submission) = 0;
        virtual Json::Value loadConf(int pid) = 0;
        virtual Json::Value judgeStatus() = 0;
};

#endif //JSONRPC_CPP_STUB_ABSTRACTSTUBSERVER_H_
