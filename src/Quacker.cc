#include "Quacker.hh"


Quacker::Quacker(const std::string& db_filename) {
  if (pond.loadDatabase(db_filename)) {
    std::cerr << "Database Error: Could Not Open" << db_filename << std::endl;
    exit(-3);
  }
}

Quacker::~Quacker() {
  std::system("clear");
  delete _user_id;
}

void Quacker::run() {
  startPage();
}

/**
 * @brief Displays the main start page for the Quacker application and prompts user actions.
 *
 * This function continually displays the main start page menu until the user logs in or exits.
 * While `_user_id` remains `nullptr`, the menu provides options to log in, sign up, or exit the program.
 * Each option triggers the corresponding page or action.
 *
 * The menu options include:
 * - **1. Log in**: Opens the login page.
 * - **2. Sign up**: Opens the signup page.
 * - **3. Exit**: Terminates the program.
 *
 * @note The screen is cleared before each menu display to keep the interface clean.
 */
void Quacker::startPage() {
  std::string error = "";
  while (this->_user_id == nullptr) {
    std::system("clear");

    char select;
    std::cout << QUACKER_BANNER << error << "\n1. Log in\n2. Sign up\n3. Exit\n\nSelection: ";
    std::cin >> select;
    
    if (std::cin.peek() != '\n') select = '0';
    // Consume any trailing '\n' and discard it
    { std::string dummy; std::getline(std::cin, dummy); }
    switch (select) {
      case '1':
        error = "";
        loginPage();
        break;
      case '2':
        error = "";
        signupPage();
        
        break;
      case '3':
        std::system("clear");
        error = "";
        exit(0);
        break;

      default:
        error = "\nInvalid Input Entered [use: 1, 2, 3]\n";
        break;
    }
  }
}

std::string Quacker::getHiddenPassword() {
  struct termios oldt, newt;
  std::string password;
  char ch;

  // save old terminal settings and disable echo
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ECHO); // disable echo
  newt.c_lflag &= ~(ICANON); // disable canonical mode 
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  while (true) {
    ch = getchar();
    if (ch == '\n') { 
      std::cout << std::endl;
      break;
    }
    else if (ch == 127 || ch == '\b') { // backspace
      if (!password.empty()) {
        password.pop_back();
        std::cout << "\b \b"; 
      }
    }
    else {
      password.push_back(ch);
      std::cout << '*'; 
    }
  }

  // restore old terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return password;
}

/**
 * @brief Displays the login page and prompts the user for credentials.
 * 
 * @note The credentials are verified using the `checkLogin` method, and if successful,
 *       the `_user_id` member is updated with the user ID.
 */
void Quacker::loginPage() {
  std::string description = "Enter login credentials or press Enter to return.";

  while (true) {
    // Clear the screen and show the login interface
    std::system("clear");
    std::cout << QUACKER_BANNER << "\n" << description << "\n\n--- Log In ---\n" << "\nUser ID: ";

    std::string user_id_str;
    int32_t user_id;
    std::string password;

    // Get the user ID input from the user
    std::getline(std::cin, user_id_str);
    if (user_id_str.empty()) {
      return;
    } else if (isID(user_id_str)) {
      try {
        user_id = std::stoi(user_id_str);
      } catch (const std::out_of_range&) {
        description = "Invalid User ID, ID must be a valid integer.";
        continue;
      }
    } else {
      description = "Invalid User ID, ID must be a valid integer.";
      continue;
    }

    // Ask for the password
    std::cout << "Password: ";
    password = this->getHiddenPassword();
    // std::getline(std::cin, password);

    // Check login credentials
    this->_user_id = pond.checkLogin(user_id, password);

    // If credentials are invalid, prompt the user to try again
    if (_user_id == nullptr) {
      description = "Invalid credentials, please enter a valid 'User ID' and 'Password', or press Enter to return.";
      continue;
    }
    break;
  }
  logged_in = true;
  mainPage();
}

/**
 * @brief Displays the sign-up page and prompts the user for details to create a new account.
 * 
 * @note The function utilizes `isValidEmail` for email validation, `isValidPhoneNumber` for phone number validation,
 *       and `pond.addUser` to add the new user to the database.
 */
