#pragma once
#include <iostream>
#include <mutex>
#include <thread>
#include <zmq.hpp>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "state.hpp"

class Command {
public:
  static Command& Instance() {
    static Command instance;
    return instance;
  }

  // Setup variables from program start
  void Setup(const std::string &match, const std::string &team, const std::string &password, const std::string &ip);

  // State monitoring
  void StartMonitorState(); /* Creates & starts thread to monitor state */
  void StopMonitorState(); /* Stops the thread monitoring state */

private:
  Command(); /* Singleton */
  Command(Command const&) = delete;
  void operator=(Command const&) = delete;

  // Helper Functions
  const std::string GenerateConnectJSON(); /* To connect to match and get token */

  // State Commands
  std::mutex running;
  std::thread monitor_thread;
  void UpdateState(); /* Infinite loop of updating state */

  // Game Commands
  void Fire(std::string tank_id);

  // Match Variables
  std::string match;
  std::string team;
  std::string password;
  std::string ip;
  std::string token; /* Returned from ZMQ on successful authentication */

  // ZMQ Variables
  zmq::context_t context;
  zmq::socket_t state_socket;
  zmq::socket_t command_socket;
};

Command::Command() : context(1), state_socket(context, ZMQ_SUB), command_socket(context, ZMQ_REQ) {
}

void Command::Setup(const std::string &_match, const std::string &_team, const std::string &_pass, const std::string &_ip) {
  match = _match;
  team = _team;
  password = _pass;
  ip = _ip;

  std::string connect_str = GenerateConnectJSON();

  // Configure ZMQ
  std::string state_addr = "tcp://" + ip + ":5556";
  std::string command_addr = "tcp://" + ip + ":5557";
  state_socket.setsockopt(ZMQ_SUBSCRIBE, match.c_str(), match.length());

  // Connect to ZMQ
  state_socket.connect(state_addr.c_str());
  command_socket.connect(command_addr.c_str());

  // Setup Game
  // Prepare
  zmq::message_t request(connect_str.length());
  memcpy(request.data(), connect_str.c_str(), connect_str.length());
  // Send
  command_socket.send(request);
  // Parse Reply
  zmq::message_t reply;
  command_socket.recv(&reply);
  std::string reply_str(static_cast<char*>(reply.data()), reply.size());
  rapidjson::Document d;
  d.Parse(reply_str.c_str());

  #ifdef NDEBUG
  std::cout << "--------------- Response ---------------" << std::endl;
  std::cout << "  " << reply_str << std::endl;
  std::cout << "----------------------------------------" << std::endl << std::endl;

  // Verify things are OK
  assert(d.HasMember("resp"));
  std::string resp = d["resp"].GetString();
  assert(resp.compare("ok") == 0);
  assert(d.HasMember("client_token"));
  #endif

  // Set Token
  token = d["client_token"].GetString();
}

const std::string Command::GenerateConnectJSON() {
  // Construct JSON
  rapidjson::Document d;
  rapidjson::Document::AllocatorType& a = d.GetAllocator();
  d.SetObject();
  d.AddMember("comm_type", "MatchConnect", a);
  d.AddMember("match_token", rapidjson::StringRef(match.c_str()), a);
  d.AddMember("team_name", rapidjson::StringRef(team.c_str()), a);
  d.AddMember("password", rapidjson::StringRef(password.c_str()), a);

  // Get JSON String
  rapidjson::StringBuffer b;
  rapidjson::Writer<rapidjson::StringBuffer> w(b);
  d.Accept(w);

  #ifdef NDEBUG
  std::cout << "Generated Connect JSON:" << std::endl;
  std::cout << b.GetString() << std::endl;
  std::cout << "----------------------------------------" << std::endl << std::endl;
  #endif

  return b.GetString();
}

void Command::Fire(std::string tank_id) {
}

void Command::UpdateState() {
  while(true) {
    // Check if we're still supposed to be running
    if(running.try_lock()) {
      // Get new state data
      zmq::message_t m;
      state_socket.recv(&m);

      // Parse what we got
      std::string d(static_cast<char*>(m.data()), m.size());
      State &s = State::Instance();
      s.Update(d);
    } else {
      // We're supposed to die
      break;
    }
    running.unlock();
  }
}

void Command::StartMonitorState() {
  monitor_thread = std::thread(&Command::UpdateState, this);
}

void Command::StopMonitorState() {
  running.lock();
  monitor_thread.join();
  running.unlock();
}
