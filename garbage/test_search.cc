#include <iostream>
#include "definitions.hh"
#include "Pond.hh"

int main() {
    Pond pond;
    // Load the test database
    if (pond.loadDatabase("/cshome/nqadri/assignment-3-bitbandarrius/test/prj-sample.db") != SQLITE_OK) {
        std::cerr << "Failed to load database." << std::endl;
        return 1;
    }

    // Test searchForUsers
    std::string user_search_term = "Jo"; // Example search term
    std::cout << "Testing searchForUsers with term: " << user_search_term << std::endl;
    auto user_results = pond.searchForUsers(user_search_term);
    std::cout << "Found " << user_results.size() << " users matching the search term." << std::endl;
    for (const auto& result : user_results) {
        std::cout << "User ID: " << result.usr << ", Name: " << result.name << std::endl;
    }

    // Test searchForTweets
    std::string tweet_search_term = "#Database"; // Example search term (can be hashtag or keyword)
    std::cout << "Testing searchForTweets with term: " << tweet_search_term << std::endl;
    auto tweet_results = pond.searchForTweets(tweet_search_term);
    std::cout << "Found " << tweet_results.size() << " tweets matching the search term." << std::endl;
    for (const auto& tweet : tweet_results) {
        std::cout << "Tweet ID: " << tweet.tid << ", Writer ID: " << tweet.writer_id
            << ", Text: " << tweet.text << ", Date: " << tweet.date
            << ", Time: " << tweet.time << std::endl;
        if (tweet.replyto_tid != 0) {
            std::cout << "Reply to Tweet ID: " << tweet.replyto_tid << std::endl;
        }
    }

    return 0;
}