void Quacker::signupPage() {
  std::string description = "Enter your details or press Enter to return... ";
  while (true) {
    // Clear the screen and show the sign-up interface
    std::system("clear");
    std::cout << QUACKER_BANNER << "\n" << description << "\n\n--- Sign Up ---\n";

    std::string name, email, phone_str, password;
    long int phone_number;

    // Get and validate the name
    std::cout << "Enter Name: ";
    std::getline(std::cin, name);
    name = trim(name);
    if (name.empty()) return;

    // Get and validate the email
    std::cout << "Enter Email: ";
    std::getline(std::cin, email);
    if (email.empty()) return;
    if (!isValidEmail(email)) {
      description = "Invalid email format, please try again [eg. example@ualberta.ca]";
      continue;
    }

    // Get and validate the phone number
    std::cout << "Enter Phone Number: ";
    std::getline(std::cin, phone_str);
    if (phone_str.empty()) return;
    phone_number = isValidPhoneNumber(phone_str);
    if (phone_number == -1) {
      description = "Invalid phone number format, please try again [eg. 510-827-7791].";
      continue;
    }

    // Get and validate the password
    std::cout << "Enter Password: ";
    password = this->getHiddenPassword();
    // std::getline(std::cin, password);
    if (password.empty()) return;

    // Add user to the database
    int32_t* new_user_id = pond.addUser(name, email, phone_number, password);
    
    // If the user is successfully added, assign the new user ID to _user_id and notify the user
    if (new_user_id != nullptr) {
      this->_user_id = new_user_id;
      std::cout << "Account created! Press Enter to log in... ";
      std::cin.get();
      logged_in = true;
      mainPage();
      break;
    } else {
        description = "Error during signup, please try again.\n";
    }
  }
}

void Quacker::mainPage() {
  std::string error = "";
  int32_t FeedDisplayCount = 5;
  while (logged_in) {
    std::system("clear");
    
    std::string username = pond.getUsername(*(this->_user_id));

    int32_t i = 1;
    char select;
    std::cout << QUACKER_BANNER << "\nWelcome back, " << username 
    << "! (User Id: " << *(this->_user_id) << ")\n\n-------------------------------------------- Your Feed ---------------------------------------------\n";
    std::cout << processFeed(*(this->_user_id), FeedDisplayCount, error, i);
    std::cout << "\n" << error << "\n\n1. See More Of My Feed\n"
                                      "2. See Less Of My Feed\n"
                                      "3. Search For Users\n"
                                      "4. Search For Quacks\n"
                                      "5. Reply/Retweet From Feed\n"
                                      "6. List Followers\n"
                                      "7. CREATE NEW POST\n"
                                      "8. Log Out\n"
                                      "Selection: ";
    std::cin >> select;
    if (std::cin.peek() != '\n') select = '0';
    // Consume any trailing '\n' and discard it
    { std::string dummy; std::getline(std::cin, dummy); }
    switch (select) {
      case '1':
        FeedDisplayCount += 5;
        error = "";
        break;

      case '2':
        FeedDisplayCount -= 5;
        error = "";
        break;

      case '3':
        this->searchUsersPage();
        error = "";
        break;

      case '4':
        this->searchQuacksPage();
        error = "";
        break;

      case '5': {
          std::cout << "\nSelect a tweet (1,2,3,...) to reply/retweet or press Enter to return... ";
          std::string input;
          std::getline(std::cin, input);
          if (input.empty()) {
            break;
          }
          else {
            bool valid_input = false;
            while(!valid_input){
              std::regex positive_integer_regex("^[1-9]\\d*$");
              if (!std::regex_match(input, positive_integer_regex)) {
                  std::cout << "\033[A\033[2K" << std::flush;
                  std::cout << "Input Is Invalid: Select a tweet (1,2,3,...) to reply/retweet OR press Enter to return... ";
                  std::getline(std::cin, input);
                  if (input.empty()) {
                    break;
                  }
                  continue;
              }

              int32_t selection = std::stoi(input)-1;
              if (selection > static_cast<int32_t>(i-2) || selection < static_cast<int32_t>(i-6)) {
                  std::cout << "\033[A\033[2K" << std::flush;
                  std::cout << "Input Is Invalid: Select a tweet (1,2,3,...) to reply/retweet OR press Enter to return... ";
                  std::getline(std::cin, input);
                  if (input.empty()) {
                    break;
                  }
                  continue;
              }
              valid_input = true;

              if (valid_input) {
                this->quackPage(*(this->_user_id), pond.getQuackFromID(this->feed_quack_ids[selection]));
              }
              break;
            }
            break;
          }
        }
      
      case '6':
        this->followersPage();
      break;

      case '7':
        postingPage();
        break;

      case '8':
        std::system("clear");
        FeedDisplayCount = 5;
        error = "";
        logged_in = false;
        delete this->_user_id;
        this->_user_id = nullptr;
        break;

      default:
        error = "\nInvalid Input Entered [use: 1, 2, 3, ..., 9].\n";
        break;
    }
  }
  
  startPage();
}

