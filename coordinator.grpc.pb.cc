// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: coordinator.proto

#include "coordinator.pb.h"
#include "coordinator.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>
namespace csce662 {

static const char* CoordService_method_names[] = {
  "/csce662.CoordService/GetSlave",
  "/csce662.CoordService/Heartbeat",
  "/csce662.CoordService/GetServer",
  "/csce662.CoordService/IsMaster",
  "/csce662.CoordService/GetAllFollowerServers",
  "/csce662.CoordService/GetFollowerServer",
};

std::unique_ptr< CoordService::Stub> CoordService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< CoordService::Stub> stub(new CoordService::Stub(channel, options));
  return stub;
}

CoordService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_GetSlave_(CoordService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Heartbeat_(CoordService_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetServer_(CoordService_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_IsMaster_(CoordService_method_names[3], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetAllFollowerServers_(CoordService_method_names[4], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetFollowerServer_(CoordService_method_names[5], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status CoordService::Stub::GetSlave(::grpc::ClientContext* context, const ::csce662::ID& request, ::csce662::ServerInfo* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce662::ID, ::csce662::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetSlave_, context, request, response);
}

void CoordService::Stub::async::GetSlave(::grpc::ClientContext* context, const ::csce662::ID* request, ::csce662::ServerInfo* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce662::ID, ::csce662::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetSlave_, context, request, response, std::move(f));
}

void CoordService::Stub::async::GetSlave(::grpc::ClientContext* context, const ::csce662::ID* request, ::csce662::ServerInfo* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetSlave_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce662::ServerInfo>* CoordService::Stub::PrepareAsyncGetSlaveRaw(::grpc::ClientContext* context, const ::csce662::ID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce662::ServerInfo, ::csce662::ID, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetSlave_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce662::ServerInfo>* CoordService::Stub::AsyncGetSlaveRaw(::grpc::ClientContext* context, const ::csce662::ID& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetSlaveRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status CoordService::Stub::Heartbeat(::grpc::ClientContext* context, const ::csce662::ServerInfo& request, ::csce662::Confirmation* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce662::ServerInfo, ::csce662::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Heartbeat_, context, request, response);
}

void CoordService::Stub::async::Heartbeat(::grpc::ClientContext* context, const ::csce662::ServerInfo* request, ::csce662::Confirmation* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce662::ServerInfo, ::csce662::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Heartbeat_, context, request, response, std::move(f));
}

void CoordService::Stub::async::Heartbeat(::grpc::ClientContext* context, const ::csce662::ServerInfo* request, ::csce662::Confirmation* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Heartbeat_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce662::Confirmation>* CoordService::Stub::PrepareAsyncHeartbeatRaw(::grpc::ClientContext* context, const ::csce662::ServerInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce662::Confirmation, ::csce662::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Heartbeat_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce662::Confirmation>* CoordService::Stub::AsyncHeartbeatRaw(::grpc::ClientContext* context, const ::csce662::ServerInfo& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncHeartbeatRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status CoordService::Stub::GetServer(::grpc::ClientContext* context, const ::csce662::ID& request, ::csce662::ServerInfo* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce662::ID, ::csce662::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetServer_, context, request, response);
}

void CoordService::Stub::async::GetServer(::grpc::ClientContext* context, const ::csce662::ID* request, ::csce662::ServerInfo* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce662::ID, ::csce662::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetServer_, context, request, response, std::move(f));
}

void CoordService::Stub::async::GetServer(::grpc::ClientContext* context, const ::csce662::ID* request, ::csce662::ServerInfo* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetServer_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce662::ServerInfo>* CoordService::Stub::PrepareAsyncGetServerRaw(::grpc::ClientContext* context, const ::csce662::ID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce662::ServerInfo, ::csce662::ID, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetServer_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce662::ServerInfo>* CoordService::Stub::AsyncGetServerRaw(::grpc::ClientContext* context, const ::csce662::ID& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetServerRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status CoordService::Stub::IsMaster(::grpc::ClientContext* context, const ::csce662::ServerInfo& request, ::csce662::Confirmation* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce662::ServerInfo, ::csce662::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_IsMaster_, context, request, response);
}

void CoordService::Stub::async::IsMaster(::grpc::ClientContext* context, const ::csce662::ServerInfo* request, ::csce662::Confirmation* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce662::ServerInfo, ::csce662::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_IsMaster_, context, request, response, std::move(f));
}

void CoordService::Stub::async::IsMaster(::grpc::ClientContext* context, const ::csce662::ServerInfo* request, ::csce662::Confirmation* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_IsMaster_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce662::Confirmation>* CoordService::Stub::PrepareAsyncIsMasterRaw(::grpc::ClientContext* context, const ::csce662::ServerInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce662::Confirmation, ::csce662::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_IsMaster_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce662::Confirmation>* CoordService::Stub::AsyncIsMasterRaw(::grpc::ClientContext* context, const ::csce662::ServerInfo& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncIsMasterRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status CoordService::Stub::GetAllFollowerServers(::grpc::ClientContext* context, const ::csce662::ID& request, ::csce662::ServerList* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce662::ID, ::csce662::ServerList, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetAllFollowerServers_, context, request, response);
}

void CoordService::Stub::async::GetAllFollowerServers(::grpc::ClientContext* context, const ::csce662::ID* request, ::csce662::ServerList* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce662::ID, ::csce662::ServerList, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetAllFollowerServers_, context, request, response, std::move(f));
}

void CoordService::Stub::async::GetAllFollowerServers(::grpc::ClientContext* context, const ::csce662::ID* request, ::csce662::ServerList* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetAllFollowerServers_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce662::ServerList>* CoordService::Stub::PrepareAsyncGetAllFollowerServersRaw(::grpc::ClientContext* context, const ::csce662::ID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce662::ServerList, ::csce662::ID, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetAllFollowerServers_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce662::ServerList>* CoordService::Stub::AsyncGetAllFollowerServersRaw(::grpc::ClientContext* context, const ::csce662::ID& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetAllFollowerServersRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status CoordService::Stub::GetFollowerServer(::grpc::ClientContext* context, const ::csce662::ID& request, ::csce662::ServerInfo* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce662::ID, ::csce662::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetFollowerServer_, context, request, response);
}

void CoordService::Stub::async::GetFollowerServer(::grpc::ClientContext* context, const ::csce662::ID* request, ::csce662::ServerInfo* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce662::ID, ::csce662::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetFollowerServer_, context, request, response, std::move(f));
}

void CoordService::Stub::async::GetFollowerServer(::grpc::ClientContext* context, const ::csce662::ID* request, ::csce662::ServerInfo* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetFollowerServer_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce662::ServerInfo>* CoordService::Stub::PrepareAsyncGetFollowerServerRaw(::grpc::ClientContext* context, const ::csce662::ID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce662::ServerInfo, ::csce662::ID, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetFollowerServer_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce662::ServerInfo>* CoordService::Stub::AsyncGetFollowerServerRaw(::grpc::ClientContext* context, const ::csce662::ID& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetFollowerServerRaw(context, request, cq);
  result->StartCall();
  return result;
}

CoordService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      CoordService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< CoordService::Service, ::csce662::ID, ::csce662::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](CoordService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce662::ID* req,
             ::csce662::ServerInfo* resp) {
               return service->GetSlave(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      CoordService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< CoordService::Service, ::csce662::ServerInfo, ::csce662::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](CoordService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce662::ServerInfo* req,
             ::csce662::Confirmation* resp) {
               return service->Heartbeat(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      CoordService_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< CoordService::Service, ::csce662::ID, ::csce662::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](CoordService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce662::ID* req,
             ::csce662::ServerInfo* resp) {
               return service->GetServer(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      CoordService_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< CoordService::Service, ::csce662::ServerInfo, ::csce662::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](CoordService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce662::ServerInfo* req,
             ::csce662::Confirmation* resp) {
               return service->IsMaster(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      CoordService_method_names[4],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< CoordService::Service, ::csce662::ID, ::csce662::ServerList, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](CoordService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce662::ID* req,
             ::csce662::ServerList* resp) {
               return service->GetAllFollowerServers(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      CoordService_method_names[5],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< CoordService::Service, ::csce662::ID, ::csce662::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](CoordService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce662::ID* req,
             ::csce662::ServerInfo* resp) {
               return service->GetFollowerServer(ctx, req, resp);
             }, this)));
}

CoordService::Service::~Service() {
}

::grpc::Status CoordService::Service::GetSlave(::grpc::ServerContext* context, const ::csce662::ID* request, ::csce662::ServerInfo* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status CoordService::Service::Heartbeat(::grpc::ServerContext* context, const ::csce662::ServerInfo* request, ::csce662::Confirmation* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status CoordService::Service::GetServer(::grpc::ServerContext* context, const ::csce662::ID* request, ::csce662::ServerInfo* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status CoordService::Service::IsMaster(::grpc::ServerContext* context, const ::csce662::ServerInfo* request, ::csce662::Confirmation* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status CoordService::Service::GetAllFollowerServers(::grpc::ServerContext* context, const ::csce662::ID* request, ::csce662::ServerList* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status CoordService::Service::GetFollowerServer(::grpc::ServerContext* context, const ::csce662::ID* request, ::csce662::ServerInfo* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


static const char* SynchService_method_names[] = {
  "/csce662.SynchService/GetAllUsers",
  "/csce662.SynchService/GetFollowersOfClient",
  "/csce662.SynchService/GetTLFL",
  "/csce662.SynchService/ResynchServer",
  "/csce662.SynchService/SynchronizerList",
};

std::unique_ptr< SynchService::Stub> SynchService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< SynchService::Stub> stub(new SynchService::Stub(channel, options));
  return stub;
}

SynchService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_GetAllUsers_(SynchService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetFollowersOfClient_(SynchService_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetTLFL_(SynchService_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_ResynchServer_(SynchService_method_names[3], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_SynchronizerList_(SynchService_method_names[4], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status SynchService::Stub::GetAllUsers(::grpc::ClientContext* context, const ::csce662::Confirmation& request, ::csce662::AllUsers* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce662::Confirmation, ::csce662::AllUsers, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetAllUsers_, context, request, response);
}

void SynchService::Stub::async::GetAllUsers(::grpc::ClientContext* context, const ::csce662::Confirmation* request, ::csce662::AllUsers* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce662::Confirmation, ::csce662::AllUsers, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetAllUsers_, context, request, response, std::move(f));
}

void SynchService::Stub::async::GetAllUsers(::grpc::ClientContext* context, const ::csce662::Confirmation* request, ::csce662::AllUsers* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetAllUsers_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce662::AllUsers>* SynchService::Stub::PrepareAsyncGetAllUsersRaw(::grpc::ClientContext* context, const ::csce662::Confirmation& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce662::AllUsers, ::csce662::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetAllUsers_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce662::AllUsers>* SynchService::Stub::AsyncGetAllUsersRaw(::grpc::ClientContext* context, const ::csce662::Confirmation& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetAllUsersRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status SynchService::Stub::GetFollowersOfClient(::grpc::ClientContext* context, const ::csce662::ID& request, ::csce662::AllUsers* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce662::ID, ::csce662::AllUsers, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetFollowersOfClient_, context, request, response);
}

void SynchService::Stub::async::GetFollowersOfClient(::grpc::ClientContext* context, const ::csce662::ID* request, ::csce662::AllUsers* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce662::ID, ::csce662::AllUsers, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetFollowersOfClient_, context, request, response, std::move(f));
}

void SynchService::Stub::async::GetFollowersOfClient(::grpc::ClientContext* context, const ::csce662::ID* request, ::csce662::AllUsers* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetFollowersOfClient_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce662::AllUsers>* SynchService::Stub::PrepareAsyncGetFollowersOfClientRaw(::grpc::ClientContext* context, const ::csce662::ID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce662::AllUsers, ::csce662::ID, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetFollowersOfClient_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce662::AllUsers>* SynchService::Stub::AsyncGetFollowersOfClientRaw(::grpc::ClientContext* context, const ::csce662::ID& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetFollowersOfClientRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status SynchService::Stub::GetTLFL(::grpc::ClientContext* context, const ::csce662::ID& request, ::csce662::TLFL* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce662::ID, ::csce662::TLFL, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetTLFL_, context, request, response);
}

void SynchService::Stub::async::GetTLFL(::grpc::ClientContext* context, const ::csce662::ID* request, ::csce662::TLFL* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce662::ID, ::csce662::TLFL, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetTLFL_, context, request, response, std::move(f));
}

void SynchService::Stub::async::GetTLFL(::grpc::ClientContext* context, const ::csce662::ID* request, ::csce662::TLFL* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetTLFL_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce662::TLFL>* SynchService::Stub::PrepareAsyncGetTLFLRaw(::grpc::ClientContext* context, const ::csce662::ID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce662::TLFL, ::csce662::ID, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetTLFL_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce662::TLFL>* SynchService::Stub::AsyncGetTLFLRaw(::grpc::ClientContext* context, const ::csce662::ID& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetTLFLRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status SynchService::Stub::ResynchServer(::grpc::ClientContext* context, const ::csce662::ServerInfo& request, ::csce662::Confirmation* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce662::ServerInfo, ::csce662::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_ResynchServer_, context, request, response);
}

void SynchService::Stub::async::ResynchServer(::grpc::ClientContext* context, const ::csce662::ServerInfo* request, ::csce662::Confirmation* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce662::ServerInfo, ::csce662::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_ResynchServer_, context, request, response, std::move(f));
}

void SynchService::Stub::async::ResynchServer(::grpc::ClientContext* context, const ::csce662::ServerInfo* request, ::csce662::Confirmation* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_ResynchServer_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce662::Confirmation>* SynchService::Stub::PrepareAsyncResynchServerRaw(::grpc::ClientContext* context, const ::csce662::ServerInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce662::Confirmation, ::csce662::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_ResynchServer_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce662::Confirmation>* SynchService::Stub::AsyncResynchServerRaw(::grpc::ClientContext* context, const ::csce662::ServerInfo& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncResynchServerRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status SynchService::Stub::SynchronizerList(::grpc::ClientContext* context, const ::csce662::ID& request, ::csce662::SynchronizerListReply* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce662::ID, ::csce662::SynchronizerListReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_SynchronizerList_, context, request, response);
}

void SynchService::Stub::async::SynchronizerList(::grpc::ClientContext* context, const ::csce662::ID* request, ::csce662::SynchronizerListReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce662::ID, ::csce662::SynchronizerListReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_SynchronizerList_, context, request, response, std::move(f));
}

void SynchService::Stub::async::SynchronizerList(::grpc::ClientContext* context, const ::csce662::ID* request, ::csce662::SynchronizerListReply* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_SynchronizerList_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce662::SynchronizerListReply>* SynchService::Stub::PrepareAsyncSynchronizerListRaw(::grpc::ClientContext* context, const ::csce662::ID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce662::SynchronizerListReply, ::csce662::ID, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_SynchronizerList_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce662::SynchronizerListReply>* SynchService::Stub::AsyncSynchronizerListRaw(::grpc::ClientContext* context, const ::csce662::ID& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncSynchronizerListRaw(context, request, cq);
  result->StartCall();
  return result;
}

SynchService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      SynchService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< SynchService::Service, ::csce662::Confirmation, ::csce662::AllUsers, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](SynchService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce662::Confirmation* req,
             ::csce662::AllUsers* resp) {
               return service->GetAllUsers(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      SynchService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< SynchService::Service, ::csce662::ID, ::csce662::AllUsers, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](SynchService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce662::ID* req,
             ::csce662::AllUsers* resp) {
               return service->GetFollowersOfClient(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      SynchService_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< SynchService::Service, ::csce662::ID, ::csce662::TLFL, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](SynchService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce662::ID* req,
             ::csce662::TLFL* resp) {
               return service->GetTLFL(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      SynchService_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< SynchService::Service, ::csce662::ServerInfo, ::csce662::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](SynchService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce662::ServerInfo* req,
             ::csce662::Confirmation* resp) {
               return service->ResynchServer(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      SynchService_method_names[4],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< SynchService::Service, ::csce662::ID, ::csce662::SynchronizerListReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](SynchService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce662::ID* req,
             ::csce662::SynchronizerListReply* resp) {
               return service->SynchronizerList(ctx, req, resp);
             }, this)));
}

SynchService::Service::~Service() {
}

::grpc::Status SynchService::Service::GetAllUsers(::grpc::ServerContext* context, const ::csce662::Confirmation* request, ::csce662::AllUsers* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status SynchService::Service::GetFollowersOfClient(::grpc::ServerContext* context, const ::csce662::ID* request, ::csce662::AllUsers* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status SynchService::Service::GetTLFL(::grpc::ServerContext* context, const ::csce662::ID* request, ::csce662::TLFL* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status SynchService::Service::ResynchServer(::grpc::ServerContext* context, const ::csce662::ServerInfo* request, ::csce662::Confirmation* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status SynchService::Service::SynchronizerList(::grpc::ServerContext* context, const ::csce662::ID* request, ::csce662::SynchronizerListReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace csce662
