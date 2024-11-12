# CMPUT291 - Fall 2024  
## Mini Project 1  (group project)

  

**Due: Nov 15th at 23:30 - LATE SUBMISSIONS ARE ACCEPTED UNTIL 16th Nov 23:30**  

  

##### ****Clarifications**:**

You are responsible for monitoring the course discussion forum in eclass and this section of the project specification for more details or clarifications.

##### ******Updates**:****

28/10/2024: The schema has been updated for table "retweets"  
01/11/2024: Added details in the 4th system functionality.  
07/11/2024: Added more clarifications regarding hashtags in the 1st system functionality.  
07/11/2024: Added more clarifications regarding logouts.  
09/11/2024: Added more clarifications regarding user search  
11/11/2024: Added more clarifications regarding user search  
  

### Introduction

The objectives of this assignment are:

1. To instruct on the use of SQL in a host programming language.

2. To showcase functionalities resulting from the integration of SQL with a host programming language.

Your task in this project is to build a system for managing enterprise data in a database and providing services to users. Data storage will be implemented using a SQLite database, and you will write code in Python (or alternatively Java, C++, etc.) to access it. Your code will create a simple command line interface. While you have the option to implement a GUI interface, **there will be no support or bonus for choosing this route**. You are also free to write your code in Python, Java, C++, or any other language suitable for the task.  **If you opt for a language other than Python, please inform the instructor and your lab TA in advance.**

### Group work policy

You will be doing this project with other partners from the 291 class (i.e., groups of size 3-4). We assume that group members will contribute equally to the project, and as such, they will receive the same mark. In the event of difficulties within a group, where a partner is not carrying their weight, document all your contributions. If there is a break-up, each group member will get credit only for his/her portion of the work completed (losing the mark for any work either not completed or completed by the partners). In case of conflicts or contradicting claims, we may use GitHub commits to reflect each team member’s contribution.

### Database Specification

You are given the following relational schema:

  

-   users(usr, name, email, phone, pwd)
    
-   follows(flwer, flwee, start_date)
    
-   lists(owner_id, lname)
    
-   include(owner_id, lname, tid)
    
-   tweets(tid, writer_id, text, tdate, ttime, replyto_tid)
    
-   retweets(tid, retweeter_id, writer_id, spam, rdate)
    
-   hashtag_mentions(tid,term)
    

The tables are derived from the specification of Assignment 2. Some tables and attributes may have been changed for simplicity.

Each user has a unique user id (usr), in addition to their name, email, phone and password (pwd).

The table "follows" records each following by storing the follower's id (flwer), the followee's id (flwee), and the date of following (start_date).

User tweets are recorded in the table "tweets". Each tweet has a unique identifier (tid), id of the user who wrote the tweet (writer_id), the text, the date (tdate), and the time (ttime). If the tweet is a reply to another tweet, it will store the original tweet's id (replyto_tid)

Table "retweets" records the id of the tweet (tid), id of the retweeter (retweeter_id), and the tweet's writer. It also stores a flag indicating whether it is spam (spam), and the date of the retweet (rdate).

A user's favorite list is stored in the "lists" table that records the user's id (owner_id) and the list name (lname). The include table consists of the user’s id (owner_id), list name (lname), and tweet id (tid).

The SQL commands to create the tables of the system are given below. Use the given schema in your project and do NOT change any table/column names as we will be testing your project using our database with the given schema.

Note: Foreign key constraints must be turned on for each session in SQLite3. Can be done using: PRAGMA foreign_keys = ON;

  

### Login Screen

  

The first screen should provide options for both registered and unregistered users. There must also be an option to exit the program. Registered users should be able to log in using a valid user ID and password, respectively referred to as usr and pwd in table users. Passwords in the user table (column pwd) are stored as plain text. This is for the ease of testing. Any encryption or hashing will complicate the demo process and can result in mark losses.

  

After a registered user signs in, the system should list all tweets  **and**  retweets from users who are being followed; the tweets should be ordered based on date from the latest to the oldest. If there are more than 5 such tweets, only 5 will be shown and the user will be given the option to see more (when prompted it should show the next 5).

  

Unregistered users should be able to sign up by providing a name, email, phone, and password.  **The user ID (i.e. the field usr in table users) should be provided by the system and be unique**. After a successful login or signup, users should be able to perform the subsequent operations (possibly chosen from a menu) as discussed next.

### System Functionalities

After a successful login, users should be able to perform all of the following tasks:

  

1. Search for tweets:  The user should be able to enter one or more keywords and the system should retrieve every tweet that matches at least one of the keywords. A tweet matches a keyword if:

  

