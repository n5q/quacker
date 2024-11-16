#pragma once

#include <cstdlib>
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <termios.h>
#include <unistd.h>

#include "Pond.hh"

static const std::string QUACKER_BANNER  = "[38;5;44m [39m[38;5;44m [39m[38;5;44m [39m[38;5;44m_[39m[38;5;44m_[39m[38;5;44m_[39m[38;5;43m_[39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;48m [39m[38;5;48m [39m[38;5;48m [39m[38;5;48m [39m[38;5;48m [39m[38;5;48m [39m[38;5;48m [39m[38;5;48m [39m[38;5;48m [39m[38;5;84m_[39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;119m [39m[38;5;118m [39m[38;5;118m[39m\n"
"[38;5;44m [39m[38;5;44m [39m[38;5;44m/[39m[38;5;43m_[39m[38;5;49m_[39m[38;5;49m_[39m[38;5;49m [39m[38;5;49m\\[39m[38;5;49m_[39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;49m_[39m[38;5;48m [39m[38;5;48m [39m[38;5;48m_[39m[38;5;48m_[39m[38;5;48m [39m[38;5;48m_[39m[38;5;48m [39m[38;5;48m [39m[38;5;48m_[39m[38;5;84m_[39m[38;5;83m_[39m[38;5;83m|[39m[38;5;83m [39m[38;5;83m|[39m[38;5;83m [39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m [39m[38;5;119m_[39m[38;5;118m [39m[38;5;118m_[39m[38;5;118m_[39m[38;5;118m [39m[38;5;118m [39m[38;5;118m [39m[38;5;118m [39m[38;5;118m [39m[38;5;118m [39m[38;5;154m_[39m[38;5;154m_[39m[38;5;154m[39m\n"
"[38;5;43m [39m[38;5;49m/[39m[38;5;49m/[39m[38;5;49m [39m[38;5;49m [39m[38;5;49m/[39m[38;5;49m [39m[38;5;49m/[39m[38;5;49m [39m[38;5;49m|[39m[38;5;48m [39m[38;5;48m|[39m[38;5;48m [39m[38;5;48m|[39m[38;5;48m/[39m[38;5;48m [39m[38;5;48m_[39m[38;5;48m`[39m[38;5;48m [39m[38;5;84m|[39m[38;5;83m/[39m[38;5;83m [39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m|[39m[38;5;83m [39m[38;5;83m|[39m[38;5;83m/[39m[38;5;83m [39m[38;5;83m/[39m[38;5;83m [39m[38;5;119m_[39m[38;5;118m [39m[38;5;118m\\[39m[38;5;118m [39m[38;5;118m'[39m[38;5;118m_[39m[38;5;118m_[39m[38;5;118m|[39m[38;5;118m [39m[38;5;118m [39m[38;5;154m [39m[38;5;154m<[39m[38;5;154m([39m[38;5;154mo[39m[38;5;154m [39m[38;5;154m)[39m[38;5;154m_[39m[38;5;154m_[39m[38;5;154m_[39m[38;5;154m[39m\n"
"[38;5;49m/[39m[38;5;49m [39m[38;5;49m\\[39m[38;5;49m_[39m[38;5;49m/[39m[38;5;49m [39m[38;5;49m/[39m[38;5;48m|[39m[38;5;48m [39m[38;5;48m|[39m[38;5;48m_[39m[38;5;48m|[39m[38;5;48m [39m[38;5;48m|[39m[38;5;48m [39m[38;5;48m([39m[38;5;84m_[39m[38;5;83m|[39m[38;5;83m [39m[38;5;83m|[39m[38;5;83m [39m[38;5;83m([39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m|[39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;119m<[39m[38;5;118m [39m[38;5;118m [39m[38;5;118m_[39m[38;5;118m_[39m[38;5;118m/[39m[38;5;118m [39m[38;5;118m|[39m[38;5;118m [39m[38;5;118m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m([39m[38;5;154m [39m[38;5;154m.[39m[38;5;154m_[39m[38;5;154m>[39m[38;5;148m [39m[38;5;184m/[39m[38;5;184m[39m\n"
"[38;5;49m\\[39m[38;5;49m_[39m[38;5;49m_[39m[38;5;49m_[39m[38;5;48m,[39m[38;5;48m_[39m[38;5;48m\\[39m[38;5;48m [39m[38;5;48m\\[39m[38;5;48m_[39m[38;5;48m_[39m[38;5;48m,[39m[38;5;48m_[39m[38;5;84m|[39m[38;5;83m\\[39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m,[39m[38;5;83m_[39m[38;5;83m|[39m[38;5;83m\\[39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m|[39m[38;5;119m_[39m[38;5;118m|[39m[38;5;118m\\[39m[38;5;118m_[39m[38;5;118m\\[39m[38;5;118m_[39m[38;5;118m_[39m[38;5;118m_[39m[38;5;118m|[39m[38;5;118m_[39m[38;5;154m|[39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m`[39m[38;5;148m-[39m[38;5;184m-[39m[38;5;184m-[39m[38;5;184m'[39m[38;5;184m[39m\n"
"[38;5;49m[39m";

