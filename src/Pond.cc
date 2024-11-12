#include "Pond.hh"

Pond::Pond()
  : _db(nullptr) {
}

Pond::~Pond() {
  sqlite3_close(this->_db);
}

/**
 * @brief Opens a connection to the SQLite database specified by the filename.
 *
 * @param db_filename The name of the database file to open.
 * @return int Returns SQLITE_OK (0) if the database was successfully opened,
 *         or a non-zero SQLite error code if it failed.
 */
int Pond::loadDatabase(const std::string& db_filename) {
  int exit_code = sqlite3_open(db_filename.c_str(), &this->_db);
  if (exit_code) {
    std::cerr << "Can't open database: " << sqlite3_errmsg(this->_db) << std::endl;
    return exit_code;
  }
  return 0;
}

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
bool Pond::addUser(const std::string& name, const std::string& email, const int& phone, const std::string& password){
  bool user_added = false;

  // uint32_t user_id = generateID();
  uint32_t user_id = 1; // <<<< temporary (permanent ._.)bannana
  
  const char* query = 
    "INSERT INTO users (usr, name, email, phone, pwd) "
    "VALUES (?, ?, ?, ?, ?)";
  
  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return ERROR_SQL;
  }

  // Bind parameters to prevent SQL injection.
  sqlite3_bind_int(stmt, 1, user_id);   // usr
  sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);      // name
  sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_STATIC);     // email
  sqlite3_bind_int(stmt,  4, phone);                                // phone
  sqlite3_bind_text(stmt, 5, password.c_str(), -1, SQLITE_STATIC);  // pwd

  // Execute the query.
  if (sqlite3_step(stmt) == SQLITE_DONE) {
    user_added = true;
  }
  sqlite3_finalize(stmt);
  
  return user_added;
}

/**
 * @brief Adds a new post to the posts table in the database.
 *
 * @param tweet_id The unique ID of the tweet.
 * @param user_id The ID of the user who created the post.
 * @param text The text content of the post.
 * @return true if the post was successfully added; false otherwise.
 */
bool Pond::addPost(const uint32_t& tweet_id, const uint32_t& user_id, const std::string& text) {
  bool post_added = false;

  const char* query = 
    "INSERT INTO posts (tid, writer_id, text, tdate, ttime) "
    "VALUES (?, ?, ?, ?, ?)";

  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return ERROR_SQL;
  }

  // Bind parameters to prevent SQL injection.
  sqlite3_bind_int(stmt,  1, tweet_id);                              // tid
  sqlite3_bind_int(stmt,  2, user_id);                               // writer_id
  sqlite3_bind_text(stmt, 3, text.c_str(), -1, SQLITE_STATIC);       // text
  sqlite3_bind_text(stmt, 4, this->_getDate(), -1, SQLITE_STATIC);   // tdate
  sqlite3_bind_text(stmt, 5, this->_getTime(), -1, SQLITE_STATIC);   // ttime

  // Execute the query.
  if (sqlite3_step(stmt) == SQLITE_ROW) {
      post_added = true;
  }
  sqlite3_finalize(stmt);
  
  return post_added;
}

/**
 * @brief Adds a reply tweet to the tweets table in the database.
 *
 * @param user_id The ID of the user creating the reply.
 * @param reply_tweet_id The ID of the tweet being replied to.
 * @param text The text content of the reply.
 * @return true if the reply was successfully added; false otherwise.
 */
bool Pond::addReply(const uint32_t& user_id, const uint32_t& reply_tweet_id, const std::string& text) {
  bool reply_added = false;

  const char* query = 
    "INSERT INTO tweets (tid, writer_id, text, tdate, ttime, replyto_tid) "
    "VALUES (?, ?, ?, ?, ?, ?)";

  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return ERROR_SQL;
  }

  // uint32_t new_tid = generateUniqueTweetID();
  uint32_t new_tid = 1; // Temporary ._. Fix LTR


  // Bind parameters to prevent SQL injection
  sqlite3_bind_int(stmt,  1, new_tid);                               // tid;
  sqlite3_bind_int(stmt,  2, user_id);                               // writer_id
  sqlite3_bind_text(stmt, 3, text.c_str(), -1, SQLITE_STATIC);       // text
  sqlite3_bind_text(stmt, 4, this->_getDate(), -1, SQLITE_STATIC);   // tdate
  sqlite3_bind_text(stmt, 5, this->_getDate(), -1, SQLITE_STATIC);   // ttime
  sqlite3_bind_int(stmt,  6, reply_tweet_id);                        // replyto_tid

  // Execute the query.
  if (sqlite3_step(stmt) == SQLITE_DONE) {
      reply_added = true;
  }
  sqlite3_finalize(stmt);
  
  return reply_added;
}

