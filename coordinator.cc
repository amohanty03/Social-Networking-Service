#include <algorithm>
#include <cstdio>
#include <ctime>

#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/duration.pb.h>
#include <chrono>
#include <sys/stat.h>
#include <sys/types.h>
#include <utility>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <unistd.h>
#include <google/protobuf/util/time_util.h>
#include <grpc++/grpc++.h>

#include "coordinator.grpc.pb.h"
#include "coordinator.pb.h"

using csce662::Confirmation;
using csce662::CoordService;
using csce662::ID;
using csce662::ServerInfo;
using csce662::ServerList;
using csce662::SynchService;
using google::protobuf::Duration;
using google::protobuf::Timestamp;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

struct zNode
{
    int serverID;
    std::string hostname;
    std::string port;
    std::string type;
    std::time_t last_heartbeat;
    bool missed_heartbeat;
    bool isActive();
    bool isMaster;
};

struct sNode
{
    int syncID;
    std::string hostname;
    std::string port;
    std::string type;
    bool isMaster;
};

// potentially thread safe
std::mutex v_mutex;
std::vector<zNode *> cluster1;
std::vector<zNode *> cluster2;
std::vector<zNode *> cluster3;

std::vector<sNode *> clusterSync1;
std::vector<sNode *> clusterSync2;
std::vector<sNode *> clusterSync3;

// creating a vector of vectors containing znodes
std::vector<std::vector<zNode *>> clusters = {cluster1, cluster2, cluster3};
std::vector<std::vector<sNode *>> clustersSyncs = {clusterSync1, clusterSync2, clusterSync3};

// func declarations
int findServer(std::vector<zNode *> v, int id);
int findSync(std::vector<sNode *> v, int id);
std::time_t getTimeNow();
void checkHeartbeat();

// this function returns the index of the required server in its cluster array
int findServer(std::vector<zNode *> v, int id)
{
    // v_mutex.lock();
    // std::cout << "Got to findServer with serverID: " << id << std::endl;
    // int serverIDs = ((id - 1) % 3) + 1;
    for (size_t i = 0; i < v.size(); ++i)
    {
        // std::cout << "Checking serverID: " << v[i]->serverID << " clusterID: " << id << " isMaster: " << v[i]->isMaster << " type: " << v[i]->type << std::endl;
        if (v[i]->serverID == id)
        {
            // v_mutex.unlock();
            // std::cout << "found and exiting: " << i << std::endl;
            return i; // Return the index of the zNode with the matching serverId
        }
    }

    // at this point no appropriate server was found
    return -1;
}

// this function returns the index of the required server in its cluster array
int findMasterServer(std::vector<zNode *> v, int id)
{
    // v_mutex.lock();
    // std::cout << "Got to findMasterServer with serverID: " << id << std::endl;
    // int serverIDs = ((id - 1) % 3) + 1;
    for (size_t i = 0; i < v.size(); ++i)
    {
        // std::cout << "Checking serverID: " << v[i]->serverID << " clusterID: " << id << " isMaster: " << v[i]->isMaster << " type: " << v[i]->type << std::endl;
        if (v[i]->isMaster)
        {
            // v_mutex.unlock();
            // std::cout << "found and exiting: " << i << std::endl;
            return i; // Return the index of the zNode with the matching serverId
        }
    }

    // at this point no appropriate server was found
    return -1;
}

// this function returns the index of the required synchronizer in its cluster array
int findSync(std::vector<sNode *> v, int id) {
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (v[i]->syncID == id)
        {
            return i; // Return the index of the sNode with the matching syncID
        }
    }

    // at this point no appropriate synchronizer was found
    return -1;
}


bool zNode::isActive()
{
    bool status = false;
    if (!missed_heartbeat)
    {
        status = true;
    }
    else if (difftime(getTimeNow(), last_heartbeat) < 10)
    {
        status = true;
    }
    return status;
}


class CoordServiceImpl final : public CoordService::Service
{

