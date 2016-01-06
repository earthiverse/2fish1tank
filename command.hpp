#pragma once
#include <cassert>
#include <mutex>
#include <thread>
#include <zmq.hpp>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "state.hpp"
using namespace rapidjson;

class Command {
private:
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

  std::mutex mutex;
public:
  Command(string &_team, string &_password, string &_match_id, string &_ip);
  void MonitorState();
  void Connect();
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
  // Lock!
  std::lock_guard<std::mutex> guard(mutex);

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
    Document d;
    d.Parse(data.c_str());

    if(d.IsObject()) {
      // Got Match Data (JSON)
      // Parse what we've received
      string comm_type = d["comm_type"].GetString();
      std::lock_guard<std::mutex> guard(Command);
      if(comm_type.compare("GAME_START") == 0) {
        cout << "Starting Game!" << endl;
      } else if(comm_type.compare("GAMESTATE") == 0) {
        // TODO: Update State
//        cout << "Game In Progress!" << endl;
//        cout << "  Time Remaining: " << d["timeRemaining"].GetDouble() << "s" << endl;
      } else if(comm_type.compare("GAME_END") == 0) {
        cout << "Game Ending!" << endl;

        // TODO: Check if there is any more games left. If there is none, stop playing.
      } else {
        // Unknown Command
        cout << "comm_type: " << comm_type << endl;
      }
    } else {
      // Got Match ID (GUID)
      assert(match_id.compare(data) == 0);
      continue;
    }

  }
}

void Command::MonitorState() {
  // Start the monitoring thread!
  std::thread monitor_thread(&Command::UpdateState, this);
  monitor_thread.detach();
}
