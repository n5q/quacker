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
int32_t* Pond::addUser(const std::string& name, const std::string& email, const int64_t& phone, const std::string& password) {
  int32_t user_id;

  // Get a unique user ID
  if (!getUniqueUserID(user_id)) {
    return nullptr;  // Return nullptr if we couldn't get a unique ID
  }

  const char* query =
    "INSERT INTO users (usr, name, email, phone, pwd) "
    "VALUES (?, ?, ?, ?, ?)";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return nullptr;
  }

  // Bind parameters to prevent SQL injection.
  sqlite3_bind_int(stmt, 1, user_id);                              // usr
  sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);      // name
  sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_STATIC);     // email
  sqlite3_bind_int(stmt, 4, phone);                                // phone
  sqlite3_bind_text(stmt, 5, password.c_str(), -1, SQLITE_STATIC);  // pwd

  // Execute the query.
  int32_t* result = nullptr;
  if (sqlite3_step(stmt) == SQLITE_DONE) {
    result = new int32_t(user_id);  // Allocate a new int32_t if user was added successfully
  }

  sqlite3_finalize(stmt);
  return result;  // Return either the pointer to user_id or nullptr
}

/**
 * @brief Adds a new post to the posts table in the database.
 *
 * @param quack_id The unique ID of the quack.
 * @param user_id The ID of the user who created the post.
 * @param text The text content of the post.
 * @return true if the post was successfully added; false otherwise.
 */
bool Pond::addPost(const int32_t& quack_id, const int32_t& user_id, const std::string& text) {
  bool post_added = false;

  const char* query =
    "INSERT INTO posts (tid, writer_id, text, tdate, ttime) "
    "VALUES (?, ?, ?, ?, ?)";

  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
  }

  // Bind parameters to prevent SQL injection.
  sqlite3_bind_int(stmt, 1, quack_id);                              // tid
  sqlite3_bind_int(stmt, 2, user_id);                               // writer_id
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
 * @brief Adds a reply quack to the quacks table in the database.
 *
 * @param user_id The ID of the user creating the reply.
 * @param reply_quack_id The ID of the quack being replied to.
 * @param text The text content of the reply.
 * @return true if the reply was successfully added; false otherwise.
 */
bool Pond::addReply(const int32_t& user_id, const int32_t& reply_quack_id, const std::string& text) {
  bool reply_added = false;

  const char* query =
    "INSERT INTO tweets (tid, writer_id, text, tdate, ttime, replyto_tid) "
    "VALUES (?, ?, ?, ?, ?, ?)";

  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
  }

  int32_t new_tid = 1; // Temporary ._. Fix LTR

  // Bind parameters to prevent SQL injection
  sqlite3_bind_int(stmt, 1, new_tid);                               // tid;
  sqlite3_bind_int(stmt, 2, user_id);                               // writer_id
  sqlite3_bind_text(stmt, 3, text.c_str(), -1, SQLITE_STATIC);       // text
  sqlite3_bind_text(stmt, 4, this->_getDate(), -1, SQLITE_STATIC);   // tdate
  sqlite3_bind_text(stmt, 5, this->_getDate(), -1, SQLITE_STATIC);   // ttime
  sqlite3_bind_int(stmt, 6, reply_quack_id);                        // replyto_tid

  // Execute the query.
  if (sqlite3_step(stmt) == SQLITE_DONE) {
    reply_added = true;
  }
  sqlite3_finalize(stmt);

  return reply_added;
}

/**
 * @brief Creates a new list for a user in the database.
 *
 * @param user_id The ID of the user who owns the list.
 * @param list_name The name of the new list.
 * @return true if the list was successfully created; false otherwise.
 */
bool Pond::createList(const int32_t& user_id, const std::string& list_name) {
  bool list_created = false;

  const char* query =
    "INSERT INTO lists (owner_id, lname) "
    "VALUES (?, ?)";

  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
  }

  // Bind parameters to prevent SQL injection.
  sqlite3_bind_int(stmt, 1, user_id);                            // owner_id
  sqlite3_bind_text(stmt, 2, list_name.c_str(), -1, SQLITE_STATIC); // lname

  // Execute the query.
  if (sqlite3_step(stmt) == SQLITE_DONE) {
    list_created = true;
  }

  sqlite3_finalize(stmt);
  return list_created;
}