    Status Heartbeat(ServerContext *context, const ServerInfo *serverinfo, Confirmation *confirmation) override
    {

        // using a multimap to extract custom metadata from the server's grpc to the coordinator
        const std::multimap<grpc::string_ref, grpc::string_ref> &metadata = context->client_metadata();

        std::string clusterid;
        int intClusterid;
        auto it = metadata.find("clusterid");
        if (it != metadata.end())
        {
            // customValue is the clusterid from the metadata received in the server's rpc
            std::string customValue(it->second.data(), it->second.length());

            clusterid = customValue;
            intClusterid = std::stoi(clusterid);
        }

        // std::cout << "Got Heartbeat! Serverid:" << serverinfo->type() << "(" << serverinfo->serverid() << ") and clusterid: (" << clusterid << ")\n";

        auto it2 = metadata.find("heartbeat");
        if (it2 != metadata.end())
        { // HEARTBEAT RECEIVED
            // customValue2 is the heartbeat from the metadata received from the server
            std::string customValue2(it2->second.data(), it2->second.length());

            // finding the server for which the heartbeat was received
            int curIndex = findServer(clusters[intClusterid - 1], serverinfo->serverid());
            if (curIndex != -1)
            {
                v_mutex.lock();

                zNode *curZ = clusters[intClusterid - 1][curIndex];
                curZ->last_heartbeat = getTimeNow();

                v_mutex.unlock();
            }
            // else
            // {                                                  // if a heartbeat was received, that means that sometime in the past, the server was registered and stored in our data structure in memory
            //     std::cout << "server's znode was not found\n"; // THIS SHOULD NEVER HAPPEN
            //     return Status::CANCELLED;
            // }
        }
        else
        { // NOT A HEARTBEAT, BUT INSTEAD INITIAL REGISTRATION
            // checking if server already registered but just died and rejoined again
            int curIndex;
            if (serverinfo->type() == "synchronizer")
            {
                curIndex = -1;
            } else {
                curIndex = findServer(clusters[intClusterid - 1], serverinfo->serverid());
                // std::cout << "curIndex BROOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO: " << curIndex << std::endl;
            }
            
            // server is resurrected after it was killed in the past
            if (curIndex != -1)
            {
                v_mutex.lock();

                zNode *curZ = clusters[intClusterid - 1][curIndex];
                curZ->last_heartbeat = getTimeNow(); // updating the latest heartbeat value for the server
                curZ->isMaster = false;
                v_mutex.unlock();

                std::cout << "an inactive server was resurrected " << serverinfo->serverid() << "\n";
            }
            else
            { // first time the server contacts the coordinator and needs to be registered
                if (serverinfo->type() == "synchronizer")
                {
                    bool masterSync = false;
                    std::cout << "new synchronizer registered of type: " << serverinfo->type() << " and serverid: " << serverinfo->serverid() << " of clusterID: " << intClusterid << "\n";
                    sNode *s = new sNode();
                    s->hostname = serverinfo->hostname();
                    s->port = serverinfo->port();
                    s->syncID = serverinfo->serverid();
                    s->type = serverinfo->type();

                    for (sNode* node : clustersSyncs[intClusterid - 1]) {
                        if (node->isMaster) {
                            std::cout << "master sync found in cluster " << intClusterid << std::endl;
                            masterSync = true;
                            break;
                        }
                    }
                    if (masterSync) {
                        std::cout << "master sync found so add slave sync in cluster: " << intClusterid << std::endl;
                        s->isMaster = false;
                        confirmation->set_status(false);
                    } else {
                        std::cout << "master sync not found so change to master sync: " << intClusterid << std::endl;
                        s->isMaster = true;
                        confirmation->set_status(true);
                    }

                    v_mutex.lock();

                    clustersSyncs[intClusterid - 1].push_back(s);

                    v_mutex.unlock();
                }
                else
                {
                    std::cout << "new server registered of type: " << serverinfo->type() << " and serverid: " << serverinfo->serverid() << " of clusterID: " << intClusterid << "\n";
                    zNode *z = new zNode();
                    bool masterFound = false;
                    
                    int masterSyncIndex = 0;
                    int masterIndex = 0;

                    z->hostname = serverinfo->hostname();
                    z->port = serverinfo->port();
                    z->serverID = serverinfo->serverid();
                    z->type = serverinfo->type();

                    for (zNode* node : clusters[intClusterid - 1]) {
                        if (node->isMaster) {
                            std::cout << "master found at " << node->serverID << std::endl;
                            masterFound = true;
                            masterIndex = node->serverID;
                            break;
                        }
                    }

                    if (masterFound) {
                        std::cout << "master found so slave is: " << serverinfo->serverid() << std::endl;
                        z->isMaster = false;
                    } else {
                        std::cout << "master not found so change to master is: " << serverinfo->serverid() << std::endl;
                        z->isMaster = true;
                    }

                    z->last_heartbeat = getTimeNow();

                    v_mutex.lock();

                    // adding the newly created server to its relevant cluster
                    clusters[intClusterid - 1].push_back(z);

                    v_mutex.unlock();
                }
            }
        }
        return Status::OK;
    }

