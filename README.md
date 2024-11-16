[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/ZQQhHp7h)
# Assignment3

# CMPUT 291 Mini Project 1 -Fall 2024
Group member names and ccids (3-4 members)  
  omahmoud, Omar Mahmoud  
  ymoussa, Yousef Moussa  
  nqadri, nasif qadri


### **Group Work Break-Down Strategy**
We divided tasks based on strengths and collaboration needs:

- **Nasif Qadri**: Responsible for backend development, including database design and implementation of the `Pond` class.
- **Yousef Moussa**: Focused on frontend development and the user interface within the `Quacker` class.
- **Omar Mahmoud**: Managed testing strategies, the data population script, debugging edge cases, and documenting/commenting code.

However, all of us contributed to different parts of the project, so these roles were not strictly defined.

---

### **Code Execution Guide**
1. **Prerequisites**:  
   - Python 3.x installed  
   - Required libraries (install using `pip install -r requirements.txt`)  
   - SQLite or other database engine setup (as applicable)

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
   - Adjust parameters for the generated data in the `populate_db()` function in the script if needed.

---

### **Collaboration Statement**
We collaborated exclusively within our group. No external collaboration was involved beyond what is allowed by the course policy.

---

# More detail of any AI tool used.
Used for formating our Doxygen comments consistently.