/**
 * @brief Adds a quack to a list in the database.
 *
 * @param list_id The name of the list.
 * @param quack_id The ID of the quack to add to the list.
 * @param user_id The ID of the user who owns the list.
 * @return true if the quack was successfully added to the list; false otherwise.
 */
bool Pond::addToList(const std::string& list_name, const int32_t& quack_id, const int32_t& user_id) {
  bool added_to_list = false;

  // check for existence first
  if (!this->_listExists(list_name, user_id)) {
    return added_to_list;
  }

  const char* query =
    "INSERT INTO include (owner_id, lname, tid) "
    "VALUES (?, ?, ?)";

  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
  }

  // Bind parameters to prevent SQL injection.
  sqlite3_bind_int(stmt, 1, user_id);                          // owner_id
  sqlite3_bind_text(stmt, 2, list_name.c_str(), -1, SQLITE_STATIC); // lname
  sqlite3_bind_int(stmt, 3, quack_id);                         // tid

  // Execute the query.
  if (sqlite3_step(stmt) == SQLITE_DONE) {
    added_to_list = true;
  }

  sqlite3_finalize(stmt);
  return added_to_list;
}


/**
 * @brief Checks if the provided user ID and password are valid for login.
 *
 * @param user_id The user ID to check in the database.
 * @param password The password corresponding to the user ID.
 * @return true if the login credentials are valid; false otherwise.
 */
int32_t* Pond::checkLogin(const int32_t& user_id, const std::string& password) {
  int32_t* user_id_ptr = nullptr;

  const char* query =
    "SELECT * "
    "FROM users "
    "WHERE usr = ? "
    "AND pwd = ?";

  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return nullptr;
  }

  // Bind parameters to prevent SQL injection.
  sqlite3_bind_int(stmt, 1, user_id);                               // usr
  sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);  // pwd


  // Execute the query.
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    int32_t retrieved_id = sqlite3_column_int(stmt, 0);
    user_id_ptr = new int32_t(retrieved_id);
  }
  sqlite3_finalize(stmt);

  return user_id_ptr;
}

/**
 * @brief Retrieves the username associated with a given user ID from the database.
 *
 * This function prepares an SQL statement to query the username of a specific user
 * from the `users` table using the provided user ID. If a matching user ID is found,
 * it retrieves the `name` column as the username. If no match is found or an error occurs,
 * it returns an empty string.
 *
 * @param user_id The unique identifier of the user whose username is being retrieved.
 * @return A std::string containing the username if found, otherwise an empty string.
 */
std::string Pond::getUsername(const int32_t& user_id) {
  std::string username;

  const char* query =
    "SELECT name "
    "FROM users "
    "WHERE usr = ?";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return "";
  }

  sqlite3_bind_int(stmt, 1, user_id);

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    const unsigned char* retrieved_username = sqlite3_column_text(stmt, 0);
    if (retrieved_username != nullptr) {
      username = reinterpret_cast<const char*>(retrieved_username);
    }
    else username = "";
  }

  sqlite3_finalize(stmt);

  return username;
}

/**
 * @brief Retrieves a feed of quacks and requacks for a given user.
 *
 * @param user_id The unique identifier of the user for whom the feed is generated.
 * @return A vector of strings where each string represents a formatted entry in the feed.
 */
