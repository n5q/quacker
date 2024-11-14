#pragma once

#include <cstdlib>
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <sstream>
#include <algorithm>
#include <numeric>

#include "Pond.hh"

static const std::string QUACKER_BANNER  = "[38;5;44m [39m[38;5;44m [39m[38;5;44m [39m[38;5;44m_[39m[38;5;44m_[39m[38;5;44m_[39m[38;5;43m_[39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;48m [39m[38;5;48m [39m[38;5;48m [39m[38;5;48m [39m[38;5;48m [39m[38;5;48m [39m[38;5;48m [39m[38;5;48m [39m[38;5;48m [39m[38;5;84m_[39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;119m [39m[38;5;118m [39m[38;5;118m[39m\n"
"[38;5;44m [39m[38;5;44m [39m[38;5;44m/[39m[38;5;43m_[39m[38;5;49m_[39m[38;5;49m_[39m[38;5;49m [39m[38;5;49m\\[39m[38;5;49m_[39m[38;5;49m [39m[38;5;49m [39m[38;5;49m [39m[38;5;49m_[39m[38;5;48m [39m[38;5;48m [39m[38;5;48m_[39m[38;5;48m_[39m[38;5;48m [39m[38;5;48m_[39m[38;5;48m [39m[38;5;48m [39m[38;5;48m_[39m[38;5;84m_[39m[38;5;83m_[39m[38;5;83m|[39m[38;5;83m [39m[38;5;83m|[39m[38;5;83m [39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m [39m[38;5;119m_[39m[38;5;118m [39m[38;5;118m_[39m[38;5;118m_[39m[38;5;118m [39m[38;5;118m [39m[38;5;118m [39m[38;5;118m [39m[38;5;118m [39m[38;5;118m [39m[38;5;154m_[39m[38;5;154m_[39m[38;5;154m[39m\n"
"[38;5;43m [39m[38;5;49m/[39m[38;5;49m/[39m[38;5;49m [39m[38;5;49m [39m[38;5;49m/[39m[38;5;49m [39m[38;5;49m/[39m[38;5;49m [39m[38;5;49m|[39m[38;5;48m [39m[38;5;48m|[39m[38;5;48m [39m[38;5;48m|[39m[38;5;48m/[39m[38;5;48m [39m[38;5;48m_[39m[38;5;48m`[39m[38;5;48m [39m[38;5;84m|[39m[38;5;83m/[39m[38;5;83m [39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m|[39m[38;5;83m [39m[38;5;83m|[39m[38;5;83m/[39m[38;5;83m [39m[38;5;83m/[39m[38;5;83m [39m[38;5;119m_[39m[38;5;118m [39m[38;5;118m\\[39m[38;5;118m [39m[38;5;118m'[39m[38;5;118m_[39m[38;5;118m_[39m[38;5;118m|[39m[38;5;118m [39m[38;5;118m [39m[38;5;154m [39m[38;5;154m<[39m[38;5;154m([39m[38;5;154mo[39m[38;5;154m [39m[38;5;154m)[39m[38;5;154m_[39m[38;5;154m_[39m[38;5;154m_[39m[38;5;154m[39m\n"
"[38;5;49m/[39m[38;5;49m [39m[38;5;49m\\[39m[38;5;49m_[39m[38;5;49m/[39m[38;5;49m [39m[38;5;49m/[39m[38;5;48m|[39m[38;5;48m [39m[38;5;48m|[39m[38;5;48m_[39m[38;5;48m|[39m[38;5;48m [39m[38;5;48m|[39m[38;5;48m [39m[38;5;48m([39m[38;5;84m_[39m[38;5;83m|[39m[38;5;83m [39m[38;5;83m|[39m[38;5;83m [39m[38;5;83m([39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m|[39m[38;5;83m [39m[38;5;83m [39m[38;5;83m [39m[38;5;119m<[39m[38;5;118m [39m[38;5;118m [39m[38;5;118m_[39m[38;5;118m_[39m[38;5;118m/[39m[38;5;118m [39m[38;5;118m|[39m[38;5;118m [39m[38;5;118m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m([39m[38;5;154m [39m[38;5;154m.[39m[38;5;154m_[39m[38;5;154m>[39m[38;5;148m [39m[38;5;184m/[39m[38;5;184m[39m\n"
"[38;5;49m\\[39m[38;5;49m_[39m[38;5;49m_[39m[38;5;49m_[39m[38;5;48m,[39m[38;5;48m_[39m[38;5;48m\\[39m[38;5;48m [39m[38;5;48m\\[39m[38;5;48m_[39m[38;5;48m_[39m[38;5;48m,[39m[38;5;48m_[39m[38;5;84m|[39m[38;5;83m\\[39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m,[39m[38;5;83m_[39m[38;5;83m|[39m[38;5;83m\\[39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m_[39m[38;5;83m|[39m[38;5;119m_[39m[38;5;118m|[39m[38;5;118m\\[39m[38;5;118m_[39m[38;5;118m\\[39m[38;5;118m_[39m[38;5;118m_[39m[38;5;118m_[39m[38;5;118m|[39m[38;5;118m_[39m[38;5;154m|[39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m [39m[38;5;154m`[39m[38;5;148m-[39m[38;5;184m-[39m[38;5;184m-[39m[38;5;184m'[39m[38;5;184m[39m\n"
"[38;5;49m[39m";

class Quacker
{
public:
  Quacker(const std::string& db_filename);
  ~Quacker();

  std::string trim(const std::string& str);

  void run();
  
private:
  void startPage();

  /**
   * @brief Displays the login page and prompts the user for credentials.
   * 
   * @note The credentials are verified using the `checkLogin` method, and if successful,
   *       the `_user_id` member is updated with the user ID.
   */
  void loginPage();

  /**
   * @brief Displays the sign-up page and prompts the user for details to create a new account.
   * 
   * @note The function utilizes `isValidEmail` for email validation, `isValidPhoneNumber` for phone number validation,
   *       and `pond.addUser` to add the new user to the database.
   */
  void signupPage();

  
  void mainPage();

  /**
   * @brief Displays the user search page and prompts the user to enter a search term.
   *
   * The search term should be a portion of a user's name. The search results
   * are displayed to the user 5 at a time, showing matching users.
   */
  void userSearchPage();

  /**
   * @brief Displays the quack search page and prompts the user to enter a search term.
   *
   * The search term should be a keyword or a hashtag prefixed with '#'. The search results
   * are displayed to the user 5 at a time, showing matching users.
   */
  void quackSearchPage();

  void replyMenu(const int32_t& user_id, const Pond::Quack& reply);
  
  void quackMenu(const int32_t& user_id, const Pond::Quack& reply);
  
  std::string processFeed(const std::int32_t& user_id, int32_t& FeedDisplayCount, std::string& error);

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

  Pond pond;
  int32_t* _user_id = nullptr;
  bool logged_in = false;
};