void Quacker::postingPage() {
  std::system("clear");
  std::string description = "Type your new Quack or press Enter to return.";
  std::string quack_text;
  while (true) {
    std::system("clear");
    std::cout << QUACKER_BANNER << "\n" << description << "\n\n--- New Quack ---\n";
    std::cout << "Enter your new quack: ";
    std::getline(std::cin, quack_text);
    quack_text = trim(quack_text);
    if (quack_text.empty()) {
      break;
    }
    if (pond.addQuack(*(this->_user_id), quack_text) != nullptr) {
      std::cout << "Quack posted successfully!\n";
      std::cout << "Press Enter to return... ";
      std::string input;
      std::getline(std::cin, input);
      while (!input.empty()) {
        std::cout << "\033[A\033[2K" << std::flush;
        std::cout << "Input Is Invalid: Press Enter to return... ";
        std::getline(std::cin, input);
      }
    } 
    else {
      description = "Error posting Quack, insure there are no duplicate hashtags and try again.";
    }
  }
}

/**
 * @brief Displays the user search page and prompts the user to enter a search term.
 *
 * The search term should be a portion of a user's name. The search results
 * are displayed to the user 5 at a time, showing matching users.
*/
void Quacker::searchUsersPage() {
  std::string description = "Search for a user or press Enter to return.";
  while (true) {
    // show search interface
    std::system("clear");
    std::cout << QUACKER_BANNER << "\n" << description << "\n\n--- User Search ---\n";

    std::string search_term;
    std::cout << "Search for user name: ";
    std::getline(std::cin, search_term);
    search_term = trim(search_term);
    if (search_term.empty()) return;

    // query
    std::vector<Pond::User> results = pond.searchForUsers(search_term);

    // display results
    if (results.empty()) {
      std::cout << "No users found matching the search term.\n";
      std::cout << '\n' << '\n';
      std::cout << "Press Enter to return... ";
      std::string input;
      std::getline(std::cin, input);
      while (!input.empty()) {
        std::cout << "\033[A\033[2K" << std::flush;
        std::cout << "Input Is Invalid: Press Enter to return... ";
        std::getline(std::cin, input);
      }
    } else {
      uint32_t i = 1;

      std::cout << "Found " << results.size() << " users matching the search term.\n\n";

      for (const Pond::User& result : results) {
        std::ostringstream oss;

        oss << "----------------------------------------------------------------------------------------------------\n";
        oss << i << ".\n";
        oss << "  User ID: " << std::setw(40) << std::left << result.usr
            << "Name: " << result.name << "\n\n";
        ++i;
        std::cout << oss.str();
      } std::cout << "----------------------------------------------------------------------------------------------------\n\n";

    // Prompt the user to search again or return
    std::cout << "Select a user (1,2,3,...) to follow OR press Enter to return: ";
    std::string input;
    std::getline(std::cin, input);
    if (input.empty()) {
      continue;
    }
    else {
      bool valid_input = false;
      while (!valid_input) {
        std::regex positive_integer_regex("^[1-9]\\d*$");
        if (!std::regex_match(input, positive_integer_regex)) {
          std::cout << "\033[A\033[2K" << std::flush;
          std::cout << "Input Is Invalid: Select a user (1,2,3,...) to follow OR press Enter to return: ";
          std::getline(std::cin, input);
          if (input.empty()) {
            break;
          }
          continue;
        }

        uint32_t selection = std::stoi(input) - 1;
        if (selection > i - 2) {
          std::cout << "\033[A\033[2K" << std::flush;
          std::cout << "Input Is Invalid: Select a user (1,2,3,...) to follow OR press Enter to return: ";
          std::getline(std::cin, input);
          if (input.empty()) {
            break;
          }
          continue;
        }
        valid_input = true;
        
        if (selection <= results.size()) {
          this->userPage(results[selection]);
        }
        break;
      }  
    }
  }
  }
}

 void Quacker::userPage(const Pond::User& user) {
  int32_t user_id = *(this->_user_id);
  std::string error = "";
  int32_t hardstop = 3;
  while (true) {
    int32_t i = 1;
    std::system("clear");
    char select;
    std::cout << QUACKER_BANNER;
    std::cout << "\nActions For User:\n\n";
    std::ostringstream oss;
    oss << "----------------------------------------------------------------------------------------------------\n";
    oss << "  User ID: " << std::setw(40) << std::left << user.usr
        << "Name: " << user.name << "\n";
    oss << "  Followers: " << std::setw(38) << std::left << pond.getFollowers(user.usr).size()
        << "Follows: " << pond.getFollows(user.usr).size() << "\n  Quack Count: " << pond.getQuacks(user.usr).size() << "\n\n";
    std::cout << oss.str();
    std::cout << "------------------------------------------- User's Quacks ------------------------------------------\n\n";
    
    std::vector<Pond::Quack> users_quacks = pond.getQuacks(user.usr);

    for (const Pond::Quack& result : users_quacks) {
        if(i > hardstop) break;
        std::ostringstream oss;
        
        oss << i << ".\n";
        oss << "Quack ID: " << result.tid;
        oss << ", Author: " << ((pond.getUsername(result.writer_id) != "") ? pond.getUsername(result.writer_id) : "Unknown");
        oss << std::string(69 - oss.str().length(), ' ');
        oss << "Date and Time: " << (result.date.empty() ? "Unknown" : result.date);
        oss << " " << (result.time.empty() ? "Unknown" : result.time) << "\n\n";
        oss << "Text: " << formatTweetText(result.text, 94) << "\n";

        oss << "\n";
        for(int i = 0; i < 100; ++i) oss << '-'; 
        oss << '\n';
        std::cout << oss.str();

        ++i;
      }

    std::cout << error <<
      "\n\n1. See More Of The Users Quacks"
      "\n2. See Less Of The Users Quacks"
      "\n3. Follow The User"
      "\n4. Return"
      "\n\nSelection: ";
    std::cin >> select;
    if (std::cin.peek() != '\n') select = '0';
    // Consume any trailing '\n' and discard it
    { std::string dummy; std::getline(std::cin, dummy); }
    switch (select) {
      case '1':
        error = "";
        hardstop += 3;
        if (static_cast<long unsigned int>(hardstop) > users_quacks.size() + 3){
          error = "\nThis User Has No More Quacks To Diplay!";
          hardstop -= 3;
          break;
        }
        break;
      case '2':
        error = "";
        if (hardstop == 0){
          error = "You Are Already Not Seeing Any Quacks!";
          break;
        }
        hardstop -= 3;
        break;
      case '3': 
        {
          error = "";
          bool already_follows = false;
          for (int32_t flws : pond.getFollows(user_id)) {
            if (flws == user.usr || user_id == user.usr) { 
              if (flws == user.usr) std::cout << "You already follow " << user.name << "\n";
              if (user_id == user.usr) std::cout << "You can't follow yourself " << user.name << "\n";
              std::cout << "Press Enter to return... ";
              std::string input;
              std::getline(std::cin, input);
              while (!input.empty()) {
                std::cout << "\033[A\033[2K" << std::flush;
                std::cout << "Input Is Invalid: Press Enter to return... ";
                std::getline(std::cin, input);
              }
              already_follows = true;
              break;
            }
          }
          if (!already_follows) {
            pond.follow(user_id, user.usr);
            std::cout << "You are now following " << user.name << "\n";
            std::cout << "Press Enter to return... ";
            std::string input;
            std::getline(std::cin, input);
            while (!input.empty()) {
              std::cout << "\033[A\033[2K" << std::flush;
              std::cout << "Input Is Invalid: Press Enter to return... ";
              std::getline(std::cin, input);
            }
          }
        }
        break;
      case '4':
        error = "";
        return;
      default:
        error = "\nInvalid Input Entered [use: 1, 2, 3].\n";
        break;
    }
  }
}
/**
 * @brief Displays the quack search page and prompts the user to enter a search term.
 *
 * The search term should be a keyword or a hashtag prefixed with '#'. The search results
 * are displayed to the user 5 at a time, showing matching users.
 */