    // function returns the server information for requested client id
    // this function assumes there are always 3 clusters and has math
    // hardcoded to represent this.
    Status GetServer(ServerContext *context, const ID *id, ServerInfo *serverinfo) override
    {
        // std::cout << "Got GetServer for clientID: " << id->id() << std::endl;
        int clusterId = ((id->id() - 1) % 3) + 1;

        // Your code here
        // If server is active, return serverinfo

        // finding a server to assign to the new client
        int curIndex = findMasterServer(clusters[clusterId - 1], clusterId);
        // std::cout << "curindex: " << curIndex << " clusterId: " << clusterId << std::endl;
        if (curIndex != -1)
        {
            v_mutex.lock();
            zNode *curZ = clusters[clusterId - 1][curIndex];
            v_mutex.unlock();
            if (curZ->isActive())
            { // setting the ServerInfo values to return to the client if its server is active
                serverinfo->set_hostname(curZ->hostname);
                serverinfo->set_port(curZ->port);
            }
            else
            {
                std::cout << "The server is not active!\n";
            }
        }
        else
        {
            std::cout << "The server that is supposed to serve the client is down!\n";
        }

        return Status::OK;
    }

    Status GetSlave(ServerContext* context, const ID *id, ServerInfo *serverinfo) {
        std::cout << "Got GetSlave for clientID: " << id->id() << std::endl;

        // Calculate the cluster ID based on client ID
        int clusterId = ((id->id() - 1) % 3) + 1;
        int requestedServerId = serverinfo->serverid();

        // Lock the vector for thread safety
        // v_mutex.lock();

        // Iterate through the servers in the cluster to find a slave
        zNode *slaveNode = nullptr;
        for (zNode *node : clusters[clusterId - 1]) {
            // std::cout << "node->serverID: " << node->serverID << " cluster id: " << clusterId << " master: " << node->isMaster << std::endl;
            if (node->serverID != requestedServerId && !node->isMaster) {
                slaveNode = node;
                break;
            }
        }

        // v_mutex.unlock();

        if (slaveNode != nullptr) {
            // Populate the ServerInfo with the slave's details
            serverinfo->set_hostname(slaveNode->hostname);
            serverinfo->set_port(slaveNode->port);
            serverinfo->set_type(slaveNode->type);
            serverinfo->set_clusterid(clusterId);
            serverinfo->set_serverid(slaveNode->serverID);
            serverinfo->set_ismaster(false);  // Explicitly set as not master
            std::cout << "Found Slave: " << slaveNode->hostname << ":" << slaveNode->port << std::endl;
        } else {
            std::cerr << "Slave server not found in cluster " << clusterId << "!" << std::endl;
            return Status(grpc::NOT_FOUND, "No available slave server found.");
        }

        return Status::OK;
    }

    Status GetAllFollowerServers(ServerContext* context, const ID* id, ServerList* serverlist) override {
        // std::cout << "Got GetAllFollowerServers for clientID: " << id->id() << std::endl;
        
        int clusterId = (id->id() - 1) % 3 + 1;  

        for (auto& cluster : clustersSyncs) {
            // std::cout << "enter first for " << std::endl;
            for (auto& node : cluster) {
                // std::cout << "enter second for " << std::endl;
                // if (node->type == "synchronizer") {
                    // std::cout << "enter if " << std::endl;
                serverlist->add_serverid(node->syncID);
                serverlist->add_hostname(node->hostname);
                serverlist->add_port(node->port);
                // }
            }
        }
        
        return Status::OK;
    }