/**
 * @class Quacker
 * @brief Manages the user interface and interaction for the Quacker application.
 *
 * The `Quacker` class handles user input, navigation between various pages, and 
 * communication with the `Pond` class for database operations. It serves as the 
 * frontend logic for the application, managing features like login, sign-up, posting 
 * quacks, and searching users or quacks.
 *
 * ### Features:
 * - Login and sign-up functionality.
 * - Posting and replying to quacks.
 * - Searching users and quacks with pagination.
 * - Validating user input for email, phone numbers, and IDs.
 */
class Quacker
{
public:
  /**
   * @brief Constructs a Quacker object and attempts to load the database.
   *
   * This constructor initializes the Quacker object and attempts to load
   * the database from the specified file. If the database cannot be loaded,
   * an error message is printed to `std::cerr`, and the program exits with
   * a status code of ERROR_SQL.
   *
   * @param db_filename The name of the database file to load.
   *
   * @note Ensure that the provided `db_filename` points to a valid and
   * accessible database file to prevent the program from terminating.
   */
  Quacker(const std::string& db_filename);

  /**
   * @brief Destructor for the Quacker class.
   *
   * This destructor clears the console by executing the `clear` system command 
   * and releases the memory allocated for the `_user_id` member variable.
   */
  ~Quacker();

  /**
   * @brief Initiates the main execution flow of the Quacker application.
   *
   * This method serves as the entry point for running the Quacker application.
   * It begins by calling the `startPage()` method, which likely handles
   * the initial setup or user interface for the application.
   */
  void run();
  
private:

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
   */
  void startPage();
  
  /**
   * @brief Displays the login interface and handles user authentication.
   *
   * This method provides a user interface for logging into the Quacker application.
   * Users can enter their login credentials (User ID and Password) or press Enter to return
   * to the previous menu. It validates the input, checks the login credentials, and either
   * allows the user to proceed or prompts for corrections.
   */
  void loginPage();

  /**
   * @brief Handles user registration by collecting and validating input.
   *
   * This method provides an interface for new users to sign up by entering their
   * name, email, phone number, and password. It validates the input and adds the 
   * user to the database upon successful registration.
   *
   * @details
   * - Ensures the email follows a valid format and the phone number is properly structured.
   * - Safely captures and stores the password.
   * - Attempts to register the user and provides feedback if the process encounters errors.
   * - Automatically logs the user in after successful registration.
   */
  void signupPage();

      
  /**
   * @brief Manages the main user interface for logged-in users.
   *
   * This method serves as the central hub for logged-in users, providing access 
   * to features such as viewing and interacting with their feed, searching for 
   * users or posts, creating new posts, and logging out.
   *
   * @details
   * - Displays the user feed and adjusts the number of visible posts based on user selection.
   * - Validates user input to ensure actions correspond to available menu options.
   * - Provides options for replying to or retweeting posts directly from the feed.
   * - Handles logging out by cleaning up the session and redirecting to the start page.
   */
  void mainPage();

