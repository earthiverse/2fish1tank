#include <iostream>
#include <boost/program_options.hpp>
#include "bot.hpp"
#include "command.hpp"
namespace po = boost::program_options;
using namespace std;

int main(int argc, char* argv[]) {
  // Command line argument parsing provided by boost!
  po::options_description desc("2fish1tank options");
  desc.add_options()
    ("help", "Show Program Usage")
    ("match", po::value<string>(), "Match ID")
    ("team", po::value<string>(), "Team Name")
    ("password", po::value<string>(), "Team Password")
    ("ip", po::value<string>(), "Server IP")
  ;
  po::variables_map args;
  po::store(po::command_line_parser(argc, argv)
    .options(desc)
    .style(po::command_line_style::default_style | po::command_line_style::allow_long_disguise)
    .run(), args);
  po::notify(args);

  if(args.size() < 4 || args.count("help")) {
    // Show help
    cout << desc << endl;
    return 0;
  }

  string match = args["match"].as<string>();
  string team = args["team"].as<string>();
  string password = args["password"].as<string>();
  string ip = args["ip"].as<string>();

  // Display the configuration
  cout << "Launching 2fish1tank..." << endl;
  cout << "------------ Configuration -------------" << endl;
  cout << "  Match ID: " << match << endl;
  cout << "  Team Name: " << team << endl;
  cout << "  Team Password: " << password << endl;
  cout << "  Server IP: " << ip << endl;
  cout << "----------------------------------------" << endl << endl;

  // Connect
  cout << "Connecting to '" << ip << "'..." << endl;
  Command command(team, password, match, ip);
  command.Connect();

  // Start Monitoring Game State
  cout << "Starting State Monitor..." << endl;
  State state;
  command.MonitorState();

  // Start Bot
  while(true) {
    // TODO: Get State
    cout << "Doing Bot Stuff" << endl;

    sleep(10);
  }
  return 0;
}
