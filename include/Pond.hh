#pragma once

#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <unordered_set>
#include <sstream>

#include "definitions.hh"

class Pond
{
public:
  Pond();
  ~Pond();

  /**
   * @brief Represents a Quack with various attributes.
   *
   * This struct holds data related to an individual quack, including the quack ID,
   * author ID, text content, timestamp (date and time), and any quack it replies to.
   */
  struct Quack {
    int32_t tid;
    int32_t writer_id;
    std::string text;
    std::string date;
    std::string time;
    int32_t replyto_tid;
  };

  /**
   * @brief Represents a User with a unique ID and a name.
   *
   * This struct stores basic information about a user in the system.
   */
  struct User {
    int32_t usr;
    std::string name;
  };

  /**
  * @brief Opens a connection to the SQLite database specified by the filename.
  *
  * @param db_filename The name of the database file to open.
  * @return int Returns SQLITE_OK (0) if the database was successfully opened,
  *         or a non-zero SQLite error code if it failed.
  */
  int loadDatabase(const std::string& db_filename);

  /**
  * @brief Adds a new user to the users table in the database.
  *
  * @param user_id The unique ID of the user.
  * @param name The name of the user.
  * @param email The email of the user.
  * @param phone The phone number of the user.
  * @param password The password for the user's account.
  * @return true if the user was successfully added; false otherwise.
  */
  int32_t* addUser(
    const std::string& name,
    const std::string& email,
    const int64_t& phone,
    const std::string& password
  );

  /**
  * @brief Adds a new post to the posts table in the database.
  *
  * @param quack_id The unique ID of the quack.
  * @param user_id The ID of the user who created the post.
  * @param text The text content of the post.
  * @return true if the post was successfully added; false otherwise.
  */
  bool addPost(
    const int32_t& quack_id,
    const std::int32_t& user_id,
    const std::string& text
  );

  /**
  * @brief Adds a reply quack to the quacks table in the database.
  *
  * @param user_id The ID of the user creating the reply.
  * @param reply_quack_id The ID of the quack being replied to.
  * @param text The text content of the reply.
  * @return true if the reply was successfully added; false otherwise.
  */
  bool addReply(
    const std::int32_t& user_id,
    const int32_t& reply_quack_id,
    const std::string& text
  );

  // bool requack(
  //   const std::int32_t& user_id,
  //   const int32_t& requack_quack_id,
  //   const std::string& text
  //   const bool spam
  // );

  /**
   * @brief Creates a new list for a user in the database.
   *
   * @param user_id The ID of the user who owns the list.
   * @param list_name The name of the new list.
   * @return true if the list was successfully created; false otherwise.
   */
  bool createList(
    const std::int32_t& user_id,
    const std::string& list_name
  );

  /**
   * @brief Adds a quack to a list in the database.
   *
   * @param list_id The name of the list.
   * @param quack_id The ID of the quack to add to the list.
   * @param user_id The ID of the user who owns the list.
   * @return true if the quack was successfully added to the list; false otherwise.
   */
  bool addToList(
    const std::string& list_name,
    const std::int32_t& quack_id,
    const std::int32_t& user_id
  );

  /**
  * @brief Checks if the provided user ID and password are valid for login.
  *
  * @param user_id The user ID to check in the database.
  * @param password The password corresponding to the user ID.
  * @return true if the login credentials are valid; false otherwise.
  */
  int32_t* checkLogin(
    const int32_t& user_id,
    const std::string& password
  );

  /**
   * @brief Retrieves the username associated with a given user ID from the database.
   *
   * @param user_id The unique identifier of the user whose username is being retrieved.
   * @return A std::string containing the username if found, otherwise an empty string.
   */
  std::string getUsername(
    const int32_t& user_id
  );

  /**
   * @brief Retrieves a feed of quacks and requacks for a given user.
   *
   * @param user_id The unique identifier of the user for whom the feed is generated.
   * @return A vector of strings where each string represents a formatted entry in the feed.
   */
  std::vector<std::string> getFeed(
    const int32_t& user_id
  );

  /**
   * @brief Adds a follow relationship between two users.
   *
   * @param user_id The ID of the user who is following.
   * @param follow_id The ID of the user to be followed.
   * @return true if the follow was successfully added, false otherwise.
   */
  bool follow(
    const int32_t& user_id,
    const int32_t& follow_id
  );

  /**
   * @brief Removes a follow relationship between two users.
   *
   * @param user_id The ID of the user who is unfollowing.
   * @param follow_id The ID of the user to be unfollowed.
   * @return true if the unfollow was successful, false otherwise.
   */
  bool unfollow(
    const int32_t& user_id,
    const int32_t& follow_id
  );

  bool requack(
    const int32_t& user_id,
    const int32_t& requack_quack_id,
    const bool spam
  );

  // bool report(
  //   const int32_t& user_id,
  //   const int32_t& quack_id
  // );

  /**
   * @brief Searches for users in the database whose names contain the specified search terms.
   *
   * @param search_terms The terms to search for in user names.
   * @return A vector of pairs containing user IDs and names that match the search terms.
   */
  std::vector<Pond::User> searchForUsers(
    const std::string& search_terms
  );


  /**
   * @brief search for quacks containing specific keywords or hashtags.
   *
   * @param search_terms A string of keywords or hashtags to search for in quacks.
   * @return A vector of quacks that contain the specified keywords or hashtags, ordered by date and time.
   *
   * @note case insensitive search, space seperated keywoards
   */
  std::vector<Pond::Quack> searchForQuacks(
    const std::string& search_terms
  );

private:
  sqlite3* _db;

  /**
   * @brief Finds a unique user ID that is not currently in use in the database.
   *
   * @param[out] unique_id An integer reference that will be set to a unique user ID.
   * @return `true` if a unique ID is successfully found and assigned; `false` if an error occurs.
   */
  bool getUniqueUserID(int32_t& unique_id);

  /**
   * 
   */
  bool getUnqiueQuackID(int32_t& unique_id);
  
  /**
  * @brief Retrieves the current time in GMT as a formatted string (HH:MM:SS).
  *
  * @return A string representing the current time in "HH:MM:SS" format.
  */
  char* _getTime();
  
  /**
  * @brief Retrieves the current date in GMT as a formatted string (YYYY-MM-DD).
  *
  * @return A string representing the current date in "YYYY-MM-DD" format.
  */
  char* _getDate();

  /**
   * @brief Checks if a list exists for a given user in the database.
   *
   *
   * @param list_name The name of the list to check for.
   * @param user_id The ID of the user who owns the list.
   * @return True if the list exists for the specified user, false otherwise.
   *
   * @note If there is an error preparing the SQL statement, the function will
   *       finalize the statement and return false.
   */
  bool _listExists(
    const std::string& list_name,
    const std::int32_t& user_id
  );
};
