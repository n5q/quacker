drop table if exists users;
drop table if exists follows;
drop table if exists lists;
drop table if exists include;
drop table if exists tweets;
drop table if exists retweets;
drop table if exists hashtag_mentions;

CREATE TABLE users (
    usr         int,
    name        text,
    email       text,
    phone       int,
    pwd         text,
    primary key (usr)
);

CREATE TABLE follows (
    flwer       int,
    flwee       int,
    start_date  date,
    primary key (flwer,flwee),
    foreign key (flwer) references users(usr) ON DELETE CASCADE,
    foreign key (flwee) references users(usr) ON DELETE CASCADE
);

CREATE TABLE lists (
    owner_id    int,
    lname       text,
    PRIMARY KEY (owner_id, lname),
    FOREIGN KEY (owner_id) REFERENCES users(usr) ON DELETE CASCADE
);

CREATE TABLE include (
    owner_id    int,
    lname       text,
    tid         int,
    PRIMARY KEY (owner_id, lname, tid),
    FOREIGN KEY (owner_id, lname) REFERENCES lists(owner_id, lname) ON DELETE CASCADE,
    FOREIGN KEY (tid) REFERENCES tweets(tid) ON DELETE CASCADE
);

CREATE TABLE tweets (
    tid         int,
    writer_id   int,
    text        text,
    tdate       date, 
    ttime       time,
    replyto_tid int,
    PRIMARY KEY (tid),
    FOREIGN KEY (writer_id) REFERENCES users(usr) ON DELETE CASCADE,
    FOREIGN KEY (replyto_tid) REFERENCES tweets(tid) ON DELETE CASCADE
);

CREATE TABLE retweets (
    tid         int,
    retweeter_id   int, 
    writer_id      int, 
    spam        int,
    rdate       date,
    PRIMARY KEY (tid, retweeter_id),
    FOREIGN KEY (tid) REFERENCES tweets(tid) ON DELETE CASCADE,
    FOREIGN KEY (retweeter_id) REFERENCES users(usr) ON DELETE CASCADE,
    FOREIGN KEY (writer_id) REFERENCES users(usr) ON DELETE CASCADE
);

CREATE TABLE hashtag_mentions (
    tid         int,
    term        text,
    primary key (tid, term),
    FOREIGN KEY (tid) REFERENCES tweets(tid) ON DELETE CASCADE
);