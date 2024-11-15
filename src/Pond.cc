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

bool Pond::addHashtag(const int32_t& quack_id, const std::string& hashtag) {
  const char *query =
      "INSERT INTO hashtag_mentions (tid, term) "
      "SELECT ?, ? "
      "WHERE NOT EXISTS ("
      "  SELECT 1 FROM hashtag_mentions "
      "  WHERE tid = ? AND term = ? COLLATE NOCASE"
      ")";
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
  }

  // Bind parameters to prevent SQL injection
  sqlite3_bind_int(stmt, 1, quack_id);                          // tid
  sqlite3_bind_text(stmt, 2, hashtag.c_str(), -1, SQLITE_STATIC); // term

  // Execute the query.
  bool added = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);

  return added;
}

bool Pond::validateQuack(const int32_t& quack_id, const std::string& text) {
  // Check if the text is empty
  if (text.empty()) {
    return false;
  }

  // One tweet can have multiple hashtags but not multiple instances of the same hashtag
  std::unordered_set<std::string> hashtags;
  std::istringstream iss(text);
  std::string word;
  while (iss >> word) {
    if (word[0] == '#' && word.size() > 1) {

      std::string hashtag = word;
      std::transform(hashtag.begin(), hashtag.end(), hashtag.begin(), ::tolower);

      if (hashtags.find(hashtag) != hashtags.end()) {
        return false;
      }
      hashtags.insert(hashtag);
      this->addHashtag(quack_id, hashtag);
    }
  }

  return true;
}

/**
 * @brief Adds a new quack to the database.
 *
 * @param user_id The ID of the user who is posting the quack.
 * @param text The text of the quack.
 * @return A pointer to the unique ID of the quack if it was successfully added; nullptr otherwise.
 */
int32_t* Pond::addQuack(const int32_t& user_id, const std::string& text) {
  int32_t* result = nullptr;

  int32_t quack_id;
  if (!this->getUniqueQuackID(quack_id)) {
    return result;
  }

  if (!validateQuack(quack_id, text)) {
    return result;
  }

  const char* query =
    "INSERT INTO tweets (tid, writer_id, text, tdate, ttime) "
    "VALUES (?, ?, ?, ?, ?)";

  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return result;
  }

  // Bind parameters to prevent SQL injection.
  sqlite3_bind_int(stmt, 1, quack_id);                               // tid
  sqlite3_bind_int(stmt, 2, user_id);                                // writer_id
  sqlite3_bind_text(stmt, 3, text.c_str(), -1, SQLITE_STATIC);       // text
  sqlite3_bind_text(stmt, 4, this->_getDate(), -1, SQLITE_STATIC);   // tdate
  sqlite3_bind_text(stmt, 5, this->_getTime(), -1, SQLITE_STATIC);   // ttime

  // Execute the query.
  if (sqlite3_step(stmt) == SQLITE_DONE) {
    result = new int32_t(quack_id);
  }
  sqlite3_finalize(stmt);

  return result;
}

int32_t* Pond::addReply(const int32_t& user_id, const int32_t& reply_quack_id, const std::string& text) {
  int32_t* result = nullptr;

  const char* query =
    "INSERT INTO tweets (tid, writer_id, text, tdate, ttime, replyto_tid) "
    "VALUES (?, ?, ?, ?, ?, ?)";

  // Prepare the SQL statement.
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return result;
  }

  int32_t reply_tid;
  if (!getUniqueQuackID(reply_tid)) {
    return result;  // Return nullptr if we couldn't get a unique ID
  }

  // Bind parameters to prevent SQL injection
  sqlite3_bind_int(stmt, 1, reply_tid);                              // tid;
  sqlite3_bind_int(stmt, 2, user_id);                                // writer_id
  sqlite3_bind_text(stmt, 3, text.c_str(), -1, SQLITE_STATIC);       // text
  sqlite3_bind_text(stmt, 4, this->_getDate(), -1, SQLITE_STATIC);   // tdate
  sqlite3_bind_text(stmt, 5, this->_getTime(), -1, SQLITE_STATIC);   // ttime
  sqlite3_bind_int(stmt, 6, reply_quack_id);                         // replyto_tid

  // Execute the query.
  if (sqlite3_step(stmt) == SQLITE_DONE) {
    result = new int32_t(reply_tid);
  }
  sqlite3_finalize(stmt);

  return result;
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

