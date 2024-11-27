/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <cstddef>
#include <cstdlib>
#include <thread>
#include <cstdio>
#include <ctime>
#include <csignal>

#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/duration.pb.h>

#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <unordered_map>
#include <google/protobuf/util/time_util.h>
#include <filesystem>
#include <grpc++/grpc++.h>
#include <glog/logging.h>
#define log(severity, msg) \
    LOG(severity) << msg;  \
    google::FlushLogFiles(google::severity);

#include "sns.grpc.pb.h"
#include "coordinator.grpc.pb.h"
#include "coordinator.pb.h"
#include "client.h"

using csce662::CoordService;
using csce662::ListReply;
using csce662::Message;
using csce662::Reply;
using csce662::Request;
using csce662::SNSService;
using google::protobuf::Duration;
using google::protobuf::Timestamp;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

struct Client
{
    std::string username;
    bool connected = true;
    int following_file_size = 0;
    std::vector<Client *> client_followers;
    std::vector<Client *> client_following;
    ServerReaderWriter<Message, Message> *stream = 0;
    // adding these two new variables below to monitor client heartbeats
    std::time_t last_heartbeat;
    bool missed_heartbeat = false;
    bool operator==(const Client &c1) const
    {
        return (username == c1.username);
    }
};

void checkHeartbeat();
std::time_t getTimeNow();
std::string clusterId;
std::string serverId;
std::string serverFileName;

std::unique_ptr<csce662::CoordService::Stub> coordinator_stub_;

// coordinator rpcs
IReply Heartbeat(std::string clusterId, std::string serverId, std::string hostname, std::string port);

// Vector that stores every client that has been created
/* std::vector<Client*> client_db; */

// using an unordered map to store clients rather than a vector as this allows for O(1) accessing and O(1) insertion
std::unordered_map<std::string, Client *> client_db;

// util function for checking if a client exists in the client_db and fetching it if it does
Client *getClient(std::string username)
{
    auto it = client_db.find(username);

    if (it != client_db.end())
    {
        return client_db[username];
    }
    else
    {
        return NULL;
    }
}

bool checkIfMaster(std::string clusterId, std::string serverId)
{
    // std::cout << "It comes here" << std::endl;
    grpc::ClientContext context;
    csce662::ServerInfo serverInfo;
    csce662::Confirmation confirmation;

    // Set the server information
    serverInfo.set_clusterid(std::stoi(clusterId));
    serverInfo.set_serverid(std::stoi(serverId));

    // std::cout << "Before RPC" << std::endl;
    // Call the IsMaster RPC on the Coordinator
    grpc::Status status = coordinator_stub_->IsMaster(&context, serverInfo, &confirmation);
    // std::cout << "After RPC" << std::endl;

    if (!status.ok())
    {
        std::cerr << "Failed to check if server is Master: " << status.error_message() << std::endl;
        return false; // Assume not Master if the call fails
    }

    return confirmation.status(); // True if Master, False otherwise
}

// bool file_contains_user(const std::string& filePath, const std::string& user) {
//     std::ifstream inFile(filePath);
//     std::string line;

//     while (std::getline(inFile, line)) {
//         if (line == user) {
//             return true;
//         }
//     }