void Quacker::searchQuacksPage() {
  std::string description = "Search for a keyword or hashtag, or press Enter to return... ";
  while (true) {
    // show search interface
    std::system("clear");
    std::cout << QUACKER_BANNER << "\n" << description << "\n\n--- Quack Search ---\n";

    std::string search_term;
    std::cout << "Search for a Quack: ";
    std::getline(std::cin, search_term);
    search_term = trim(search_term);
    if (search_term.empty()) return;

    // query
    std::vector<Pond::Quack> results = pond.searchForQuacks(search_term);
   
    
    // display results
    if (results.empty()) {
      std::cout << "No Quacks found matching the search term.\n";
      std::cout << '\n' << '\n';
      std::cout << "Press Enter to return... ";
      std::string input;
      std::getline(std::cin, input);
      while (!input.empty()) {
        std::cout << "\033[A\033[2K" << std::flush;
        std::cout << "Input Is Invalid: Press Enter to return... ";
        std::getline(std::cin, input);
      }
    }
    else {
      uint32_t i = 1;

      std::cout << "Found " << results.size() << " Quacks matching the search term.\n";
      std::cout << '\n';
      for(int i = 0; i < 100; ++i) std::cout << '-';
      std::cout << '\n';
      
      for (const Pond::Quack& result : results) {
        std::ostringstream oss;
        
        oss << i << ".\n";
        oss << "Quack ID: " << result.tid;
        oss << ", Author: " << ((pond.getUsername(result.writer_id) != "") ? pond.getUsername(result.writer_id) : "Unknown");
        oss << std::string(69 - oss.str().length(), ' ');
        oss << "Date and Time: " << (result.date.empty() ? "Unknown" : result.date);
        oss << " " << (result.time.empty() ? "Unknown" : result.time) << "\n\n";
        oss << "Text: " << formatTweetText(result.text, 94) << "\n";
        // if (result.replyto_tid != 0) {
        //     oss << "Reply to Quack ID: " << result.replyto_tid << "\n";
        // }
        oss << "\n";
        for(int i = 0; i < 100; ++i) oss << '-'; 
        oss << '\n';
        std::cout << oss.str();

        ++i;
      }
      std::cout << '\n';

      // Prompt the user to search again or return
      std::cout << "Select a tweet (1,2,3,...) to reply/retweet OR press Enter to return... ";
      std::string input;
      std::getline(std::cin, input);
      if (input.empty()) {
        continue;
      }
      else {
        bool valid_input = false;
        while(!valid_input){
          std::regex positive_integer_regex("^[1-9]\\d*$");
          if (!std::regex_match(input, positive_integer_regex)) {
              std::cout << "\033[A\033[2K" << std::flush;
              std::cout << "Input Is Invalid: Select a tweet (1,2,3,...) to reply/retweet OR press Enter to return... ";
              std::getline(std::cin, input);
              if (input.empty()) {
                break;
              }
              continue;
          }

          uint32_t selection = std::stoi(input)-1;
          if (selection > i-2) {
              std::cout << "\033[A\033[2K" << std::flush;
              std::cout << "Input Is Invalid: Select a tweet (1,2,3,...) to reply/retweet OR press Enter to return... ";
              std::getline(std::cin, input);
              if (input.empty()) {
                break;
              }
              continue;
          }
          valid_input = true;
        
          if (selection <= results.size()) {
            this->quackPage(*(this->_user_id), results[selection]);
          }
          break;
        }
      }
    }
  }
}

