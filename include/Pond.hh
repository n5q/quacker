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
  * @param tweet_id The unique ID of the tweet.
  * @param user_id The ID of the user who created the post.
  * @param text The text content of the post.
  * @return true if the post was successfully added; false otherwise.
  */
  bool addPost(
    const int32_t& tweet_id,
    const std::int32_t& user_id,
    const std::string& text
  );

  /**
  * @brief Adds a reply tweet to the tweets table in the database.
  *
  * @param user_id The ID of the user creating the reply.
  * @param reply_tweet_id The ID of the tweet being replied to.
  * @param text The text content of the reply.
  * @return true if the reply was successfully added; false otherwise.
  */
  bool addReply(
    const std::int32_t& user_id,
    const int32_t& reply_tweet_id,
    const std::string& text
  );

  // bool retweet(
  //   const std::int32_t& user_id,
  //   const int32_t& retweet_tweet_id,
  //   const std::string& text
  //   const bool spam
  // );

  bool createList(
    const std::int32_t& user_id,
    const std::string& list_name
  );

  bool addToList(
    const std::string& list_name,
    const std::int32_t& tweet_id,
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
    const std::int32_t& user_id,
    const std::string& password
  );
  
  std::string getUsername(
    const std::int32_t& user_id
  );

  // std::vector<std::string> getFeed(
  //   const std::int32_t& user_id
  // );

  bool follow(
    const std::int32_t& user_id,
    const std::int32_t& follow_id
  );

  bool unfollow(
    const std::int32_t& user_id,
    const std::int32_t& follow_id
  );

  struct Tweet {
    int32_t tid;
    int32_t writer_id;
    std::string text;
    std::string date;
    std::string time;
    int32_t replyto_tid;
  };

  std::vector<Tweet> searchForTweets(
    const std::string& search_terms
  );

  
  std::vector<std::pair<std::int32_t, std::string>> searchForUsers(
    const std::string& search_terms
  );

  // std::vector<std::pair<std::string, std::string>> searchForUsers(
  //   const std::string& keyword
  // );

  bool get_unique_user_id(int32_t& unique_id);

private:
  sqlite3* _db;

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

  
  bool _checkTweetId(
    int32_t tweet_id
  );

  bool _listExists(
    const std::string& list_name,
    const std::int32_t& user_id
  );

  int32_t generateUniqueTweetID();
};
