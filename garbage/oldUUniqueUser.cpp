bool Pond::getUniqueUserID(int32_t& unique_id) {
  unique_id = 1;
  bool found = false;

  const char* query =
    "SELECT usr FROM users WHERE usr >= 0 ORDER BY usr ASC";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(this->_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int32_t current_id = sqlite3_column_int(stmt, 0);

    if (current_id == unique_id) {
      unique_id++;
    }
    else if (current_id > unique_id) {
      found = true;
      break;
    }
  }

  sqlite3_finalize(stmt);

  if (!found && unique_id > INT32_MAX) {
    unique_id = -1;
    const char* query_neg =
      "SELECT usr FROM users WHERE usr < 0 ORDER BY usr DESC";

    if (sqlite3_prepare_v2(this->_db, query_neg, -1, &stmt, nullptr) != SQLITE_OK) {
      sqlite3_finalize(stmt);
      return false;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
      int32_t current_id = sqlite3_column_int(stmt, 0);

      if (current_id == unique_id) {
        unique_id--;
      }
      else if (current_id < unique_id) {
        break;
      }
    }
    sqlite3_finalize(stmt);
  }

  return true;
}