//     return false;
// }
void loadClientDB()
{
    std::string filePath = serverFileName + "/all_users.txt";
    std::ifstream inFile(filePath);
    if (!inFile.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    std::string username;
    while (std::getline(inFile, username))
    {
        if (!username.empty() && client_db.find(username) == client_db.end())
        {
            // Add user to client_db if not already present
            Client *newClient = new Client();
            newClient->username = username;
            newClient->connected = false;  // Assume disconnected by default
            newClient->last_heartbeat = 0; // No heartbeat yet
            client_db[username] = newClient;
        }
    }

    inFile.close();
}

void syncClientDB()
{
    while (true)
    {
        loadClientDB();
        std::this_thread::sleep_for(std::chrono::seconds(3)); // Adjust interval as needed
    }
}

void loadFollowersAndFollowing(Client *client)
{
    // client->client_followers.clear();
    client->client_following.clear();
    std::string following_file_path = serverFileName + "/" + client->username + "_following.txt";
    std::string followers_file_path = serverFileName + "/" + client->username + "_followers.txt";
    // Load following
    std::ifstream following_file(following_file_path);
    if (!following_file)
    {
        log(ERROR, "Failed to open the following file for reading: " + following_file_path);
        return;
    }
    std::string following_username;
    while (std::getline(following_file, following_username))
    {
        if (following_username.empty())
        {
            continue;
        }
        Client *following_client = getClient(following_username);
        client->client_following.push_back(following_client);
    }
    following_file.close();
    // Load followers
    std::ifstream followers_file(followers_file_path);
    if (!followers_file)
    {
        log(ERROR, "Failed to open the follower file for reading: " + followers_file_path);
        return;
    }
    std::string follower_username;
    while (std::getline(followers_file, follower_username))
    {
        if (follower_username.empty())
        {
            continue;
        }
        Client *follower_client = getClient(follower_username);
        bool followerPresent = false;
        for (Client *followers : client->client_followers)
        {
            if (followers == follower_client)
            {
                followerPresent = true;
                break;
            }
        }
        if (!followerPresent)
        {
            client->client_followers.push_back(follower_client);
        }
    }
    followers_file.close();
}

void updateAllUsersFile()
{
    std::string masterFilePath = "./cluster_" + clusterId + "/1/all_users.txt";
    std::string slaveFilePath = "./cluster_" + clusterId + "/2/all_users.txt";
    std::cout << "Updating all users file" << masterFilePath << std::endl;

    std::vector<std::string> loggedInUsers;
    for (const auto &pair : client_db)
    {
        if (pair.second->connected)
        {
            std::cout << "added user: " << pair.second->username << std::endl;
            loggedInUsers.push_back(pair.second->username);
        }
    }

    for (const auto &pair : client_db)
    {
        std::cout << "user: " << pair.second->username << std::endl;
    }

    // Update master file
    {
        std::ofstream masterFile(masterFilePath, std::ios::trunc); // Overwrite the file
        for (const auto &user : loggedInUsers)
        {
            masterFile << user << std::endl;
        }
    }

    // Update slave file
    {
        std::ofstream slaveFile(slaveFilePath, std::ios::trunc); // Overwrite the file
        for (const auto &user : loggedInUsers)
        {
            slaveFile << user << std::endl;
        }
    }
}

// function to get all users from the master and slave files and return the longest list
// std::vector<std::string> getAllUsers() {
//     std::vector<std::string> users;
//     std::ifstream masterFile(serverFileName + "/all_users.txt");
//     std::string line;
//     while(std::getline(masterFile, line)) {
//         users.push_back(line);
//     }
//     return users;
// }

// // function to write to the users file
// void writeUsersFile(const std::vector<std::string>& users) {
//     std::ofstream usersFile(serverFileName + "/all_users.txt");
//     for (const auto& user : users) {
//         usersFile << user << std::endl;
//     }
// }

// // function to get all the followers of a user
// std::vector<std::string> getFollowersOfUser(std::string& userId) {
//     std::vector<std::string> followers;
//     std::ifstream followersFile(serverFileName + "/" + userId + "_followers.txt");
//     std::string line;
//     while(std::getline(followersFile, line)) {
//         followers.push_back(line);
//     }
//     return followers;
// }

// // function to get all the following of a user
// std::vector<std::string> getFollowingOfUser(std::string& userId) {
//     std::vector<std::string> following;
//     std::ifstream followingFile(serverFileName + "/" + userId + "_following.txt");
//     std::string line;
//     while(std::getline(followingFile, line)) {
//         following.push_back(line);
//     }
//     return following;
// }

class SNSServiceImpl final : public SNSService::Service
{

    Status ClientHeartbeat(ServerContext *context, const Request *request, Reply *reply) override
    {

        std::string username = request->username();

        // std::cout << "got a heartbeat from client: " << username << std::endl;
        Client *c = getClient(username);
        if (c != NULL)
        {
            c->last_heartbeat = getTimeNow();
        }
        else
        {
            std::cout << "client was not found, for some reason!\n";
            return Status::CANCELLED;
        }

        return Status::OK;
    }

    Status List(ServerContext *context, const Request *request, ListReply *list_reply) override
    {
        std::string username = request->username();

        // Read all known users from all_users.txt
        // std::string filePath = "./cluster_" + clusterId + "/" + serverId + "/all_users.txt";
        std::ifstream inFile(serverFileName + "/all_users.txt");
        if (!inFile.is_open())
        {
            std::cerr << "Failed to open file: " << serverFileName + "/all_users.txts" << std::endl;
            return Status::CANCELLED;
        }

        std::string line;
        while (std::getline(inFile, line))
        {
            if (!line.empty())
            {
                list_reply->add_all_users(line);
            }
        }
        inFile.close();

        // Add followers for the client from `client_db`
        Client *c = getClient(username);
        // loadFollowersAndFollowing(c);
        if (c != NULL)
        {
            // loadFollowersAndFollowing(c);
            for (Client *x : c->client_followers)
            {
                list_reply->add_followers(x->username);
            }
        }
        else
        {
            return Status::CANCELLED;
        }

        // add all known clients to the all_users vector
        // for (const auto& pair : client_db){
        //     list_reply->add_all_users(pair.first);
        // }

        // std::string username = request->username();

        // // add all the followers of the client to the folowers vector
        // Client* c = getClient(username);
        // if (c != NULL){
        //     for (Client* x : c->client_followers){
        //         list_reply->add_followers(x->username);
        //     }

        // } else {
        //     return Status::CANCELLED;
        // }

        // loadClientDB();
        // std::string curr_username = request->username();
        // // Iterate over the Client DB and add all usernames, and their followers
        // std::vector<std::string> users;
        // std::vector<std::string> followers;
        // for (const auto& pair : client_db) {
        //     Client *user = pair.second;
        //     users.push_back(user->username);
        //     // list_reply->add_all_users(user->username);

        //     // When we go over the current user's client, we add all the followers
        //     if (curr_username.compare(user->username) == 0) {
        //         for (Client *follower : user->client_followers) {
        //             followers.push_back(follower->username);
        //             // list_reply->add_followers(follower->username);
        //         }
        //     }
        // }
        // std::sort(users.begin(), users.end());
        // std::sort(followers.begin(), followers.end());
        // for (std::string user : users) {
        //     list_reply->add_all_users(user);
        // }
        // for (std::string follower : followers) {
        //     list_reply->add_followers(follower);
        // }

        return Status::OK;
    }

    Status Follow(ServerContext *context, const Request *request, Reply *reply) override
    {

        std::string u1 = request->username();
        std::string u2 = request->arguments(0);
        Client *c1 = getClient(u1);
        Client *c2 = getClient(u2);

        if (c1 == nullptr || c2 == nullptr)
        { // either of the clients dont exist
            return Status(grpc::CANCELLED, "invalid username");
        }

        if (c1 == c2)
        { // if a client is asked to follow itself
            return Status(grpc::CANCELLED, "same client");
        }

        // check if the client to follow is already being followed
        bool isAlreadyFollowing = std::find(c1->client_following.begin(), c1->client_following.end(), c2) != c1->client_following.end();

        if (isAlreadyFollowing)
        {
            return Status(grpc::CANCELLED, "already following");
        }

        // add the clients to each other's relevant vector
        c1->client_following.push_back(c2);
        c2->client_followers.push_back(c1);

        std::string followingFilePath = serverFileName + "/" + u1 + "_following.txt";
        std::string followersFilePath = serverFileName + "/" + u2 + "_followers.txt";

        {
            // Append to following.txt for c1
            std::ofstream followingFile(followingFilePath, std::ios::app);
            if (!followingFile)
            {
                return Status(grpc::CANCELLED, "Failed to update following file");
            }
            followingFile << u2 << std::endl;
        }

        {
            // Append to followers.txt for c2
            std::ofstream followersFile(followersFilePath, std::ios::app);
            if (!followersFile)
            {
                return Status(grpc::CANCELLED, "Failed to update followers file");
            }
            followersFile << u1 << std::endl;
        }

        if (checkIfMaster(clusterId, serverId))
        {
            grpc::ClientContext ctx;
            csce662::ID id;
            csce662::ServerInfo serverInfo;
            serverInfo.set_serverid(std::stoi(serverId));
            int userId = std::stoi(u1);
            id.set_id(userId);
            grpc::Status status = coordinator_stub_->GetSlave(&ctx, id, &serverInfo);

            if (!status.ok())
            {
                std::cerr << "Failed to retrieve Slave info: " << status.error_message() << std::endl;
                return Status(grpc::INTERNAL, "Failed to retrieve Slave info");
            }

            // Forward Login request to Slave
            serverId = serverInfo.serverid();
            auto slave_stub = csce662::SNSService::NewStub(
                grpc::CreateChannel(serverInfo.hostname() + ":" + serverInfo.port(), grpc::InsecureChannelCredentials()));

            grpc::ClientContext slave_ctx;
            Request slave_request;
            slave_request.set_username(u1);
            slave_request.add_arguments(u2);
            Reply slave_reply;

            grpc::Status slave_status = slave_stub->Follow(&slave_ctx, slave_request, &slave_reply);
            if (!slave_status.ok())
            {
                std::cerr << "Failed to forward Follow request to Slave: " << slave_status.error_message() << std::endl;
                return Status(grpc::INTERNAL, "Failed to forward Follow request");
            }
        }

        return Status::OK;
    }

    Status UnFollow(ServerContext *context, const Request *request, Reply *reply) override
    {

        std::string username = request->username();
        // using a multimap to fetch the metadata out of the client's servercontext so we can check to see if a SIGINT was issued on the client's timeline
        const std::multimap<grpc::string_ref, grpc::string_ref> &metadata = context->client_metadata();

        auto it = metadata.find("terminated");
        if (it != metadata.end())
        {
            std::string customValue(it->second.data(), it->second.length());

            std::string termStatus = customValue; // checking the value of the key "terminated" from the metadata in servercontext
            if (termStatus == "true")
            {

                Client *c = getClient(username);
                if (c != NULL)
                { // if the client exists, change its connection status and set its stream to null
                    c->last_heartbeat = getTimeNow();
                    c->connected = false;
                    c->stream = nullptr;
                }
                // DO NOT CONTINUE WITH UNFOLLOW AFTER THIS
                // Terminate here as this was not an actual unfollow request and just a makeshift way to handle SIGINTs on the client side
                return Status::OK;
            }
        }

        std::string u1 = request->username();
        std::string u2 = request->arguments(0);
        Client *c1 = getClient(u1);
        Client *c2 = getClient(u2);

        if (c1 == nullptr || c2 == nullptr)
        {
            return Status(grpc::CANCELLED, "invalid username");
        }

        if (c1 == c2)
        {
            return Status(grpc::CANCELLED, "same client");
        }

        // Find and erase c2 from c1's following
        auto it1 = std::find(c1->client_following.begin(), c1->client_following.end(), c2);
        if (it1 != c1->client_following.end())
        {
            c1->client_following.erase(it1);
        }
        else
        {
            return Status(grpc::CANCELLED, "not following");
        }

        // if it gets here, it means it was following the other client
        // Find and erase c1 from c2's followers
        auto it2 = std::find(c2->client_followers.begin(), c2->client_followers.end(), c1);
        if (it2 != c2->client_followers.end())
        {
            c2->client_followers.erase(it2);
        }

        return Status::OK;
    }

    // RPC Login
    Status Login(ServerContext *context, const Request *request, Reply *reply) override
    {
        std::string username = request->username();

        Client *c = getClient(username);
        // std::vector<std::string> allUsers = getAllUsers();
        // bool newUserFlag = false;
        // for (auto& client : allUsers) {
        //     if (client == username) {
        //         newUserFlag = false;
        //     }
        // }

        // if (newUserFlag) {
        //     allUsers.push_back(username);
        //     writeUsersFile(allUsers);
        // }

        // client_db.clear();
        // for (auto& client : allUsers) {
        //     Client* newClient = new Client();
        //     newClient->username = client;
        //     newClient->connected = true;
        //     // newClient->client_following = getFollowingOfUser(username);
        //     // newClient->client_followers = getFollowersOfUser(username);
        //     client_db[client] = newClient;
        // }
        loadClientDB();
        if (c == NULL)
        {
            std::cout << "first req client" << std::endl;
            Client *newc = new Client();
            newc->username = username;
            newc->connected = true;
            newc->last_heartbeat = getTimeNow();
            newc->missed_heartbeat = false;
            client_db[username] = newc;
            // truncateFile(client->username);
            std::ofstream all_users(serverFileName + "/all_users.txt", std::ios::app);
            all_users << request->username() << std::endl;
            all_users.close();
        }

        if (checkIfMaster(clusterId, serverId))
        {
            std::cout << "Check master first time" << std::endl;
            grpc::ClientContext ctx;
            csce662::ID id;
            csce662::ServerInfo serverInfo;
            serverInfo.set_serverid(std::stoi(serverId));
            int userId = std::stoi(username);
            id.set_id(userId);
            grpc::Status status = coordinator_stub_->GetSlave(&ctx, id, &serverInfo);

            if (!status.ok())
            {
                std::cerr << "Failed to retrieve Slave info: " << status.error_message() << std::endl;
                return Status(grpc::OK, "Failed to retrieve Slave info");
            }

            auto slave_stub = csce662::SNSService::NewStub(
                grpc::CreateChannel(serverInfo.hostname() + ":" + serverInfo.port(), grpc::InsecureChannelCredentials()));

            grpc::ClientContext slave_ctx;
            Request slave_request;
            slave_request.set_username(username);
            Reply slave_reply;

            grpc::Status slave_status = slave_stub->Login(&slave_ctx, slave_request, &slave_reply);
            if (!slave_status.ok())
            {
                std::cerr << "Failed to forward Login request to Slave: " << slave_status.error_message() << std::endl;
                return Status(grpc::INTERNAL, "Failed to forward Login request");
            }
        }

        // Update all_users.txt to reflect current state of connected users
        updateAllUsersFile();
        loadClientDB();
        // loadFollowersAndFollowing(c);

        return Status::OK;
    }

    const int MAX_MESSAGES = 20;

    Status Timeline(ServerContext *context, ServerReaderWriter<Message, Message> *stream) override
    {

        // Initialize variables important for persisting timelines on the disk
        Message m;
        Client *c;
        std::string u;
        std::vector<std::string> latestMessages;
        std::vector<std::string> allMessages;
        bool firstTimelineStream = true;

        // multimap to fetch metadata from the servercontext which contains the username of the current client
        // this helps to Initialize the stream for this client as this is first contact
        const std::multimap<grpc::string_ref, grpc::string_ref> &metadata = context->client_metadata();

        auto it = metadata.find("username");
        if (it != metadata.end())
        {
            std::string customValue(it->second.data(), it->second.length());

            // customValue is the username from the metadata received from the client
            u = customValue;
            c = getClient(u);
            c->stream = stream; // set the client's stream to be the current stream
        }

        // if this is the first time the client is logging back
        if (firstTimelineStream && c != nullptr)
        {
            // Read latest 20 messages from following file
            std::ifstream followingFile(u + "_following.txt");
            if (followingFile.is_open())
            {
                std::string line;
                while (std::getline(followingFile, line))
                {
                    allMessages.push_back(line);
                }

                // Determine the starting index for retrieving latest messages
                int startIndex = std::max(0, static_cast<int>(allMessages.size()) - MAX_MESSAGES);

                // Retrieve the latest messages
                for (int i = startIndex; i < allMessages.size(); ++i)
                {
                    latestMessages.push_back(allMessages[i]);
                }
                std::reverse(latestMessages.begin(), latestMessages.end()); // reversing the vector to match the assignment description
                followingFile.close();
            }

            // Send latest 20 messages to client via the grpc stream
            for (const std::string &msg : latestMessages)
            {
                Message latestMessage;
                latestMessage.set_msg(msg + "\n");
                stream->Write(latestMessage);
            }
            firstTimelineStream = false;
        }

        while (stream->Read(&m))
        { // while there are messages being sent by the client over the stream

            if (c != nullptr)
            {

                // Convert timestamp to string
                std::time_t timestamp_seconds = m.timestamp().seconds();
                std::tm *timestamp_tm = std::gmtime(&timestamp_seconds);

                char time_str[50]; // Make sure the buffer is large enough
                std::strftime(time_str, sizeof(time_str), "%a %b %d %T %Y", timestamp_tm);

                std::string ffo = u + '(' + time_str + ')' + " >> " + m.msg();

                // Append to user's timeline file
                std::ofstream userFile(u + ".txt", std::ios_base::app);
                if (userFile.is_open())
                {
                    userFile.seekp(0, std::ios_base::beg);
                    userFile << ffo;
                    userFile.close();
                }

                // Send the new message to all followers for their timeline
                for (Client *follower : c->client_followers)
                {
                    if (follower->stream != nullptr)
                    {
                        Message followerMessage;
                        followerMessage.set_msg(ffo);

                        if (follower->stream != nullptr)
                        {
                            follower->stream->Write(followerMessage);
                        }
                    }
                }

                // Append to  all the followers' following file
                for (Client *follower : c->client_followers)
                {
                    std::ofstream followerFile(follower->username + "_following.txt", std::ios_base::app);
                    if (followerFile.is_open())
                    {
                        followerFile.seekp(0, std::ios_base::beg);
                        followerFile << ffo;
                        followerFile.close();
                    }
                }
            }
        }

        // Check if this server is Master
        if (checkIfMaster(clusterId, serverId))
        {
            grpc::ClientContext ctx;
            csce662::ID id;
            csce662::ServerInfo slaveInfo;

            // Set user ID in request
            id.set_id(std::stoi(u));

            // Retrieve Slave information from Coordinator
            grpc::Status status = coordinator_stub_->GetSlave(&ctx, id, &slaveInfo);
            if (!status.ok())
            {
                std::cerr << "Failed to retrieve Slave info: " << status.error_message() << std::endl;
                return Status(grpc::INTERNAL, "Failed to retrieve Slave info");
            }

            // Establish connection with Slave server
            auto slave_stub = csce662::SNSService::NewStub(
                grpc::CreateChannel(slaveInfo.hostname() + ":" + slaveInfo.port(), grpc::InsecureChannelCredentials()));

            grpc::ClientContext slave_ctx;

            // Create a bidirectional stream with Slave
            auto slave_stream = slave_stub->Timeline(&slave_ctx);

            // Forward all messages between client and Slave
            std::thread client_to_slave([&]()
                                        {
                                            Message client_msg;
                                            while (stream->Read(&client_msg))
                                            {                                    // Read from client
                                                slave_stream->Write(client_msg); // Forward to Slave
                                            }
                                            slave_stream->WritesDone(); // Signal end of writes
                                        });

            std::thread slave_to_client([&]()
                                        {
                Message slave_msg;
                while (slave_stream->Read(&slave_msg)) {  // Read from Slave
                    stream->Write(slave_msg);  // Forward to client
                } });

            client_to_slave.join();
            slave_to_client.join();

            return Status::OK;
        }

        return Status::OK;
    }
};

// function that sends a heartbeat to the coordinator
IReply Heartbeat(std::string clusterId, std::string serverId, std::string hostname, std::string port, bool isHeartbeat)
{

    IReply ire;

    // creating arguments and utils to make the gRPC
    ClientContext context;
    csce662::ServerInfo serverinfo;
    csce662::Confirmation confirmation;

    if (isHeartbeat)
    {
        context.AddMetadata("heartbeat", "Hello"); // adding the server's clusterId in the metadata so the coordinator can know
    }

    context.AddMetadata("clusterid", clusterId); // adding the server's clusterId in the metadata so the coordinator can know

    int intServerId = std::stoi(serverId);

    serverinfo.set_serverid(intServerId);
    serverinfo.set_hostname(hostname);
    serverinfo.set_port(port);

    grpc::Status status = coordinator_stub_->Heartbeat(&context, serverinfo, &confirmation);
    if (status.ok())
    {
        ire.grpc_status = status;
    }
    else
    { // professor said in class that since the servers cannot be run without a coordinator, you should exit

        ire.grpc_status = status;
        std::cout << "coordinator not found! exiting now...\n";
    }

    return ire;
}

// function that runs inside a detached thread that calls the heartbeat function
void sendHeartbeat(std::string clusterId, std::string serverId, std::string hostname, std::string port)
{
    while (true)
    {

        sleep(10);

        IReply reply = Heartbeat(clusterId, serverId, "localhost", port, true);
        if (!reply.grpc_status.ok())
        {
            exit(1);
        }
    }
}

void RunServer(std::string clusterId, std::string serverId, std::string coordinatorIP, std::string coordinatorPort, std::string port_no)
{
    std::string server_address = "0.0.0.0:" + port_no;
    SNSServiceImpl service;

    // running the heartbeat function to monitor heartbeats from the clients
    std::thread hb(checkHeartbeat);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    log(INFO, "Server listening on " + server_address);

    // FROM WHAT I UNDERSTAND, THIS IS THE BEST PLACE TO REGISTER WITH THE COORDINATOR

    // need to first create a stub to communicate with the coordinator to get the info of the server to connect to
    std::string coordinator_address = coordinatorIP + ":" + coordinatorPort;
    grpc::ChannelArguments channel_args;
    std::shared_ptr<grpc::Channel> channel = grpc::CreateCustomChannel(
        coordinator_address, grpc::InsecureChannelCredentials(), channel_args);

    // Instantiate the coordinator stub
    coordinator_stub_ = csce662::CoordService::NewStub(channel);
    IReply reply = Heartbeat(clusterId, serverId, "localhost", port_no, false);
    if (!reply.grpc_status.ok())
    {
        // EXITING AS COORDINATOR WAS NOT REACHABLE
        exit(0);
    }

    // running a thread to periodically send a heartbeat to the coordinator
    std::thread myhb(sendHeartbeat, clusterId, serverId, "localhost", port_no);

    myhb.detach();

    server->Wait();
}

void checkHeartbeat()
{
    while (true)
    {
        // check clients for heartbeat > 3s

        for (const auto &pair : client_db)
        {
            if (difftime(getTimeNow(), pair.second->last_heartbeat) > 3)
            {
                // std::cout << "missed heartbeat from client with id " << pair.first << std::endl;
                if (!pair.second->missed_heartbeat)
                {
                    Client *current = getClient(pair.first);
                    if (current != NULL)
                    {
                        // std::cout << "setting the client's values in the DB to show that it is down!\n";
                        // current->connected = false;
                        current->stream = nullptr;
                        current->missed_heartbeat = true;
                        current->last_heartbeat = getTimeNow();
                    }
                    else
                    {
                        std::cout << "SUDDENLY, THE CLIENT CANNOT BE FOUND?!\n";
                    }
                }
            }
        }

        sleep(3);
    }
}

int main(int argc, char **argv)
{

    clusterId = "1";
    serverId = "1";
    std::string coordinatorIP = "localhost";
    std::string coordinatorPort = "9090";
    std::string port = "1000";

    int opt = 0;
    while ((opt = getopt(argc, argv, "c:s:h:k:p:")) != -1)
    {
        switch (opt)
        {
        case 'c':
            clusterId = optarg;
            break;
        case 's':
            serverId = optarg;
            break;
        case 'h':
            coordinatorIP = optarg;
            break;
        case 'k':
            coordinatorPort = optarg;
            break;
        case 'p':
            port = optarg;
            break;
        default:
            std::cout << "Invalid Command Line Argument\n";
        }
    }

    std::string log_file_name = std::string("server-") + port;
    google::InitGoogleLogging(log_file_name.c_str());
    log(INFO, "Logging Initialized. Server starting...");

    /* RunServer(port); */
    // changing this call so i can pass other auxilliary variables to be able to communicate with the server
    // setupDirectories();
    serverFileName = "cluster_" + clusterId + "/" + serverId;
    // std::cout << "serverFileName I PRINTED: " << serverFileName << std::endl;
    std::string cmd = "rm -rf " + serverFileName + " && mkdir -p " + serverFileName;
    system(cmd.c_str());
    // std::cout << "RESULT: " << result << std::endl;
    std::thread syncThread(syncClientDB);
    syncThread.detach();
    RunServer(clusterId, serverId, coordinatorIP, coordinatorPort, port);

    return 0;
}

std::time_t getTimeNow()
{
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}