int32_t Pond::addRequack(const int32_t &user_id, const int32_t &quack_id) {
  int32_t requack_status = -1;

  // Check if the user has already requacked this quack
  const char *check_query =
      "SELECT COUNT(*) FROM retweets WHERE tid = ? AND retweeter_id = ?";

  sqlite3_stmt *check_stmt;
  if (sqlite3_prepare_v2(this->_db, check_query, -1, &check_stmt, nullptr) != SQLITE_OK) {
    std::cerr << "SQL Error (prepare check): " << sqlite3_errmsg(this->_db) << std::endl;
    return 3;
  }

  if (sqlite3_bind_int(check_stmt, 1, quack_id) != SQLITE_OK ||
      sqlite3_bind_int(check_stmt, 2, user_id) != SQLITE_OK) {
    std::cerr << "SQL Error (bind check): " << sqlite3_errmsg(this->_db) << std::endl;
    sqlite3_finalize(check_stmt);
    return 3;
  }

  int already_requacked = 0;
  if (sqlite3_step(check_stmt) == SQLITE_ROW) {
    already_requacked = sqlite3_column_int(check_stmt, 0);
  }
  else {
    std::cerr << "SQL Error (step check): " << sqlite3_errmsg(this->_db) << std::endl;
    sqlite3_finalize(check_stmt);
    return 3;
  }

  sqlite3_finalize(check_stmt);

  if (already_requacked > 0) {
    // User has already requacked; update the existing entry to mark as spam
    const char *update_query =
        "UPDATE retweets SET spam = 1 WHERE tid = ? AND retweeter_id = ?";

    sqlite3_stmt *update_stmt;
    if (sqlite3_prepare_v2(this->_db, update_query, -1, &update_stmt, nullptr) != SQLITE_OK) {
      std::cerr << "SQL Error (prepare update): " << sqlite3_errmsg(this->_db) << std::endl;
      return 3;
    }

    if (sqlite3_bind_int(update_stmt, 1, quack_id) != SQLITE_OK ||
        sqlite3_bind_int(update_stmt, 2, user_id) != SQLITE_OK) {
      std::cerr << "SQL Error (bind update): " << sqlite3_errmsg(this->_db) << std::endl;
      sqlite3_finalize(update_stmt);
      return 3;
    }

    if (sqlite3_step(update_stmt) != SQLITE_DONE) {
      std::cerr << "SQL Error (step update): " << sqlite3_errmsg(this->_db) << std::endl;
    }
    else {
      requack_status = 1; // Status indicating spam update
    }

    sqlite3_finalize(update_stmt);
    return requack_status;
  }

  // Proceed to insert the requack as a new entry
  const char *insert_query =
      "INSERT INTO retweets (tid, retweeter_id, writer_id, rdate, spam) "
      "VALUES (?, ?, ?, ?, ?)";

  sqlite3_stmt *insert_stmt;
  if (sqlite3_prepare_v2(this->_db, insert_query, -1, &insert_stmt, nullptr) != SQLITE_OK) {
    std::cerr << "SQL Error (prepare insert): " << sqlite3_errmsg(this->_db) << std::endl;
    return 3;
  }

  if (sqlite3_bind_int(insert_stmt, 1, quack_id) != SQLITE_OK ||
      sqlite3_bind_int(insert_stmt, 2, user_id) != SQLITE_OK ||
      sqlite3_bind_int(insert_stmt, 3, this->getQuackFromID(quack_id).writer_id) != SQLITE_OK ||
      sqlite3_bind_text(insert_stmt, 4, this->_getDate(), -1, SQLITE_STATIC) != SQLITE_OK ||
      sqlite3_bind_int(insert_stmt, 5, 0) != SQLITE_OK) { // No spam for new requack
    std::cerr << "SQL Error (bind insert): " << sqlite3_errmsg(this->_db) << std::endl;
    sqlite3_finalize(insert_stmt);
    return 3;
  }

  if (sqlite3_step(insert_stmt) != SQLITE_DONE) {
    std::cerr << "SQL Error (step insert): " << sqlite3_errmsg(this->_db) << std::endl;
  }
  else {
    requack_status = 0; // Status indicating new requack added
  }

  sqlite3_finalize(insert_stmt);
  return requack_status;
}

// int32_t Pond::addRequack(const int32_t &user_id, const int32_t &quack_id, const bool spam) {
//   int32_t requack_added = -1;

//   const char *query =
//       "INSERT INTO retweets (tid, retweeter_id, writer_id, rdate, spam) "
//       "VALUES (?, ?, ?, ?, ?)";

//   sqlite3_stmt *stmt;
//   if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
//     std::cerr << "SQL Error (prepare): " << sqlite3_errmsg(this->_db) << std::endl;
//     return 3;
//   }

//   if (sqlite3_bind_int(stmt, 1, quack_id) != SQLITE_OK ||
//       sqlite3_bind_int(stmt, 2, user_id) != SQLITE_OK ||
//       sqlite3_bind_int(stmt, 3, this->getQuackFromID(quack_id).writer_id) != SQLITE_OK ||
//       sqlite3_bind_text(stmt, 4, this->_getDate(), -1, SQLITE_STATIC) != SQLITE_OK ||
//       sqlite3_bind_int(stmt, 5, spam) != SQLITE_OK) {
//     std::cerr << "SQL Error (bind): " << sqlite3_errmsg(this->_db) << std::endl;
//     sqlite3_finalize(stmt);
//     return 3;
//   }

