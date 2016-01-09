#include <iostream>
#include <boost/program_options.hpp>

#include "command.hpp"
#include "state.hpp"
#include "tankmanager.hpp"

void ShowUsage(const boost::program_options::options_description &desc) {
  std::cout << desc << std::endl;
}

int main(int argc, char* argv[]) {
  // Program options handled by Boost
  boost::program_options::options_description desc("2fish1tank options");
  desc.add_options()
    ("help,h", "Display this program usage")
    ("match,m", boost::program_options::value<std::string>(), "Match ID")
    ("team,t", boost::program_options::value<std::string>(), "Team Name")
    ("password,p", boost::program_options::value<std::string>(), "Team Password")
    ("ip,i", boost::program_options::value<std::string>(), "Competition Server IP");

  boost::program_options::variables_map args;
  boost::program_options::store(boost::program_options::command_line_parser(argc, argv)
    .options(desc)
    .style(boost::program_options::command_line_style::default_style
      | boost::program_options::command_line_style::allow_long_disguise)
    .run(), args);
  boost::program_options::notify(args);

  if(args.count("help")) {
    ShowUsage(desc);
    return 0;
  } else if(!args.count("match") || !args.count("team") || !args.count("password") || !args.count("ip")) {
    // Not enough arguments to run
    ShowUsage(desc);
    return 1;
  }

  std::string match = args["match"].as<std::string>();
  std::string team = args["team"].as<std::string>();
  std::string password = args["password"].as<std::string>();
  std::string ip = args["ip"].as<std::string>();

  // Setup game
  try {
    Command &command = Command::Instance();
    command.Setup(match, team, password, ip);

    // Start monitoring state
    State &state = State::Instance();

    TankManager tankmanager;

    while(true) {
      // Update State
      state.Update();

      // Act on the updated state
      #ifdef NDEBUG
  //    std::cout << "Game is running..." << std::endl;
      #endif
      tankmanager.Act();
      usleep(10000);
    }
  } catch (const std::exception &e) {
    std::cout << "!!!!! Caught Exception!" << std::endl;
    std::cout << e.what() << std::endl;
  }

  return 0;
}