void Quacker::replyPage(const int32_t& user_id, const Pond::Quack& reply) {
  std::string error = "";
  while (true) {
    std::system("clear");
    std::cout << QUACKER_BANNER;
    std::cout << "\nReply For Quack:\n\n";
    
    for(int i = 0; i < 100; ++i) std::cout << '-';
    std::ostringstream oss;
    
    oss << "\nQuack ID: " << reply.tid;
    oss << ", Author: " << ((pond.getUsername(reply.writer_id) != "") ? pond.getUsername(reply.writer_id) : "Unknown");
    oss << std::string(67 - oss.str().length(), ' ');
    oss << "Date and Time: " << (reply.date.empty() ? "Unknown" : reply.date);
    oss << " " << (reply.time.empty() ? "Unknown" : reply.time) << "\n\n";
    oss << "Text: " << formatTweetText(reply.text, 94) << "\n\n";
    oss << "Requack Count: " << pond.getRequackCount(reply.tid) << "     Reply Count: " << pond.getReplies(reply.tid).size() << "\n\n";

    std::cout << oss.str();
    
    for(int i = 0; i < 100; ++i) std::cout << '-';

    std::string reply_text;
    std::cout << "\n\nEnter your reply or press Enter to cancel: ";
    std::getline(std::cin, reply_text);
    if (reply_text.empty()) return;
    if (pond.addReply(user_id, reply.tid, reply_text)) {
      std::cout << "\nReply posted successfully!\n";
      std::cout << "Press Enter to return... ";
      std::string input;
      std::getline(std::cin, input);
      while (!input.empty()) {
        std::cout << "\033[A\033[2K" << std::flush;
        std::cout << "Input Is Invalid: Press Enter to return... ";
        std::getline(std::cin, input);
      } if(input.empty()) break;
    }
    else {
      error = "Error posting reply, please try again.";
    }
  }
}

