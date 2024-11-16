# QUACKER
Group members (3-4 members)  
  Omar Mahmoud  
  Yousef Moussa  
  Nasif Qadri

Quacker 🦆 is a CLI based social media platform that allows users to share posts, follow others, and interact with content, entirely built with **C++** and its corresponding **SQLite3** library. 

---

### **Code Execution Guide**
1. **Prerequisites**:  
   - g++: Ensure the GNU C++ compiler is installed and supports C++17.
   - make: Ensure the make utility is available.
   - libsqlite3: Ensure SQLite3 development headers and libraries are installed on the system

2. **Execution**:  
   - Compile the application using the provided `Makefile`:
   
     ```
     make
     ```
   - Execute the application with a database filename:
     
     ```
     build/quacker <database_filename>
     ```

3. **Testing**:  
   - Run the test script `test/populate_db.py` to populate the database with random test data:
     
     ```
     python3 test/populate_db.py
     ```