/**
 * @brief Checks if the provided user ID and password are valid for login.
 *
 * @param user_id The user ID to check in the database.
 * @param password The password corresponding to the user ID.
 * @return true if the login credentials are valid; false otherwise.
 */
bool Pond::checkLogin(const uint32_t& user_id, const std::string& password) {
  bool valid_login = false;

  const char* query = 
    "SELECT * "
    "FROM users "
    "WHERE usr = ? "
    "AND pwd ?";
  
  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return ERROR_SQL;
  }

  // Bind parameters to prevent SQL injection.
  sqlite3_bind_int(stmt, 1, user_id);                               // usr
  sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);  // pwd

  // Execute the query.
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    valid_login = true;
  }
  sqlite3_finalize(stmt);
  
  return valid_login;
}

/**
 * @brief Adds a follow relationship between two users.
 *
 * Inserts a new record into the "follows" table,
 * indicating that the user with ID `user_id` has started following
 * the user with ID `follow_id`. The current date is added as the
 * start date of the follow relationship.
 *
 * @param user_id The ID of the user who is following.
 * @param follow_id The ID of the user to be followed.
 * @return true if the follow was successfully added, false otherwise.
 */
bool Pond::follow(const uint32_t& user_id, const uint32_t& follow_id) {
  bool follow_added = false;

  const char* query = 
    "INSERT INTO follows (flwer, flwee, start_date) "
    "VALUES (?, ?, ?)";
  
  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return ERROR_SQL;
  }

  // Bind parameters to prevent SQL injection.
  sqlite3_bind_int(stmt, 1, user_id);                               // follower_id
  sqlite3_bind_int(stmt, 2, follow_id);                             // followee_id
  sqlite3_bind_text(stmt, 3, this->_getDate(), -1, SQLITE_STATIC);  // start_date


  // Execute the query.
  if (sqlite3_step(stmt) == SQLITE_DONE) {
    follow_added = true;
  }
  sqlite3_finalize(stmt);
  
  return follow_added;
}

/**
 * @brief Removes a follow relationship between two users.
 *
 * Deletes a record from the "follows" table,
 * indicating that the user with ID `user_id` has stopped
 * following the user with ID `follow_id`.
 *
 * @param user_id The ID of the user who is unfollowing.
 * @param follow_id The ID of the user to be unfollowed.
 * @return true if the unfollow was successful, false otherwise.
 */
bool Pond::unfollow(const uint32_t& user_id, const uint32_t& follow_id) {
  bool unfollowed = false;

  const char* query = 
    "DELETE FROM follows "
    "WHERE flwer = ? "
    "AND flwee = ?";
  
  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return ERROR_SQL;
  }

  // Bind parameters to prevent SQL injection.
  sqlite3_bind_int(stmt, 1, user_id);   // follower_id
  sqlite3_bind_int(stmt, 2, follow_id); // followee_id

  // Execute the query.
  if (sqlite3_step(stmt) == SQLITE_DONE) {
    unfollowed = true;
  }
  sqlite3_finalize(stmt);
  
  return unfollowed;
}



/**
 * @brief Retrieves the current time in GMT as a formatted string (HH:MM:SS).
 *
 * @return A string representing the current time in "HH:MM:SS" format.
 */
char* Pond::_getTime() {
  std::time_t rn = std::time(nullptr);
  std::tm* gmt = std::gmtime(&rn);

  char* t = new char[9];
  std::strftime(t, sizeof(t), "%H:%M:%S", gmt);
  
  return t;
}

/**
 * @brief Retrieves the current date in GMT as a formatted string (YYYY-MM-DD).
 *
 * @return A string representing the current date in "YYYY-MM-DD" format.
 */
char* Pond::_getDate() {
  std::time_t rn = std::time(nullptr);
  std::tm* gmt = std::gmtime(&rn);

  char* t = new char[11];
  // yyyy-mm-dd
  std::strftime(t, sizeof(t), "%F", gmt);

  return t;
}

