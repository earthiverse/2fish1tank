#pragma once
#include <iostream>
#include <mutex>
#include <thread>
#include <zmq.hpp>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

enum Direction { FWD, REV };
enum Rotation { CW, CCW };

class Command {
public:
  static Command& Instance() {
    static Command instance;
    return instance;
  }

  // Setup variables from program start
  void Setup(const std::string &match, const std::string &team, const std::string &password, const std::string &ip);

  // State
  std::string GetStateJSON();

  // Game Commands
  void Fire(const std::string &tank_id);
  void Move(const std::string &tank_id, const Direction &dir, const double &amt);
  void Rotate(const std::string &tank_id, const Rotation &rot, const double &rad);
  void RotateTurret(const std::string &tank_id, const Rotation &rot, const double &rad);

  // Getters
  const std::string &GetTeamName();

private:
  Command(); /* Singleton */
  Command(Command const&) = delete;
  void operator=(Command const&) = delete;

  // Helper Functions
  std::string SendCommand(const std::string &json);
  const std::string GenerateConnectJSON(); /* To connect to match and get token */

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

  // Configure ZMQ
  std::string state_addr = "tcp://" + ip + ":5556";
  std::string command_addr = "tcp://" + ip + ":5557";
  state_socket.setsockopt(ZMQ_SUBSCRIBE, match.c_str(), match.length());

  // Connect to ZMQ
  state_socket.connect(state_addr.c_str());
  command_socket.connect(command_addr.c_str());

  // Setup Game
  std::string reply_str = SendCommand(GenerateConnectJSON());
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
  rapidjson::Document::AllocatorType &a = d.GetAllocator();
  d.SetObject();
  d.AddMember("comm_type", "MatchConnect", a);
  d.AddMember("match_token", rapidjson::StringRef(match.c_str()), a);
  d.AddMember("team_name", rapidjson::StringRef(team.c_str()), a);
  d.AddMember("password", rapidjson::StringRef(password.c_str()), a);

  // Get JSON String
  rapidjson::StringBuffer b;
  rapidjson::Writer<rapidjson::StringBuffer> w(b);
  d.Accept(w);
  std::string json = b.GetString();

  #ifdef NDEBUG
  std::cout << "Generated Connect JSON:" << std::endl;
  std::cout << json << std::endl;
  std::cout << "----------------------------------------" << std::endl << std::endl;
  #endif

  return json;
}

void Command::Fire(const std::string &tank_id) {
  // Construct JSON
  rapidjson::Document d;
  rapidjson::Document::AllocatorType &a = d.GetAllocator();
  d.SetObject();
  d.AddMember("tank_id", rapidjson::StringRef(tank_id.c_str()), a);
  d.AddMember("comm_type", "FIRE", a);
  d.AddMember("client_token", rapidjson::StringRef(token.c_str()), a);

  // Get JSON String
  rapidjson::StringBuffer b;
  rapidjson::Writer<rapidjson::StringBuffer> w(b);
  d.Accept(w);
  std::string json = b.GetString();

  // Send Command
}

std::string Command::GetStateJSON() {
  // Get new state data
  zmq::message_t m;
  state_socket.recv(&m);

  // Parse what we got
  return std::string(static_cast<char*>(m.data()), m.size());
}

std::string Command::SendCommand(const std::string &json) {
  // Prepare
  zmq::message_t request(json.length());
  memcpy(request.data(), json.c_str(), json.length());

  // Send
  command_socket.send(request);

  // Return Response
  zmq::message_t reply;
  command_socket.recv(&reply);
  return std::string(static_cast<char*>(reply.data()), reply.size());
}

const std::string &Command::GetTeamName() {
  return team;
}
