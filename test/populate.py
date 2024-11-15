import sqlite3
from faker import Faker
import random

def populate_database_with_random_data(db_name, user_count=100, tweet_count=500, list_count=200, follow_count=300):
    conn = sqlite3.connect(db_name)
    cursor = conn.cursor()

    # Enable foreign key constraints
    cursor.execute("PRAGMA foreign_keys = ON;")
    
    fake = Faker()
    random.seed(42)

    # Clear existing data (if any)
    tables = ["hashtag_mentions", "retweets", "tweets", "include", "lists", "follows", "users"]
    for table in tables:
        cursor.execute(f"DELETE FROM {table}")
    
    # Generate users
    users = []
    for usr_id in range(1, user_count + 1):
        name = fake.name()
        email = fake.email()
        phone = fake.random_number(digits=10)
        pwd = fake.password()
        users.append((usr_id, name, email, phone, pwd))
    cursor.executemany("INSERT INTO users (usr, name, email, phone, pwd) VALUES (?, ?, ?, ?, ?)", users)
    
    # Generate follows
    for _ in range(follow_count):
        while True:
            flwer = random.randint(1, user_count)
            flwee = random.randint(1, user_count)
            if flwer != flwee:
                start_date = fake.date_between(start_date='-2y', end_date='today')
                try:
                    cursor.execute("INSERT OR IGNORE INTO follows (flwer, flwee, start_date) VALUES (?, ?, ?)", (flwer, flwee, start_date))
                    break
                except sqlite3.IntegrityError:
                    continue  # Retry if insertion fails
    
    # Generate lists
    lists = []
    for _ in range(list_count):
        owner_id = random.randint(1, user_count)
        lname = fake.word()
        lists.append((owner_id, lname))
    cursor.executemany("INSERT INTO lists (owner_id, lname) VALUES (?, ?)", lists)
    
    # Generate tweets
    tweets = []
    hashtags = []
    for tid in range(1, tweet_count + 1):
        writer_id = random.randint(1, user_count)
        text = fake.sentence(nb_words=10)
        tdate = fake.date_between(start_date='-1y', end_date='today')
        ttime = fake.time()
        replyto_tid = random.choice([None] + list(range(1, tid)))  # Randomly decide if it's a reply
        tweets.append((tid, writer_id, text, tdate, ttime, replyto_tid))

        # Random hashtags for the tweet
        for _ in range(random.randint(0, 3)):
            term = f"#{fake.word()}"
            hashtags.append((tid, term))
    cursor.executemany("INSERT INTO tweets (tid, writer_id, text, tdate, ttime, replyto_tid) VALUES (?, ?, ?, ?, ?, ?)", tweets)
    cursor.executemany("INSERT INTO hashtag_mentions (tid, term) VALUES (?, ?)", hashtags)

    # Generate retweets
    retweets = []
    for _ in range(tweet_count // 2):  # About half of tweets are retweeted
        tid = random.randint(1, tweet_count)
        retweeter_id = random.randint(1, user_count)
        while retweeter_id == tweets[tid - 1][1]:  # Avoid self-retweet
            retweeter_id = random.randint(1, user_count)
        writer_id = tweets[tid - 1][1]
        spam = random.randint(0, 1)
        rdate = fake.date_between(start_date='-1y', end_date='today')
        retweets.append((tid, retweeter_id, writer_id, spam, rdate))
    cursor.executemany("INSERT INTO retweets (tid, retweeter_id, writer_id, spam, rdate) VALUES (?, ?, ?, ?, ?)", retweets)

    # Generate include data
    includes = []
    for _ in range(tweet_count // 4):  # Include about 25% of tweets in lists
        owner_id = random.randint(1, user_count)
        possible_lists = [l[1] for l in lists if l[0] == owner_id]
        if possible_lists:  # Ensure there are lists for this user
            lname = random.choice(possible_lists)
            tid = random.randint(1, tweet_count)
            includes.append((owner_id, lname, tid))
    cursor.executemany("INSERT INTO include (owner_id, lname, tid) VALUES (?, ?, ?)", includes)

    conn.commit()
    conn.close()
    print("Database populated with random test data.")

# Replace 'test.db' with the path to your SQLite database
populate_database_with_random_data('test.db')
