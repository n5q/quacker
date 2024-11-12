#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

static const std::string QUACKER_BANNER = "   ____                  _             \n  /___ \\_   _  __ _  ___| | _____ _ __      __\n //  / / | | |/ _` |/ __| |/ / _ \\ '__|   <(o )___\n/ \\_/ /| |_| | (_| | (__|   <  __/ |       ( ._> /\n\\___,_\\ \\__,_|\\__,_|\\___|_|\\_\\___|_|        `---'\n";

class Quacker
{
public:
  Quacker();
  ~Quacker();

  void run();
  
private:
  void startPage();
  void loginPage();
  void signupPage();
  std::string _user_id;

};