std::vector<std::string> Pond::getFeed(const int32_t& user_id) {
    std::vector<std::string> feed;

    const char* query = 
        "SELECT 'tweet' AS type, t1.tid, u1.name, t1.writer_id, t1.tdate AS date, t1.ttime AS time, t1.text "
        "FROM tweets t1 "
        "JOIN follows f1 ON t1.writer_id = f1.flwee "
        "JOIN users u1 ON t1.writer_id = u1.usr "
        "WHERE f1.flwer = ? "
        "UNION "
        "SELECT 'retweet' AS type, t2.tid, u2.name, r.retweeter_id AS writer_id, r.rdate AS date, t2.ttime AS time, t2.text "
        "FROM retweets r "
        "JOIN tweets t2 ON t2.tid = r.tid "
        "JOIN follows f2 ON r.retweeter_id = f2.flwee "
        "JOIN users u2 ON r.retweeter_id = u2.usr "
        "WHERE f2.flwer = ? AND r.spam = 0 "
        "ORDER BY date DESC, time DESC";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return feed;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, user_id);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* tweet_id = sqlite3_column_text(stmt, 1);  // Id of quack 
        const unsigned char* username = sqlite3_column_text(stmt, 2);  // Username of the quack author
        const unsigned char* date = sqlite3_column_text(stmt, 4);      // Date of quack/requack
        const unsigned char* time = sqlite3_column_text(stmt, 5);      // Time of quack/requack
        const unsigned char* text = sqlite3_column_text(stmt, 6);      // Text of quack/requack

        std::ostringstream oss;
        oss << "Quack Id: " << reinterpret_cast<const char*>(tweet_id);
        oss << ", Author: " << (username ? reinterpret_cast<const char*>(username) : "Unknown");
        oss << std::string(66 - oss.str().length(), ' '); 
        oss << "Date and Time: " <<(date ? reinterpret_cast<const char*>(date) : "Unknown")
            << " " << (time ? reinterpret_cast<const char*>(time) : "Unknown") << "\n\n";
        oss << "Text: " <<(text ? formatTweetText(reinterpret_cast<const char*>(text), 94) : "") << "\n";

        feed.push_back(oss.str());
    }

    sqlite3_finalize(stmt);

    return feed;
}

/**
 * @brief Adds a follow relationship between two users.
 *
 * @param user_id The ID of the user who is following.
 * @param follow_id The ID of the user to be followed.
 * @return true if the follow was successfully added, false otherwise.
 */
bool Pond::follow(const int32_t& user_id, const int32_t& follow_id) {
  bool follow_added = false;

  const char* query =
    "INSERT INTO follows (flwer, flwee, start_date) "
    "VALUES (?, ?, ?)";

  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
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
 * @param user_id The ID of the user who is unfollowing.
 * @param follow_id The ID of the user to be unfollowed.
 * @return true if the unfollow was successful, false otherwise.
 */
bool Pond::unfollow(const int32_t& user_id, const int32_t& follow_id) {
  bool unfollowed = false;

  const char* query =
    "DELETE FROM follows "
    "WHERE flwer = ? "
    "AND flwee = ?";

  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
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

bool Pond::reply(const int32_t& user_id, const int32_t& reply_quack_id, const std::string& text) {
  bool reply_added = false;

  const char* query =
    "INSERT INTO tweets (tid, writer_id, text, tdate, ttime, replyto_tid) "
    "VALUES (?, ?, ?, ?, ?, ?)";

  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
  }

  int32_t new_tid = -1; // TODO: get id from moussas function

  // Bind parameters to prevent SQL injection
  sqlite3_bind_int(stmt,  1, new_tid);                               // tid;
  sqlite3_bind_int(stmt,  2, user_id);                               // writer_id
  sqlite3_bind_text(stmt, 3, text.c_str(), -1, SQLITE_STATIC);       // text
  sqlite3_bind_text(stmt, 4, this->_getDate(), -1, SQLITE_STATIC);   // tdate
  sqlite3_bind_text(stmt, 5, this->_getTime(), -1, SQLITE_STATIC);   // ttime
  sqlite3_bind_int(stmt,  6, reply_quack_id);                        // replyto_tid

  // Execute the query.
  if (sqlite3_step(stmt) == SQLITE_DONE) {
    reply_added = true;
  }
  sqlite3_finalize(stmt);

  return reply_added;
}

bool Pond::requack(const int32_t& user_id, const int32_t& requack_quack_id, const bool spam) {
  bool requack_added = false;

  const char* query =
    "INSERT INTO retweets (tid, retweeter_id, rdate, spam) "
    "VALUES (?, ?, ?, ?)";

  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
  }

  // Bind parameters to prevent SQL injection
  sqlite3_bind_int(stmt, 1, requack_quack_id);                      // tid
  sqlite3_bind_int(stmt, 2, user_id);                               // retweeter_id
  sqlite3_bind_text(stmt, 3, this->_getDate(), -1, SQLITE_STATIC);  // rdate
  sqlite3_bind_int(stmt, 4, spam);                                  // spam

  // Execute the query.
  if (sqlite3_step(stmt) == SQLITE_DONE) {
    requack_added = true;
  }
  sqlite3_finalize(stmt);

  return requack_added;
}

