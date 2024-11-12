#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

#include "Pond.hh"

static const std::string QUACKER_BANNER = "   ____                  _             \n  /___ \\_   _  __ _  ___| | _____ _ __      __\n //  / / | | |/ _` |/ __| |/ / _ \\ '__|   <(o )___\n/ \\_/ /| |_| | (_| | (__|   <  __/ |       ( ._> /\n\\___,_\\ \\__,_|\\__,_|\\___|_|\\_\\___|_|        `---'\n";

class Quacker
{
public:
  Quacker(const std::string& db_filename);
  ~Quacker();

  void run();
  
private:
  void startPage();
  void loginPage();
  void signupPage();

  Pond pond;
  std::string _user_id;

};
