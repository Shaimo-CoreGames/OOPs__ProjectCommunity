#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <mysql.h>

using namespace std;

class MySQLHelper {
private:
    static MYSQL* GetConnection() {
        MYSQL* conn = mysql_init(nullptr);
        if (!conn) {
            cerr << "MySQL initialization failed" << endl;
            return nullptr;
        }

        if (!mysql_real_connect(conn, "localhost", "root", "SL@serverroot",
            "shahmeer", 0, nullptr, 0)) {
            cerr << "Connection Error: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return nullptr;
        }

        return conn;
    }

public:
    static bool ExecuteQuery(const string& query) {
        MYSQL* conn = GetConnection();
        if (!conn) return false;

        bool loaded = (mysql_query(conn, query.c_str()) == 0);
        //.c_str() converts the C++ string into a C - style string(const char*), because the MySQL C API function mysql_query() needs a C - style string, not a C++ std::string
        if (!loaded) {
            cerr << "Query Error: " << mysql_error(conn) << endl;
        }

        mysql_close(conn);
        return loaded;
    }

    static bool ExecuteQuery(MYSQL* conn, const string& query) {
        if (!conn) return false;

        bool loaded = (mysql_query(conn, query.c_str()) == 0);
        if (!loaded) {
            cerr << "Query Error: " << mysql_error(conn) << endl;
        }

        return loaded;
    }

    static vector<map<string, string>> Select(const string& query) {
        vector<map<string, string>> results;
        //map<string, string>	A small table that holds(column name ➔ value) for one row
        //vector<map<string, string>> results	A list of rows where each row maps column names to their values
        MYSQL* conn = GetConnection();

        if (!conn)
            return results;

        if (mysql_query(conn, query.c_str()))  // as this condition returns 0 on true
        {
            cerr << "Query Error: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return results;
        }

        MYSQL_RES* result = mysql_store_result(conn);//  gets the full result set (all rows + columns).
        if (!result) {
            mysql_close(conn);
            return results;
        }

        int num_fields = mysql_num_fields(result);
        MYSQL_FIELD* fields = mysql_fetch_fields(result);
        //  fields pointer will points to each column of result

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            map<string, string> record;  // like record["id"] = "1"
            // (key = column name, value = actual value)
            unsigned long* lengths = mysql_fetch_lengths(result);

            for (int i = 0; i < num_fields; i++)
            {
                string fieldName = fields[i].name;
                string value = row[i] ? string(row[i], lengths[i]) : "NULL";
                // row[i] gives you the data(value) at column i (like "Ali" or "20").
                record[fieldName] = value;
            }

            results.push_back(record);
        }

        mysql_free_result(result);
        mysql_close(conn);
        return results;
    }

    static int GetLastInsertedId(MYSQL* conn) {
        if (!conn) return -1;

        int lastId = -1;
        if (mysql_query(conn, "SELECT LAST_INSERT_ID()") == 0) {
            MYSQL_RES* result = mysql_store_result(conn);
            if (result) {
                MYSQL_ROW row = mysql_fetch_row(result);
                if (row && row[0]) {
                    lastId = atoi(row[0]);
                }
                mysql_free_result(result);
            }
        }
        else {
            cerr << "Error getting last ID: " << mysql_error(conn) << endl;
        }

        return lastId;
    }


    static int ExecuteNonQuery(const string& query) {
        MYSQL* conn = GetConnection();
        if (!conn) return 0;

        int affected = 0;
        if (mysql_query(conn, query.c_str()) == 0) {
            affected = mysql_affected_rows(conn);
        }
        else {
            cerr << "Query Error: " << mysql_error(conn) << endl;
        }

        mysql_close(conn);
        return affected;
    }

    static int GetIdByQuery(const string& query) {
        MYSQL* conn = GetConnection();
        if (!conn) return -1;

        int id = -1;
        if (mysql_query(conn, query.c_str()) == 0) // ietration point (returns 0 on true )
        {
            MYSQL_RES* result = mysql_store_result(conn);// result holds a "copy" of all the rows already found by MySQL.
            if (result) {
                MYSQL_ROW row = mysql_fetch_row(result);
                if (row && row[0]) // checking the entire row and row[0] th index for id 
                {
                    id = atoi(row[0]);
                }
                mysql_free_result(result);
            }
        }
        else {
            cerr << "Query Error: " << mysql_error(conn) << endl;
        }

        mysql_close(conn);
        return id;
    }

    static string GetStringByQuery(const string& query) {
        MYSQL* conn = GetConnection();
        if (!conn) return "";

        string resultStr;
        if (mysql_query(conn, query.c_str()) == 0) {
            MYSQL_RES* result = mysql_store_result(conn);
            if (result) {
                MYSQL_ROW row = mysql_fetch_row(result);
                if (row && row[0]) {
                    resultStr = row[0];
                }
                mysql_free_result(result);
            }
        }
        else {
            cerr << "Query Error: " << mysql_error(conn) << endl;
        }

        mysql_close(conn);
        return resultStr;
    }
};