    Status GetFollowerServer(ServerContext* context, const ID* id, ServerInfo* serverinfo) override {
        // std::cout << "Got GetFollowerServer for clientID: " << id->id() << std::endl;

        int clusterId = (id->id() - 1) % 3 + 1;  // Calculate cluster ID based on client ID

        for (auto& cluster : clusters) {
            for (auto& node : cluster) {
                serverinfo->set_serverid(node->serverID);
                serverinfo->set_hostname(node->hostname);
                serverinfo->set_port(node->port);
                serverinfo->set_type(node->type);
            }
        }
        // for (zNode* node : clusters[clusterId - 1]) {
        //     if (!node->isMaster && node->isActive()) {  // Find an active follower
        //         serverinfo->set_serverid(node->serverID);
        //         serverinfo->set_hostname(node->hostname);
        //         serverinfo->set_port(node->port);
        //         serverinfo->set_type(node->type);
        //         return Status::OK;  // Successfully found a follower
        //     }
        // }

        std::cerr << "No active synchronizer found in cluster: " << clusterId << std::endl;
        return Status(grpc::NOT_FOUND, "No active synchronizer found");
    }



    Status IsMaster(ServerContext* context, const ServerInfo* serverinfo, Confirmation* confirmation) {
        // std::cout << "Coord IsMaster" << std::endl;
        int clusterId = serverinfo->clusterid();
        int serverId = serverinfo->serverid();

        // v_mutex.lock();
        auto& cluster = clusters[clusterId - 1];  // Get the cluster corresponding to clusterId
        // std::cout << "Got ClusteriD: " << clusterId << "Got serverid" << serverId << std::endl;
        // Find the server in the cluster
        int curIndex = findServer(clusters[clusterId - 1], serverId);
        // std::cout << "Got Index" << std::endl;
        // auto it = std::find_if(cluster.begin(), cluster.end(), [serverId](zNode* node) {
        //     return node->serverID == serverId;
        // });

        if (curIndex != -1) {
            // std::cout << "Coord IsMaster curIndex -1" << std::endl;
            zNode* server = clusters[clusterId - 1][curIndex];
            confirmation->set_status(server->isMaster);  // Set status based on isMaster flag
        } else {
            confirmation->set_status(false);  // Server not found although it shouldn't reach here
        }

        // v_mutex.unlock();
        return Status::OK;
    }

};

void RunServer(std::string port_no)
{
    // start thread to check heartbeats
    std::thread hb(checkHeartbeat);
    // localhost = 127.0.0.1
    std::string server_address("127.0.0.1:" + port_no);
    CoordServiceImpl service;
    // grpc::EnableDefaultHealthCheckService(true);
    // grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char **argv)
{

    std::string port = "3010";
    int opt = 0;
    while ((opt = getopt(argc, argv, "p:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            port = optarg;
            break;
        default:
            std::cerr << "Invalid Command Line Argument\n";
        }
    }
    RunServer(port);
    return 0;
}

void checkHeartbeat()
{
    while (true)
    {
        // check servers for heartbeat > 10
        // if true turn missed heartbeat = true
        //  Your code below
        v_mutex.lock();

        // iterating through the clusters vector of vectors of znodes
        for (auto &c : clusters)
        {
            for (auto &s : c)
            {
                if (difftime(getTimeNow(), s->last_heartbeat) > 10)
                {
                    std::cout << "missed heartbeat from server " << s->serverID << std::endl;
                    if (!s->missed_heartbeat)
                    {
                        s->missed_heartbeat = true;
                        s->last_heartbeat = getTimeNow();
                    } else { // missing second heartbeat will enter this section
                        s->isMaster = false; // master is dead, demote to slave
                        for (auto &servers : c)
                        {
                            if ((servers->isMaster == false) && (servers->isActive())) {
                                servers->isMaster = true;
                                break;
                            }
                        }
                    }
                }
            }
        }

        v_mutex.unlock();

        sleep(3);
    }
}

std::time_t getTimeNow()
{
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}
