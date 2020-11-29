#include <memory>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include "hellostreamingworld.grpc.pb.h"

using std::chrono::system_clock;

using grpc::Server;
using grpc::ServerAsyncWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;
using hellostreamingworld::HelloRequest;
using hellostreamingworld::HelloReply;
using hellostreamingworld::MultiGreeter;

class ServerImpl final {
public:
  ~ServerImpl() {
    server_->Shutdown();
    call_cq_->Shutdown();
  }

  void Run() {
    std::string server_address("0.0.0.0:50051");

    ServerBuilder builder;
    builder.SetOption(grpc::MakeChannelArgumentOption(GRPC_ARG_MAX_CONCURRENT_STREAMS, 10));
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service_);

    call_cq_ = builder.AddCompletionQueue();
    server_ = builder.BuildAndStart();
    std::cout << "Server listening on " << server_address << std::endl;

    HandleRpcs();
  }

private:
  class CallData {


  public:
    CallData(MultiGreeter::AsyncService *service, ServerCompletionQueue *cq, int client_id)
        : service_(service), call_cq(cq), responder_(&ctx_), status_(CREATE), times_(0), client_id_(client_id) {
      std::cout << "Created CallData " << client_id_ << std::endl;
      Proceed();
    }

    void Proceed() {
      if (status_ == CREATE) {
        status_ = PROCESS;
        service_->RequestsayHello(&ctx_, &request_, &responder_, call_cq, call_cq, this);
      } else if (status_ == PROCESS) {
        std::cout << "Client being processed:  " << request_.name() << "; client_id: " << client_id_ << std::endl;
        if (times_ == 0) {
          new CallData(service_, call_cq, client_id_ + 1);
        }

        if (times_++ >= 20) {
          status_ = FINISH;
          responder_.Finish(Status::OK, this);
        } else {
          std::string prefix("Hello ");
          reply_.set_message(prefix + request_.name() + ", no " + request_.num_greetings());

          // For illustrating queue-to-front behaviour
          using namespace std::chrono_literals;
          std::this_thread::sleep_for(1s);

          std::string stu(3 * 1024 * 1024, 'a');
          reply_.set_message(stu);
          responder_.Write(reply_, this);
        }
      } else {
        std::cout << "delete call_data: " << client_id_ << std::endl;
        GPR_ASSERT(status_ == FINISH);
        delete this;
      }
    }

    void Stop() {
      std::cerr << "Finishing up client " << client_id_ << std::endl;
      status_ = CallStatus::FINISH;
    }

  private:
    MultiGreeter::AsyncService *service_;
    ServerCompletionQueue *call_cq;
    ServerContext ctx_;

    HelloRequest request_;
    HelloReply reply_;

    ServerAsyncWriter<HelloReply> responder_;

    int times_;
    int client_id_;

    enum CallStatus {
      CREATE,
      PROCESS,
      FINISH
    };
    CallStatus status_; // The current serving state.
  };


  void HandleRpcs() {
    new CallData(&service_, call_cq_.get(), num_clients_++);
    void *tag; // uniquely identifies a request.
    bool ok;
    while (true) {
      GPR_ASSERT(call_cq_->Next(&tag, &ok));
      if (!ok) {
        static_cast<CallData *>(tag)->Stop();
        continue;
      }
      static_cast<CallData *>(tag)->Proceed();
    }
  }

  int num_clients_ = 0;
  std::unique_ptr<ServerCompletionQueue> call_cq_;
  MultiGreeter::AsyncService service_;
  std::unique_ptr<Server> server_;
};


int main(int argc, char **argv) {
  ServerImpl server;
  server.Run();

  return 0;
}