void Quacker::quackPage(const int32_t& user_id, const Pond::Quack& reply) {
  std::string error = "";
  while (true) {
    std::system("clear");
    char select;
    std::cout << QUACKER_BANNER;
    std::cout << "\nActions For Quack:\n\n";
    
    for(int i = 0; i < 100; ++i) std::cout << '-';
    std::ostringstream oss;
    
    oss << "\nQuack ID: " << reply.tid;
    oss << ", Author: " << ((pond.getUsername(reply.writer_id) != "") ? pond.getUsername(reply.writer_id) : "Unknown");
    oss << std::string(67 - oss.str().length(), ' ');
    oss << "Date and Time: " << (reply.date.empty() ? "Unknown" : reply.date);
    oss << " " << (reply.time.empty() ? "Unknown" : reply.time) << "\n\n";
    oss << "Text: " << formatTweetText(reply.text, 94) << "\n\n";
    oss << "Requack Count: " << pond.getRequackCount(reply.tid) << "     Reply Count: " << pond.getReplies(reply.tid).size() << "\n\n";

    std::cout << oss.str();
    
    for(int i = 0; i < 100; ++i) std::cout << '-';

    std::cout << error <<
      "\n\n1. Reply"
      "\n2. Requack"
      "\n3. Return"
      "\n\nSelection: ";
    std::cin >> select;
    if (std::cin.peek() != '\n') select = '0';
    // Consume any trailing '\n' and discard it
    { std::string dummy; std::getline(std::cin, dummy); }
    switch (select) {
      case '1':
        error = "";
        this->replyPage(user_id, reply);
        break;
      case '2': {
        error = "";
        int32_t joebiden = pond.addRequack(user_id, reply.tid);
        if (joebiden == 0) {
          std::cout << "Requack successful!\n";
          std::cout << "Press Enter to return... ";
          std::string input;
          std::getline(std::cin, input);
          while (!input.empty()) {
            std::cout << "\033[A\033[2K" << std::flush;
            std::cout << "Input Is Invalid: Press Enter to return... ";
            std::getline(std::cin, input);
          }
        }
        else if (joebiden == 1) {
          error = "\n\nYou've already requacked this, marked as spam...\n";
        }
        else{
          error = "\n\nError requacking, please try again.\n";
        }
        break;
    }
      case '3':
        error = "";
        return;
      default:
        error = "\n\nInvalid Input Entered [use: 1, 2, 3].\n";
        break;
    }
  }
}