  /**
   * @brief Allows the user to compose and post a new Quack.
   *
   * This method provides a user interface for creating and submitting a new Quack. 
   * Users can input their text or press Enter to exit without posting.
   *
   * @details
   * - Validates the input to ensure it is not empty before attempting to post.
   * - Attempts to post the Quack using the database. If successful, the user is notified.
   * - Handles errors during posting, such as issues with duplicate hashtags, and provides feedback.
   */
  void postingPage();

  /**
   * @brief Provides a user interface for searching and interacting with users.
   *
   * This method allows users to search for other users by name and displays the results.
   * Users can view more or fewer results, select a user to follow, or exit the search interface.
   *
   * @details
   * - Retrieves and displays search results based on the input query.
   * - Allows users to interact with search results by selecting a user to view or follow.
   * - Handles navigation through paginated search results.
   */
  void searchUsersPage();


  /**
   * @brief Provides a user interface for searching and interacting with Quacks.
   *
   * This method allows users to search for Quacks using keywords or hashtags and 
   * displays the results. Users can navigate through results, reply to or requack 
   * posts, or return to the main menu.
   *
   * @details
   * - Retrieves and displays search results based on the entered search term.
   * - Supports pagination for viewing more or fewer Quacks in the results.
   * - Allows users to interact with Quacks by replying or requacking.
   * - Validates user input for result navigation and Quack interaction to ensure proper behavior.
   */
  void searchQuacksPage();

  /**
   * @brief Displays a detailed user profile and allows interactions with the user's content.
   *
   * This method provides an interface to view a user's profile, including their information, 
   * quacks, and follower statistics. Users can navigate through the profile, view more or fewer 
   * quacks, follow the user, or return to the previous menu.
   *
   * @details
   * - Displays the selected user's profile details, including name, follower count, and quack count.
   * - Fetches and displays the user's quacks, with pagination to show more or fewer quacks.
   * - Provides an option to follow the user, with validation to prevent self-following or duplicate follows.
   */
  void userPage(const Pond::User& user);

  /**
   * @brief Provides an interface for replying to a specific Quack.
   *
   * This method allows a user to write and post a reply to a given Quack. The 
   * details of the Quack being replied to are displayed, and the user can submit 
   * their reply or cancel the action.
   *
   * @details
   * - Displays the original Quack details, including text, author, and timestamps.
   * - Allows users to compose a reply, which is then added to the database.
   * - Handles errors during reply submission and provides appropriate feedback.
   * - Users can exit the reply interface by pressing Enter without entering text.
   */
  void replyPage(const int32_t& user_id, const Pond::Quack& reply);

  /**
   * @brief Provides an interface for interacting with a specific Quack.
   *
   * This method allows a user to view details about a Quack and take actions such as 
   * replying, requacking, or returning to the previous menu.
   *
   * @details
   * - Displays detailed information about the Quack, including its author, content, and metadata.
   * - Users can reply to the Quack, which redirects to the reply interface.
   * - Users can requack the post, with validation to prevent duplicate requacks.
   * - Handles errors during requacking and provides feedback.
   * - Allows users to exit the interface by selecting the return option.
   */
  void quackPage(const int32_t& user_id, const Pond::Quack& reply);

  void followersPage();
  
  
  std::string processFeed(const std::int32_t& user_id, int32_t& FeedDisplayCount, std::string& error, int32_t& i);

  std::string getHiddenPassword();

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
  int64_t isValidPhoneNumber(const std::string& input);

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
  bool isValidEmail(const std::string& email);

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
  bool isID(std::string str);

  std::string formatTweetText(const std::string& text, int lineWidth);

  int32_t extractQuackID(const std::string& quackString);

  std::string trim(const std::string& str);
  
  Pond pond;
  int32_t* _user_id = nullptr;
  bool logged_in = false;
  std::vector<int32_t> feed_quack_ids;

};