-   The keyword has the prefix # and is mentioned by the tweet as a hashtag in the **hashtag_mentions table.** **Hashtags are defined as anything that lies between # and the next whitespace.**
    

OR

-   The keyword doesn't have the prefix # but it appears in the tweet text. (The symbol # is not part of any keyword and only indicates that the keyword that follows is expected to appear as a hashtag).
    

  

The tweets should be ordered based on the latest date to the oldest. If there are more than 5 such tweets, only 5 will be shown and the user will be given the option to see more (when prompted it should show the next 5). The user should be able to select a tweet and see some statistics about the tweet including the number of retweets and the number of replies. Also, the user should be able to compose a reply to a tweet (see the section on composing a tweet), or retweet it (i.e. repost it to all people who follow the user).

  

2. Search for users: The user should be able to enter a keyword and the system should retrieve all users whose names contain the keyword. **A keyword can be interpreted as a sequence of alphanumeric characters.**

**The search query DOES NOT need to match the username exactly, for example, if you search "foy", it should show results: Foy, Malfoy. The outputs should display both the user id and the user name so that users with same names can be distinguished.** The result would be sorted in an ascending order of name length.

  

If there are more than 5 matching users, only 5 would be shown and the user would be given an option to see more but again 5 at a time. The user should be able to select a user and see more information about the user including the number of tweets, the number of users being followed by the user, the number of followers, and up to 3 most recent tweets. The user should be given the option to follow the user or see more tweets.

  

3. Compose a tweet. The user should be able to compose a tweet. A tweet can have hashtags that are marked with a # before each hashtag. Information about hashtags must be stored in table hashtag_mentions. **One tweet can have multiple hashtags but not multiple instances of the same hashtag.**

  

4. List followers. The user should be able to list all users who follow them. From the list, the user should be able to select a follower and see more information about the follower including the number of tweets, the number of users being followed, the number of followers, and up to 3 most recent tweets. The user should be given the option to follow the selected user or see more tweets.

If there are more than 5 followers users, only 5 would be shown and the user would be given an option to see more but again 5 at a time. If there are less than or equal to 5, display them all.  

  

5. Logout. There must be an option to log out of the system. Upon logout, it should NOT exit the program. It should go back to the initial stage where you can login with id and pwd.

String matching: Except for the password which is case-sensitive, all other string matches (including searches for tweets and users) are case-insensitive.  This means jonathan will match Jonathan, JONATHAN, and jonathaN, and you cannot make any assumption on the case of the strings in the database. The database can have strings in uppercase, lowercase, or any mixed format.

SQL injection attacks and password entry: You are expected to counter SQL injection attacks and make the password non-visible at the time of typing.

Error checking: Every good programmer should do some basic error checking to make sure the data entered is correct. We cannot say how much error checking you should or should not do, or detail out all possible checkings. However, we can say that we won't be trying to break down your system but your system also should not break down when the user makes a mistake.

### Testing