class Person {
public:
    int age;
    string name;

public:
    // Constructor
    Person() {}
    Person(int a, const string& n) {
        age = a;
        name = n;
    }

    // Getters
    int getAge() const {
        return age;
    }

    string getName() const {
        return name;
    }

    // Method to display the data
    void display() const {
        cout << "age: " << age << ", Name: " << name << endl;
    }
};


int main() {

    /*
    *  int id;
     string name;
     int age;
     string grade;
     cout << "Enter ID: ";
     cin >> id;
     cout << "Enter name: ";
     cin >> name;
     cout << "Enter age: ";
     cin >> age;
     cout << "Enter grade (like A, B+, etc.): ";
     cin >> grade;
     if (MySQLHelper::ExecuteQuery(q1)) {
         cout << "Data Inserted Successfully!" << endl;
     }
     else {
         cout << "Failed to insert data." << endl;
     }*/


     /*string q2 = "ALTER TABLE students MODIFY id INT AUTO_INCREMENT ";
     if (MySQLHelper::ExecuteQuery(q2)) {
         cout << "Table modified: id is now AUTO_INCREMENT!" << endl;
     }
     else {
         cout << "Failed to modify table." << endl;
     }*/
     /* string q2 = "ALTER TABLE students ADD Email varchar(255)  ";
      if (MySQLHelper::ExecuteQuery(q2)) {
          cout << "Table modified: New Column is added!" << endl;
      }
      else {
          cout << "Failed to modify table." << endl;
      }
  */


  /* string q3 = "SELECT * FROM students";
   vector<map<string, string>> results = MySQLHelper::Select(q3); // <<< FIXED HERE!

   for (const auto& record : results) // record is one row from results
   {
       for (const auto& field : record) // Each field is one column(key,value pair) of that row.
       {
           cout << field.first << ": " << field.second << " | ";
       }
       cout << endl;
   } */
   /*
   int id;
   string name;
   int age;
   string grade;

 cout << "Enter id: ";
   cin >> id;
   cout << "Enter name: ";
   cin >> name;
   cout << "Enter age: ";
   cin >> age;
   cout << "Enter grade (like A, B+, etc.): ";
   cin >> grade;

   string q4 = "INSERT INTO students ( name, age, grade) VALUES ('" + name + "', " + to_string(age) + ", '" + grade + "')";

   MYSQL* conn = MySQLHelper::GetConnection();
   if (!conn) {
       cerr << "Connection failed!" << endl;
       return 1;
   }

   if (MySQLHelper::ExecuteQuery(conn, q4)) {
       int lastId = MySQLHelper::GetLastInsertedId(conn);
       cout << "Last inserted ID = " << lastId << endl;
   }


   mysql_close(conn);
   */
   /*
   int affected=MySQLHelper::ExecuteNonQuery("INSERT INTO students (name, age,grade) VALUES ('Ali', 22,'A')");
   if (affected > 0) {
       cout << "✅ Insert successful! Rows affected: " << affected << endl;
   }
   else {
       cout << "❌ Insert failed!" << endl;
   }
   */

   /* int id = MySQLHelper::GetIdByQuery("Select id from students where name='Sara'");
     cout << id;
     */

     /* string email = MySQLHelper::GetStringByQuery("SELECT email FROM students WHERE id = 10");
     if (!email.empty()) {
         cout << "Found student email: " << email << endl;
     }
     else {
         cout << "Student not found!" << endl;
     }
     */

     /* bool temp=MySQLHelper::ExecuteQuery("CREATE TABLE IF NOT EXISTS students(id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(50))");
        if (temp)
            cout << "New Table created! ";
        else
            cout << "Table already exists !";*/


            // Initialize MySQL library
          /*  if (mysql_library_init(0, nullptr, nullptr)) {
                cerr << "Could not initialize MySQL library" << endl;
                return 1;
            */


            /* string name ;
             int age = 45;
             cout << "ENter Name: ";
             cin >> name;
             std::string query = "INSERT INTO test (name,age) VALUES('" + name +"'," + to_string(age) + ")";
             string query = "alter table test add column age int;";
             string query = "Delete from test where id=3";*/

             /*string query = "Update students set age=23 where id =2";
             MySQLHelper::ExecuteQuery(query);

             cout << "Last inserted ID: " << MySQLHelper::GetLastInsertedId() << endl;
         */


    Person* arrayPerson = new Person[10];  // dynamic array
    int count = 0;

    auto results = MySQLHelper::Select("SELECT * FROM students");

    for (const auto& rw : results) {
        if (count >= 10)
            break; // prevent out-of-bounds

        int age = stoi(rw.at("age")); // convert age string into int
        string name = rw.at("name");

        // Either use setters (if Person has them)
        /*arrayPerson[count].age=age;
        arrayPerson[count].name=name;*/

        // OR use a temporary object if you want to use the constructor
        arrayPerson[count] = Person(age, name);
        count++;
    }
    cout << arrayPerson[2].age << arrayPerson[3].age;


    //Clean up MySQL library

    mysql_library_end();
    return 0;
}