void Quacker::followersPage() {
  std::string error = "";
  while (true) {
    std::system("clear");
    std::vector<Pond::User> results = pond.getFollowers(*(this->_user_id));
    std::cout << QUACKER_BANNER;
    std::cout << "\nYour Followers:\n\n";
    uint32_t i = 1;
    for (const Pond::User& flwer : results) {
      std::ostringstream oss;
      oss << "----------------------------------------------------------------------------------------------------\n";
      oss << i << ".   User ID: " << std::setw(40) << std::left << flwer.usr
        << "Name: " << flwer.name << "\n";
      std::cout << oss.str();
      ++i;
    }
    std::cout << "----------------------------------------------------------------------------------------------------\n\n";

    std::cout << "Select a user (1,2,3,...) OR press Enter to return: ";
    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) {
      return;
    }

    bool valid_input = false;
    while (!valid_input) {
      std::regex positive_integer_regex("^[1-9]\\d*$");
      if (!std::regex_match(input, positive_integer_regex)) {
        std::cout << "\033[A\033[2K" << std::flush;
        std::cout << "Input Is Invalid: Select a user (1,2,3,...) OR press Enter to return: ";
        std::getline(std::cin, input);

        if (input.empty()) {
          return; 
        }
        continue;
      }

      uint32_t selection = std::stoi(input) - 1;
      if (selection > i - 2) {
        std::cout << "\033[A\033[2K" << std::flush;
        std::cout << "Input Is Invalid: Select a user (1,2,3,...) OR press Enter to return: ";
        std::getline(std::cin, input);

        if (input.empty()) {
          return; 
        }
        continue;
      }
      valid_input = true;

      if (selection < results.size()) {
        this->userPage(results[selection]);
      }
      break;
    }
  }
}


std::string Quacker::processFeed(const std::int32_t& user_id, int32_t& FeedDisplayCount, std::string& error, int32_t& i) {
    std::vector<std::string> feed = pond.getFeed(user_id);

    int32_t maxQuacks = feed.size();
    i = 1;
    this->feed_quack_ids.clear();
    if (FeedDisplayCount >= maxQuacks + 5) {
        // Case 1: FeedDisplayCount is 5 or more beyond the available quacks
        error = "\nYou Have No More Quacks Left To Display.\n";
        FeedDisplayCount = std::max(0, static_cast<int>(FeedDisplayCount) - 5);
        std::ostringstream oss;
        while(i-1 < maxQuacks) {
            this->feed_quack_ids.push_back(extractQuackID(feed[i-1]));
            ++i;
            if((maxQuacks-5) >= i-1) continue;;
            oss << i-1 << ".\n";
            oss << feed[i-2] << "\n";
            for(int i = 0; i < 100; ++i) oss << '-'; 
            oss << '\n';
        }
        return oss.str();
    } else if (FeedDisplayCount >= maxQuacks && FeedDisplayCount <= maxQuacks + 4) {
        // Case 2: FeedDisplayCount is between maxQuacks and maxQuacks + 4
        // Do not modify FeedDisplayCount, but limit the display to maxQuacks
    } else if (FeedDisplayCount < maxQuacks && FeedDisplayCount > 0) {
        // Case 3: FeedDisplayCount is between 0 and maxQuacks
        // FeedDisplayCount remains as is
    } else if (FeedDisplayCount <= 0) {
        // Case 4: FeedDisplayCount is less than zero
        
        if(FeedDisplayCount != 0) error = "\nYou Are Already Not Displaying Any Quacks.\n";
        FeedDisplayCount = 0;
        return "";
    }

    int32_t displayCount = std::min(FeedDisplayCount, maxQuacks);

    std::ostringstream oss;
    while(i-1 < displayCount) {
        this->feed_quack_ids.push_back(extractQuackID(feed[i-1]));
        ++i;
        if((displayCount-5) >= i-1) continue;;
        oss << i-1 << ".\n";
        oss << feed[i-2] << "\n";
        for(int i = 0; i < 100; ++i) oss << '-'; 
        oss << '\n';
    }

    return oss.str();
}

