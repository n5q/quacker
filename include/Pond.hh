#pragma once

#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <unordered_set>
#include <sstream>
#include <algorithm>

#include "definitions.hh"

/**
 * @class Pond
 * @brief A class to manage and interact with a social media-style database system.
 *
 * The Pond class serves as the central component for managing users, quacks (posts),
 * interactions such as following, replying, and liking, and general database queries.
 * It provides methods to add, retrieve, and manipulate data related to users, quacks, hashtags,
 * lists, and interactions.
 *
 * ### Features:
 * - Manage users with functions for adding, retrieving, and authenticating.
 * - Handle quacks, including creation, replies, requacks, and searching by content or hashtags.
 * - Manage lists of quacks for users.
 * - Enable user interactions such as following, unfollowing, and feed generation.
 *
 * The class interacts with an SQLite database to persistently store and retrieve data.
 * It ensures proper validation of data and handles unique ID generation for users and quacks.
 */
class Pond
{
public:

  /**
   * @brief Constructs a new Pond object.
   *
   * Initializes the database connection pointer to `nullptr` to ensure a safe 
   * and uninitialized state before the database is loaded.
   *
   * @note The database connection is not established in the constructor. 
   *       Use the `loadDatabase` method to open a database connection.
   */
  Pond();

  /**
   * @brief Destructs the Pond object and releases resources.
   *
   * Closes the SQLite database connection if it was opened, ensuring proper 
   * cleanup of resources when the Pond object goes out of scope.
   *
   * @note If the database connection was never opened (i.e., `_db` is `nullptr`),
   *       this method safely does nothing.
   */
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
   * @brief Adds a hashtag to the hashtag_mentions table in the database.
   *
   * This method associates a hashtag with a specific quack in the database. If the hashtag 
   * is already linked to the given quack (case-insensitively), no duplicate entry will be created.
   *
   * @param quack_id The unique ID of the quack to which the hashtag is being added.
   * @param hashtag The hashtag term to associate with the quack.
   * @return true if the hashtag was successfully added; false if an error occurred or 
   *         the hashtag already exists for the quack.
   *
   * @note Ensures case-insensitive uniqueness of hashtags for the specified quack.
   */
  bool addHashtag(
    const int32_t &quack_id, const std::string &hashtag
  );

  /**
   * @brief Validates a quack's content and processes its hashtags.
   *
   * This method ensures the text of a quack is non-empty and processes any hashtags within 
   * the text. A quack can contain multiple hashtags, but duplicate hashtags (case-insensitive) 
   * are not allowed. If the text contains valid hashtags, they are added to the 
   * `hashtag_mentions` table in the database.
   *
   * @param quack_id The unique ID of the quack being validated.
   * @param text The text content of the quack to validate and process.
   * @return true if the quack is valid (non-empty text and no duplicate hashtags); 
   *         false otherwise.
   *
   * @note The method converts all hashtags to lowercase for consistent storage and validation.
   *       It uses the `addHashtag` method to store valid hashtags in the database.
   */
  bool validateQuack(
    const int32_t &quack_id, const std::string &text
  );