/**
 * @brief Searches for users in the database whose names contain the specified search terms.
 *
 * @param search_terms The terms to search for in user names.
 * @return A vector of pairs containing user IDs and names that match the search terms.
 */
std::vector<Pond::User> Pond::searchForUsers(const std::string& search_terms) {
  std::vector<Pond::User> results;

  const char* query =
    "SELECT usr, name "
    "FROM users "
    // lower for case insensitive search
    "WHERE LOWER(name) LIKE '%' || LOWER(?) || '%'";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return results;
  }

  // Bind parameters to prevent SQL injection.
  sqlite3_bind_text(stmt, 1, search_terms.c_str(), -1, SQLITE_STATIC); // name

  // Execute the query.
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int32_t id_result = sqlite3_column_int(stmt, 0);
    std::string name_result = (const char*)sqlite3_column_text(stmt, 1);
    Pond::User user;
    user.usr = id_result;
    user.name = name_result;
    results.push_back(user);
  }

  sqlite3_finalize(stmt);
  return results;
}


/**
 * @brief search for quacks containing specific keywords or hashtags.
 *
 * @param search_terms A string of keywords or hashtags to search for in quacks.
 * @return A vector of quacks that contain the specified keywords or hashtags, ordered by date and time.
 *
 * @note case insensitive search, space seperated keywoards
 */
