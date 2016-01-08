#pragma once
#include <cassert>
#include <thread>
#include <zmq.hpp>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "state.hpp"
using namespace rapidjson;

class Command {
public:
  

  static Command& Instance() {
    static Command instance;
    return instance;
  }
  

  Command(Command const&) = delete;
  void operator=(Command const&) = delete;

  void MonitorState();
  void Connect();

private:
  Command(string &_team, string &_password, string &_match_id, string &_ip);
  string GenerateConnectJSON();
  void UpdateState();

  string team;
  string password;
  string match_id;
  string ip;

  string client_token;

  zmq::context_t context;
  zmq::socket_t state_socket;
  zmq::socket_t command_socket;
  
};

Command::Command(string &_team, string &_password, string &_match_id, string &_ip) : team(_team), password(_password), match_id(_match_id), ip(_ip),
context(1), state_socket(context, ZMQ_SUB), command_socket(context, ZMQ_REQ) {
  // Create String to Connect
  // TODO: Seperate port numbers as literals
  string state = "tcp://" + ip + ":5556";
  string command = "tcp://" + ip + ":5557";

  // Configure
  // Filter only messages related to the current match
  state_socket.setsockopt(ZMQ_SUBSCRIBE, _match_id.c_str(), _match_id.length());

  // Connect
  state_socket.connect(state.c_str());
  command_socket.connect(command.c_str());
}

string Command::GenerateConnectJSON() {
  // Construct JSON
  Document d;
  Document::AllocatorType& allocator = d.GetAllocator();
  d.SetObject();
  d.AddMember("comm_type", "MatchConnect", allocator);
  d.AddMember("match_token", StringRef(match_id.c_str()), allocator);
  d.AddMember("team_name", StringRef(team.c_str()), allocator);
  d.AddMember("password", StringRef(password.c_str()), allocator);

  // Get JSON String
  StringBuffer buf;
  Writer<StringBuffer> writer(buf);
  d.Accept(writer);
  return buf.GetString();
}

void Command::Connect() {
  string connectJSON = GenerateConnectJSON();

  // Generate the request
  zmq::message_t request(connectJSON.length());
  memcpy(request.data(), connectJSON.c_str(), connectJSON.length());

  // Send the request
  command_socket.send(request);

  // Get the reply
  zmq::message_t reply;
  command_socket.recv(&reply);

  Document d;
  d.Parse(static_cast<char*>(reply.data()));

  cout << "--------------- Response ---------------" << endl;
  cout << "  " << static_cast<char*>(reply.data()) << endl;
  cout << "----------------------------------------" << endl << endl;

  // Verify things are okay
  assert(d.HasMember("resp"));
  string resp = d["resp"].GetString();
  assert(resp.compare("ok") == 0);
  assert(d.HasMember("comm_type"));
  assert(d.HasMember("client_token"));

  // Store client token
  client_token = d["client_token"].GetString();
}

void Command::UpdateState() {
  while(true) {
    // Get new state data
    zmq::message_t state;
    state_socket.recv(&state);

    // Parse what we got
    string data(static_cast<char*>(state.data()), state.size());
    State& s = State::Instance();
    s.ParseJSON(data, match_id);
  }
}

void Command::MonitorState() {
  // Start the monitoring thread!
  std::thread monitor_thread(&Command::UpdateState, this);
  monitor_thread.detach();
}