//   if (sqlite3_step(stmt) != SQLITE_DONE) {
//     std::cerr << "SQL Error (step): " << sqlite3_errmsg(this->_db) << std::endl;
//   }
//   else {
//     requack_added = 0;
//   }

//   sqlite3_finalize(stmt);
//   return requack_added;
// }

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
    Pond::User user;
    user.usr = sqlite3_column_int(stmt, 0);
    user.name = (const char*)sqlite3_column_text(stmt, 1);
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
      const char *text_query =
        "SELECT tid, writer_id, text, tdate, ttime, replyto_tid "
        "FROM tweets "
        "WHERE LOWER(text) LIKE '% ' || LOWER(?) || ' %' "
        "OR LOWER(text) LIKE '% ' || LOWER(?) || ' %' "
        "OR LOWER(text) LIKE '% ' || LOWER(?) "
        "OR LOWER(text) LIKE '% ' || LOWER(?) "
        "OR LOWER(text) LIKE LOWER(?) || ' %' "
        "OR LOWER(text) LIKE LOWER(?) || ' %' "
        "OR LOWER(text) = LOWER(?)"
        "OR LOWER(text) = LOWER(?)"
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
      sqlite3_bind_text(stmt, 7, kw.c_str(), -1, SQLITE_STATIC);
      sqlite3_bind_text(stmt, 8, ("#" + kw).c_str(), -1, SQLITE_STATIC);

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

uint32_t Pond::getRequackCount(const int32_t& quack_id) {
  uint32_t requack_count = 0;

  const char *query =
    "SELECT COUNT(tid) "
    "FROM retweets "
    "WHERE tid = ?";

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return requack_count;
  }

  sqlite3_bind_int(stmt, 1, quack_id);

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    requack_count = sqlite3_column_int(stmt, 0);
  }

  sqlite3_finalize(stmt);

  return requack_count;
}

std::vector<int32_t> Pond::getReplies(const int32_t& quack_id) {
  std::vector<int32_t> results;

  const char* query =
    "SELECT tid "
    "FROM tweets "
    "WHERE replyto_tid = ?";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return results;
  }

  sqlite3_bind_int(stmt, 1, quack_id);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    results.push_back(sqlite3_column_int(stmt, 0));
  }

  sqlite3_finalize(stmt);
  
  return results;
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

std::vector<int32_t> Pond::getFollowers(const int32_t& user_id) {
  std::vector<int32_t> results;

  const char* query =
    "SELECT flwer "
    "FROM follows "
    "WHERE flwee = ?";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return results;
  }

  sqlite3_bind_int(stmt, 1, user_id);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    results.push_back(sqlite3_column_int(stmt, 0));
  }

  sqlite3_finalize(stmt);
  
  return results;
}

std::vector<int32_t> Pond::getFollows(const int32_t& user_id) {
  std::vector<int32_t> results;

  const char* query =
  "SELECT flwee "
  "FROM follows "
  "WHERE flwer = ?";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return results;
  }

  sqlite3_bind_int(stmt, 1, user_id);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    results.push_back(sqlite3_column_int(stmt, 0));
  }

  sqlite3_finalize(stmt);

  return results;
}

std::vector<Pond::Quack> Pond::getQuacks(const int32_t& user_id) {
  std::vector<Pond::Quack> results;

  const char* query =
    "SELECT tid, writer_id, text, tdate, ttime, replyto_tid "
    "FROM tweets "
    "WHERE writer_id = ? "
    "ORDER BY tdate DESC, ttime DESC";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return results;
  }

  sqlite3_bind_int(stmt, 1, user_id);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Pond::Quack quack;
    quack.tid =  sqlite3_column_int(stmt, 0);
    quack.writer_id = sqlite3_column_int(stmt, 1);
    quack.text = (const char*)(sqlite3_column_text(stmt, 2));
    quack.date = (const char*)(sqlite3_column_text(stmt, 3));
    quack.time = (const char*)(sqlite3_column_text(stmt, 4));
    quack.replyto_tid = sqlite3_column_int(stmt, 5);

    results.push_back(quack);
  }

  sqlite3_finalize(stmt);
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

bool Pond::getUniqueQuackID(int32_t& unique_id) {
  const char* query =
    "SELECT MAX(tid) FROM tweets";

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
  std::strftime(t, 9, "%H:%M:%S", gmt);

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
  std::strftime(t, 11, "%F", gmt);

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