std::vector<Pond::Quack> Pond::searchForQuacks(const std::string& search_terms) {
  std::vector<Pond::Quack> results;
  std::unordered_set<int32_t> quack_ids; // keep track of unique quack ids across searches

  // Split the keyword input into individual keywords, using commas as delimiters
  std::istringstream iss(search_terms);
  std::vector<std::string> keywords;
  std::string keyword;
  while (std::getline(iss, keyword, ',')) { // specify comma as delimiter
    keywords.push_back(keyword);
  }

  const char* hashtag_query =
    "SELECT t.tid, t.writer_id, t.text, t.tdate, t.ttime, t.replyto_tid "
    "FROM tweets t "
    "JOIN hashtag_mentions ht ON t.tid = ht.tid "
    "WHERE LOWER(ht.term) LIKE LOWER(?)"
    "ORDER BY t.tdate DESC, t.ttime DESC";

  // Prepare to query 
  sqlite3_stmt* stmt;
  for (const std::string& kw : keywords) {
    if (kw[0] == '#') {
      // std::string hashtag = kw.substr(1);  // remove # prefix

      if (sqlite3_prepare_v2(this->_db, hashtag_query, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        continue;
      }

      sqlite3_bind_text(stmt, 1, kw.c_str(), -1, SQLITE_STATIC);

      // Retrieve results
      while (sqlite3_step(stmt) == SQLITE_ROW) {
        int32_t quack_id = sqlite3_column_int(stmt, 0);
        if (quack_ids.find(quack_id) == quack_ids.end()) {
          Pond::Quack quack;
          quack.tid = quack_id;
          quack.writer_id = sqlite3_column_int(stmt, 1);
          quack.text = (const char*)(sqlite3_column_text(stmt, 2));
          quack.date = (const char*)(sqlite3_column_text(stmt, 3));
          quack.time = (const char*)(sqlite3_column_text(stmt, 4));
          quack.replyto_tid = sqlite3_column_int(stmt, 5);

          results.push_back(quack);
          // quack_ids.insert(quack_id);
        }
      }
      sqlite3_finalize(stmt);
    }

    else { // text keyword
      const char* text_query =
        "SELECT tid, writer_id, text, tdate, ttime, replyto_tid "
        "FROM tweets "
        "WHERE LOWER(text) LIKE '% ' || LOWER(?) || ' %' "
        "OR LOWER(text) LIKE '% ' || LOWER(?) || ' %' "
        "OR LOWER(text) LIKE '% ' || LOWER(?) "
        "OR LOWER(text) LIKE '% ' || LOWER(?) "
        "OR LOWER(text) LIKE LOWER(?) || ' %' "
        "OR LOWER(text) LIKE LOWER(?) || ' %' "
        "ORDER BY tdate DESC, ttime DESC";


      if (sqlite3_prepare_v2(this->_db, text_query, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        continue;
      }

      // std::string kw_ht = "#"+kw;
      sqlite3_bind_text(stmt, 1, kw.c_str(), -1, SQLITE_STATIC);
      sqlite3_bind_text(stmt, 2, ("#"+kw).c_str(), -1, SQLITE_STATIC);
      sqlite3_bind_text(stmt, 3, kw.c_str(), -1, SQLITE_STATIC);
      sqlite3_bind_text(stmt, 4, ("#"+kw).c_str(), -1, SQLITE_STATIC);
      sqlite3_bind_text(stmt, 5, kw.c_str(), -1, SQLITE_STATIC);
      sqlite3_bind_text(stmt, 6, ("#"+kw).c_str(), -1, SQLITE_STATIC);

      // Retrieve results
      while (sqlite3_step(stmt) == SQLITE_ROW) {
        int32_t quack_id = sqlite3_column_int(stmt, 0);
        if (quack_ids.find(quack_id) == quack_ids.end()) {
          Quack quack;
          quack.tid = quack_id;
          quack.writer_id = sqlite3_column_int(stmt, 1);
          quack.text = (const char*)(sqlite3_column_text(stmt, 2));
          quack.date = (const char*)(sqlite3_column_text(stmt, 3));
          quack.time = (const char*)(sqlite3_column_text(stmt, 4));
          quack.replyto_tid = sqlite3_column_int(stmt, 5);

          results.push_back(quack);
          quack_ids.insert(quack_id);
        }
      }
      sqlite3_finalize(stmt);
    }
  }

  return results;
}

/**
 * @brief Finds a unique user ID that is not currently in use in the database.
 *
 * @param[out] unique_id An integer reference that will be set to a unique user ID.
 * @return `true` if a unique ID is successfully found and assigned; `false` if an error occurs.
 */
bool Pond::getUniqueUserID(int32_t& unique_id) {
  const char* query =
    "SELECT MAX(usr) FROM users";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
  }

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    int32_t max_id = sqlite3_column_int(stmt, 0);
    unique_id = max_id + 1;
  } else {
    unique_id = 1;
  }

  sqlite3_finalize(stmt);
  return true;
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
bool Pond::_listExists(const std::string &list_name, const int32_t &user_id) {
  bool exists = false;

  const char* query = "SELECT 1 FROM lists WHERE owner_id = ? AND lname = ?";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
  }

  // Bind parameters to prevent SQL injection.
  sqlite3_bind_int(stmt, 1, user_id);                                     // owner_id
  sqlite3_bind_text(stmt, 2, list_name.c_str(), -1, SQLITE_STATIC);       // lname

  // Execute the query.
  exists = sqlite3_step(stmt) == SQLITE_ROW;
  sqlite3_finalize(stmt);

  if (!exists) {
    return false;
  }
  else {
    return true;
  }
}

std::string Pond::formatTweetText(const std::string& text, int lineWidth) {
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

/**
 * @brief Retrieves a quack from the database using its unique ID.
 *
 * @param quack_id The unique ID of the quack to retrieve.
 * @return A Pond::Quack struct containing the quack's information.
 */
Pond::Quack Pond::getQuackFromID(const int32_t& quack_id) {
  Pond::Quack quack;

  const char* query =
    "SELECT tid, writer_id, text, tdate, ttime, replyto_tid "
    "FROM tweets "
    "WHERE tid = ?";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return quack;
  }

  sqlite3_bind_int(stmt, 1, quack_id);

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    quack.tid = sqlite3_column_int(stmt, 0);
    quack.writer_id = sqlite3_column_int(stmt, 1);
    quack.text = (const char*)sqlite3_column_text(stmt, 2);
    quack.date = (const char*)sqlite3_column_text(stmt, 3);
    quack.time = (const char*)sqlite3_column_text(stmt, 4);
    quack.replyto_tid = sqlite3_column_int(stmt, 5);
  }

  sqlite3_finalize(stmt);
  return quack;
  }