At development time, you will be testing your programs with your own data sets but conforming to the project specification. You have been provided a  [small sample DB](https://drive.google.com/file/d/1RUyzCC0IsdflXN10m09dLzTYXO4IWhx0/view?usp=sharing), but you should add more data to it as you deem necessary for your development purpose.

At demo time,  you will be given a database file name that has our test data (e.g., prj-test.db), and you will be passing the file name to your application as a command line argument. Don't hard-code the database name in your application since the database name is not known in advance, and you don't want to change your code at demo time (see next). The database will include the tables given above (created using these  [SQL statements](https://drive.google.com/file/d/1c0VRJHpDsbZF44rteHRZhBu2TL1ZFDd7/view?usp=sharing)) and our own test data. Your application will be tested under a TA account.

  

Every group will book a time slot convenient to all group members to demo their projects. At demo time, all group members must be present. Once we create our tables and populate them with our data, you will be asked to start your application and perform various tasks, showing how your application is handling each task. A mark will be assigned to your demo immediately after the testing.

Here are some important details about our testing process and your choices:

1.  The demo will be run using the source code submitted and nothing else. It is essential to include every file that is needed to compile and run your code.
    
2.  We must be able to compile and run your code under our account on undergrad machines and using our own database. You are not allowed to make any changes to the code without a hefty penalty (read more in the penalty section).
    
3.  Our test data and our test cases will be published after the project due date but before our demo times. This means you have a chance to test your application and learn about possible issues (if any) before your demo time.
    
4.  Before we start your demo, you have the options to (a) run your code following our marking scheme, (b) change your code under the penalty scheme discussed above, or (c) withdraw from the demo.
    

-   Your preferred choice should be running your uploaded code as is. Under this option, your demo mark will be based on how your program performs on our test cases.
    
-   If you decide to change your code, you will be given up to 5 minutes to change anything you wish in your application according to the penalty scheme discussed above. After we start running your application, no change can be made, and your project will be assessed strictly following our marking scheme. For the same reason, it is important to run your application with our test cases in advance and know if your application fails and where it fails.
    
-   You may withdraw from demoing your work for any reason (e.g., you need to make more than 10 line changes to make it work). In such cases, we will assess your project visually and will assign a mark ranging between 0 to 20% for the effort. If you decide to take this option, you have to let us know explicitly either before demo time or at the demo time before attempting to run your application. Otherwise, we will not be evaluating any submission that is not demoed.  **Also, this option is not applicable after we start demoing your application.**
    

5.  Your code cannot be demoed on a laptop (yours or ours) or any machine other than the lab machine with only one exception. The exception is if you are developing your application using a less traditional programming language or tool that is not available on lab machines, you MAY be allowed to demo your application on a laptop. Those cases should be discussed with the instructor well before the project due date and approval must be obtained. Otherwise, you cannot demo your project on any machine other than the lab machines.
    

### Penalties

The penalty is a 5-point deduction for every edit. An edit can be adding a character, removing a character, or replacing a character in your application code. The changes are accumulative, meaning if you add 2 lines to one file and remove 2 lines from another file, your changes will be counted as 4, and you will lose 20 marks.

**Changing more than 5 lines is not allowed, and if that is the only way to run your application, you will get zero on the demo part.**

### Instructions for Submissions

Create your Github repo:

1.  Read the  [Instructions for creating GitHub repositories](https://eclass.srv.ualberta.ca/mod/page/view.php?id=8157667).
    
2.  **Go to [https://classroom.github.com/a/ZQQhHp7h](https://classroom.github.com/a/ZQQhHp7h)  to create your repository. Choose a unique name for your team.**
    
3.  Update the README.md file on your Github repository with your name and ccid and those of your group members.
    
4.  Make sure your teammates join your group's repository.
    
5.  SUBMIT the repository URL through eClass immediately.
    

### Github submission of code

Your code will be uploaded to GitHub. Your README.md file lists the names and ccids of all group members as well as the names of anyone you have collaborated with (as much as it is allowed within the course policy) or a line saying that you did not collaborate with anyone else. This is also the place to acknowledge the use of any source of information besides the course textbook and/or class notes.

The _README.md_ file MUST also include your group work break-down strategy, which must list the break-down of the work items among partners, both the time spent (an estimate) and the progress made by each partner, and your method of coordination to keep the project on track. The README should also include documentation of any decision you have made that is not in the project specification or any coding you have done beyond or different from what is required.

Your _README.md_ will not include the source code. However, your source code will be inspected for source code quality (whether the code is easy to read and if data processing is all done in SQL and not in the application) and self-documentation (whether the code is properly commented). The _README.md_ _Should also include the steps required to run your code._

  

Your repository must also include your design document named designDoc.pdf, a type-written document saved as a PDF. Your design document cannot exceed 4 pages.

The design document should include (a) a general overview of your system with a small user guide, (b) a detailed design of your software with a focus on the components required to deliver the major functions of your application, (c) your testing strategy, and (d) your group work break-down strategy. The general overview of the system gives a high-level introduction and may include a diagram showing the flow of data between different components; this can be useful for both users and developers of your application. The detailed design of your software should describe the responsibility and interface of each primary function or class (not secondary utility functions/classes) and the structure and relationships among them. Depending on the programming language being used, you may have methods, functions, or classes. The testing strategy discusses your general strategy for testing, with the scenarios being tested, the coverage of your test cases, and (if applicable) some statistics on the number of bugs found and the nature of those bugs. The group work strategy must list the breakdown of the work items among partners, both the time spent (an estimate) and the progress made by each partner, and your method of coordination to keep the project on track. The design document should also include documentation of any decision you have made that is not in the project specification or any coding you have done beyond or different from what is required.

  

Your design document will not include the source code. However, your source code will be inspected for source code quality (whether the code is easy to read and if data processing is all done in SQL and not in the application) and self-documentation (whether the code is properly commented).

  

### Marking

Your project will be evaluated under the following schema:

84%: Implementation of the functionalities listed in this specification; this component will be assessed in a demo session.

12%: Documentation and the quality of your source code.

4%: Group coordination and the project breakdown between partners.

###   

### Questions

Please use the following point of contact as the primary recipient and CC the others:

DB spec issues: hmquamra@ualberta.ca, omidvart@ualberta.ca

Schema issues: mhabibol@ualberta.ca, daviran@ualberta.ca

GitHub issues: abinoy@ualberta.ca