  /**
   * @brief Adds a new quack to the database.
   *
   * @param user_id The ID of the user who is posting the quack.
   * @param text The text of the quack.
   * @return A pointer to the unique ID of the quack if it was successfully added; nullptr otherwise.
   */
  int32_t* addQuack(
    const int32_t& user_id,
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
  int32_t* addReply(
    const int32_t& user_id,
    const int32_t& reply_quack_id,
    const std::string& text
  );

  /**
   * @brief Adds a requack (retweet) for a specific quack by a user.
   *
   * This method checks if the user has already requacked the given quack. If the requack 
   * already exists, it updates the entry to mark it as spam. Otherwise, it adds a new 
   * requack entry to the database.
   *
   * @param user_id The unique ID of the user performing the requack.
   * @param quack_id The unique ID of the quack being requacked.
   * @return An integer status code:
   *         - 0: A new requack was successfully added.
   *         - 1: The requack already exists and was marked as spam.
   *         - 3: An error occurred during the process.
   *
   * @note The method uses parameterized SQL queries to prevent SQL injection and ensures
   *       proper database interaction. Dates for new requacks are recorded using the current
   *       date.
   *
   * @details 
   * - **Spam Handling**: If a requack already exists for the user and quack, the method updates
   *   the existing record, setting the `spam` flag to `1`.
   * - **New Requack**: If no requack exists, a new entry is added to the `retweets` table,
   *   linking the `quack_id` to the `user_id` and recording the `writer_id` and current date.
   */
  int32_t addRequack(
      const int32_t &user_id,
      const int32_t &quack_id
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
    const int32_t& quack_id,
    const int32_t& user_id
  );
  
  /**
   * @brief Creates a new list for a user in the database.
   *
   * @param user_id The ID of the user who owns the list.
   * @param list_name The name of the new list.
   * @return true if the list was successfully created; false otherwise.
   */
  bool createList(
    const int32_t& user_id,
    const std::string& list_name
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
  
  /**
   * @brief Retrieves a feed of quacks and requacks for a given user.
   *
   * @param user_id The unique identifier of the user for whom the feed is generated.
   * @return A vector of strings where each string represents a formatted entry in the feed.
   */
  std::vector<std::string> getFeed(
    const int32_t& user_id
  );

  uint32_t getRequackCount(const int32_t& quack_id);
  
  std::vector<int32_t> getReplies(const int32_t& quack_id);
  
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
   * @brief Retrieves a quack from the database using its unique ID.
   *
   * @param quack_id The unique ID of the quack to retrieve.
   * @return A Pond::Quack struct containing the quack's information.
   */
  Pond::Quack getQuackFromID(
    const int32_t& quack_id
  );

  /**
   * @brief Retrieves the list of followers for a specified user.
   *
   * This method queries the database to find all users who follow the specified user
   * and returns their IDs and names.
   *
   * @param user_id The unique ID of the user whose followers are to be retrieved.
   * @return A vector of `Pond::User` objects, where each object contains:
   *         - `usr`: The unique ID of the follower.
   *         - `name`: The name of the follower.
   *
   * @note If no followers are found or if an error occurs during the query, the method
   *       returns an empty vector.
   *
   * @details
   * - The method uses an SQL query with a join between the `follows` and `users` tables
   *   to fetch the follower details.
   * - Parameterized SQL queries are used to prevent SQL injection.
   * - Each follower is represented by a `Pond::User` struct.
   */
  std::vector<Pond::User> getFollowers(const int32_t& user_id);

  /**
   * @brief Retrieves a list of users that a specified user is following.
   *
   * This method queries the database to find all user IDs of the users whom the 
   * specified user has chosen to follow.
   *
   * @param user_id The unique ID of the user whose following list is to be retrieved.
   * @return A vector of integers where each integer represents the unique ID of a user 
   *         that the specified user is following.
   *
   * @note If the user is not following anyone or if an error occurs during the query, 
   *       the method returns an empty vector.
   */
  std::vector<int32_t> getFollows(const int32_t& user_id);


  /**
   * @brief Retrieves all quacks created by a specified user.
   *
   * This method queries the database to fetch all quacks (tweets) authored by the given 
   * user, sorted by date and time in descending order (most recent first).
   *
   * @param user_id The unique ID of the user whose quacks are to be retrieved.
   * @return A vector of `Pond::Quack` objects, where each object contains:
   *         - `tid`: The unique ID of the quack.
   *         - `writer_id`: The unique ID of the user who authored the quack.
   *         - `text`: The text content of the quack.
   *         - `date`: The date the quack was created (format: YYYY-MM-DD).
   *         - `time`: The time the quack was created (format: HH:MM:SS).
   *         - `replyto_tid`: The unique ID of the quack this quack is replying to, 
   *           or 0 if it is not a reply.
   *
   * @note If the user has not authored any quacks or if an error occurs during the query, 
   *       the method returns an empty vector.
   */
  std::vector<Pond::Quack> getQuacks(
    const int32_t &user_id
  );

private:
  sqlite3* _db;

/**
 * @brief Generates a unique ID for a new user by determining the maximum existing user ID.
 *
 * This method queries the `users` table in the database to find the maximum `usr` 
 * (user ID) currently used. It then increments this value by 1 to create a new unique ID. 
 * If no users exist in the database, it assigns `1` as the first ID.
 *
 * @param[out] unique_id An integer reference that will hold the generated unique user ID.
 * @return true if the query was successful and the unique ID was generated; false otherwise.
 *
 * @note 
 * - This method assumes a sequential numbering system for user IDs.
 * - In the event of an empty `users` table, the ID starts from 1.
 * - If an error occurs while preparing or executing the SQL query, the method returns `false`.
 */
  bool _getUniqueUserID(
    int32_t& unique_id
  );

  /**
   * @brief Generates a unique ID for a new quack by determining the maximum existing quack ID.
   *
   * This method queries the `tweets` table in the database to find the maximum `tid` (quack ID) 
   * currently used. It then increments this value by 1 to create a new unique ID. If no quacks 
   * exist in the database, it assigns `1` as the first ID.
   *
   * @param[out] unique_id An integer reference that will hold the generated unique quack ID.
   * @return true if the query was successful and the unique ID was generated; false otherwise.
   *
   * @note 
   * - This method assumes a sequential numbering system for quack IDs.
   * - In the event of an empty `tweets` table, the ID starts from 1.
   * - If an error occurs while preparing or executing the SQL query, the method returns `false`.
   */
  bool _getUniqueQuackID(
    int32_t& unique_id
  );
  
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
    const int32_t& user_id
  );

  /**
   * @brief Formats a tweet's text to fit within a specified line width.
   *
   * This method takes a tweet's text and formats it such that each line does not 
   * exceed the given line width. Words are kept intact and moved to the next line 
   * if they cannot fit on the current line.
   *
   * @param text The input string containing the text to be formatted.
   * @param lineWidth The maximum width (in characters) allowed for each line.
   * @return A formatted string where lines are separated by newline characters (`\n`).
   *
   * @note 
   * - Words that are longer than the specified line width will be placed on their own line.
   * - Consecutive spaces are ignored when formatting.
   */
  std::string formatTweetText(
    const std::string& text, int lineWidth
  );
};