/**
 * @brief Validates a phone number string and returns its numeric value.
 *
 * Extracts digits from the input string and checks if it contains 10 or 11 digits,
 * excluding any newline or carriage return characters. Returns the numeric value
 * if valid, otherwise returns -1.
 *
 * @param input The phone number string to validate.
 * @return The numeric value of the phone number if valid, -1 if invalid.
 */
int64_t Quacker::isValidPhoneNumber(const std::string& input) {
  if (input.find('\n') != std::string::npos || input.find('\r') != std::string::npos) {
    return -1;
  }

  std::string digits;

  for (char c : input) {
    if (std::isdigit(c)) {
      digits += c;
    }
  }

  if (digits.size() < 10 || digits.size() > 11) {
    return -1;
  }

  try {
    return std::stoll(digits);
  } catch (const std::out_of_range&) {
    return -1;
  }
}

/**
 * @brief Validates an email address format.
 *
 * This function checks if the given email string adheres to a standard email
 * format and ensures it does not contain any newline or carriage return
 * characters, which are considered invalid.
 *
 * @param email The email address to validate.
 * @return true if the email address is in a valid format, false otherwise.
 *
 * @note The regex used is sourced from https://emailregex.com/
 */
bool Quacker::isValidEmail(const std::string& email) {
  if (email.find('\n') != std::string::npos || email.find('\r') != std::string::npos) {
    return false;
  }

  const std::regex email_pattern(R"((?:[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*|"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\]))");
  return std::regex_match(email, email_pattern);
}

/**
 * @brief Determines if a string is a valid integer ID.
 *
 * If the entire
 * string is successfully parsed as an integer (i.e., no non-numeric
 * characters remain), it is considered a valid ID.
 *
 * @param str The string to check.
 * @return true if the string is a valid integer, false otherwise.
 */
bool Quacker::isID(std::string str) {
  char* p;
  strtol(str.c_str(), &p, 10);
  return *p == 0;
}

/**
 * @brief Trims leading and trailing whitespace from a string.
 * 
 * @param str The string to be trimmed.
 * @return A new string with leading and trailing whitespace removed.
 * 
 * @note This function does not modify the original string but instead returns a new string with the whitespace removed.
 */
std::string Quacker::trim(const std::string& str) {
  // Find the first non-whitespace character from the start
  auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
      return std::isspace(ch);
  });

  // Find the last non-whitespace character from the end
  auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
      return std::isspace(ch);
  }).base();

  // Return the trimmed string
  return (start < end) ? std::string(start, end) : std::string();
}

std::string Quacker::formatTweetText(const std::string& text, int lineWidth) {
    std::istringstream words(text);  // Stream to split text into words
    std::string word;
    std::ostringstream formattedText;
    int currentLineLength = 0;

    while (words >> word) {
        if (currentLineLength + word.length() + 1 > static_cast<std::string::size_type>(lineWidth)) {
            formattedText << "\n";
            currentLineLength = 0;
        }

        if (currentLineLength > 0) {
            formattedText << " ";
            currentLineLength++;
        }

        formattedText << word;
        currentLineLength += word.length();
    }

    return formattedText.str();
}

int32_t Quacker::extractQuackID(const std::string& quackString) {
    const std::string prefix = "Quack Id: ";
    
    if (quackString.find(prefix) == 0) {
        std::regex pattern(R"(^Quack Id:\s+(\d+))");
        std::smatch match;

        if (std::regex_search(quackString, match, pattern)) {
            return static_cast<int32_t>(std::stoi(match[1].str()));
        } else {
            std::cerr << "Error: No valid integer found after 'Quack Id: '" << std::endl;
            return -1;
        }
    } else {
        std::cerr << "Error: String does not start with 'Quack Id: '" << std::endl;
        return -1;
    }
}