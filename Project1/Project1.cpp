#include <iostream>
#include <iomanip>// setw()
#include <windows.h>
#include <algorithm> // all_of
#include <string>
#include <vector>
#include <map>
#include <mysql.h>

using namespace std;

void showEVotingFrontPage() {
    system("cls");
    cout << "                                 ========================================================================\n";
    cout << "\033[32m\n                               -_- -_- -_- -_- -_-  \033[0m";
    cout << "ELECTRONIC VOTING SYSTEM - PAKISTAN)";
        cout << "\033[32m-_- -_- -_- -_- -_-\033[0m\n\n";
    cout << "                                                         _______       " << endl;
    cout << "                                                        |       |      " << endl;
    cout << "                                                        | VOTE  |      " << endl;
    cout << "                                                        |_______|      " << endl;
    cout << "                                                           ||          " << endl;
    cout << "                                                           ||          " << endl;
    cout << "                                                         __||__        " << endl;
    cout << "                                                        |_____|        " << "\033[33m   << Drop your vote here\033[0m" << endl;
    cout << "                                                        /     \\       " << endl;
    cout << "                                                       |_______|       " << endl;
    cout << "\n\033[32m                                                     Empowering Democracy with Technology!\033[0m\n";
    cout << "                                 ========================================================================\n";
}

class MySQLHelper
{
private:
    static MYSQL* GetConnection()
    {
        MYSQL* conn = mysql_init(nullptr);
        if (!conn)
        {
            cerr << "MySQL initialization failed" << endl;
            return nullptr;
        }
        if (!mysql_real_connect(conn, "localhost", "root", "SL@serverroot", "EVoting", 0, nullptr, 0))
        {
            cerr << "Connection Error: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return nullptr;
        }
        return conn;
    }

public:
    static bool ExecuteQuery(const string& query)
    {
        MYSQL* conn = GetConnection();
        if (!conn)
            return false;
        bool loaded = (mysql_query(conn, query.c_str()) == 0);
        if (!loaded)
            cerr << "Query Error: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return loaded;
    }
    static vector<map<string, string>> Select(const string& query)
    {
        vector<map<string, string>> results;
        MYSQL* conn = GetConnection();
        if (!conn)
            return results;
        if (mysql_query(conn, query.c_str()))
        {
            cerr << "Query Error: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return results;
        }
        MYSQL_RES* result = mysql_store_result(conn);//main point( Server sa query ka result retrieve karna .)
        if (!result)
        {
            mysql_close(conn);
            return results;
        }
        int num_fields = mysql_num_fields(result);
        MYSQL_FIELD* fields = mysql_fetch_fields(result);
        MYSQL_ROW row; // MYSQL_ROW ka matlab hai ( array of column values )
        while ((row = mysql_fetch_row(result)))
        {
            map<string, string> record;
            unsigned long* lengths = mysql_fetch_lengths(result);
            for (int i = 0; i < num_fields; i++)
            {
                string fieldName = fields[i].name;
                string value = row[i] ? string(row[i], lengths[i]) : "NULL";
                record[fieldName] = value;
            }
            results.push_back(record);
        }
        mysql_free_result(result);
        mysql_close(conn);
        return results;
    }
	static MYSQL_RES* ExecuteSelect(const string& query)
	{
		MYSQL* conn = GetConnection();
		if (!conn)
			return nullptr;
		if (mysql_query(conn, query.c_str()))
		{
			cerr << "Query Error: " << mysql_error(conn) << endl;
			mysql_close(conn);
			return nullptr;
		}
		MYSQL_RES* result = mysql_store_result(conn);
		if (!result)
		{
			mysql_close(conn);
			return nullptr;
		}
		return result;
	}


};

// ======================= Base Classes =======================
// Base User Class
class User {
protected:
    int id;
    string username;
public:
    virtual void menu() = 0;

    // Admin Signup
    bool adminSignup() {
        string checkQuery = "SELECT COUNT(*) AS count FROM Users WHERE role = 'admin'";
        auto result = MySQLHelper::Select(checkQuery);

        // No admin exists, proceed with admin signup
        if (!result.empty() && stoi(result[0]["count"]) == 0) {
            cout << "Enter Username: ";
            getline(cin, username);
            string pwd;
            cout << "Enter Password: ";
            getline(cin, pwd);

            string insertQuery = "INSERT INTO Users (username, password, role) VALUES ('" + username + "', '" + pwd + "', 'admin')";
            if (MySQLHelper::ExecuteQuery(insertQuery)) {
                cout << "___________________-_-__________________\n";
                cout << "\033[32m \n ---! Administrator profile created successfully !---\n\033[0m";
                cout << "________________________________________\n";
                cout << "                                                        Tap ENTER to proceed to the menu...\n";
                cin.get();
                return true;
            }
            else {
                cout << "\033[31m\n     ?__Unable to complete admin account creation__?\n\033[0m";
                cout << "                                                        Tap ENTER to proceed to main menu...\n";
                cin.get();
                return false;
            }
        }
        cout << "\033[31m\n     ?__Admin Account already Exist__?\n\033[0m";
        cout << "                                                        Tap ENTER to proceed to main menu...\n";
        cin.get();     
        return false;
    }

    // Admin Login
    bool adminLogin() {
        string checkQuery = "SELECT COUNT(*) AS count FROM Users WHERE role = 'admin'";
        auto result = MySQLHelper::Select(checkQuery);

        if (!result.empty() && stoi(result[0]["count"]) == 0) {
            // No admin exists, call adminSignup() to create one
            cout << "\033[32mAdmin account missing. Set up a new administrator First:\n\033[0m";
            return adminSignup(); // Proceed with admin signup if none exists
        }

        // Admin exists, proceed with regular login
        cout << "______Admin Login\n";
        cout << "Enter Username: ";
        getline(cin, username);
        string pwd;
        cout << "Enter Password: ";
        getline(cin, pwd);

        string query = "SELECT id FROM Users WHERE username='" + username + "' AND password='" + pwd + "' AND role='admin'";
        auto resultLogin = MySQLHelper::Select(query);

        if (!resultLogin.empty()) {
            id = stoi(resultLogin[0]["id"]);
            cout << "___________________-_-__________________\n";
            cout << "\033[32m \n ---! Admin Login Successfully !---\n\033[0m";
            cout << "________________________________________\n";
            cout << "                                                        Tap ENTER to proceed to the menu...\n";
            cin.ignore();
            return true;
        }
        else {
            cout << "\033[31m\n     ?__LogIn failed__?\n\033[0m";
            cout << "                                                        Tap ENTER to proceed to the main menu...\n";
            cin.get();          
            return false;
        }
    }

    // Voter Signup
    bool voterSignup() {
        string password, cnic;

        cout << "Signup as Voter\n";
        cout << "Enter CNIC (13 digits): ";
        getline(cin, cnic);

        if (cnic.length() != 13 || !all_of(cnic.begin(), cnic.end(), ::isdigit)) {
            cout << "\033[31m\n                                                   CNIC format invalid—it should be 13 digits long.\n\033[0m";
            cout << "                                                        Tap ENTER to proceed to the main menu...\n";
            cin.get();
            return false;
        }

        string checkCNICQuery = "SELECT id FROM Users WHERE cnic = '" + cnic + "'";
        auto existing = MySQLHelper::Select(checkCNICQuery);
        if (!existing.empty()) {
            cout << "\033[31m\n     ?__CNIC Already Exists__?\n\033[0m";
            cout << "                                                    Tap ENTER to proceed to the main menu...\n";
            cin.get();
            return false;
        }

        cout << "Enter Username: ";
        getline(cin, username);
        cout << "Enter Password: ";
        getline(cin, password);
        int province_id = 0, district_id = 0;

        auto provs = MySQLHelper::Select("SELECT id,name FROM Provinces");
        cout << "\033[32m\n_____________* Explore the Provinces Below: *\n\033[0m";
        for (auto& p : provs)
            cout << p.at("id") << ". " << p.at("name") << "\n";

        cout << "Select Province ID: "; cin >> province_id;
        cin.ignore();
        cout << "\033[32m\n_____________* Choose Your District *\n\033[0m";
        auto dists = MySQLHelper::Select("SELECT id,name FROM Districts WHERE province_id=" + to_string(province_id));
        cout << "Districts:\n";
        for (auto& d : dists)
            cout << d.at("id") << ". " << d.at("name") << "\n";

        cout << "Select District ID: "; cin >> district_id;
        cin.ignore();

        string insertQuery = "INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('"
            + username + "', '" + password + "', 'voter', '" + cnic + "', "
            + to_string(province_id) + ", " + to_string(district_id) + ")";

        if (MySQLHelper::ExecuteQuery(insertQuery)) {
            cout << "___________________-_-__________________\n";
            cout << "\033[32m \n ---! Voter SignedUp Successfully !---\n\033[0m";
            cout << "________________________________________\n";
            cout << "                                                        Tap ENTER to further proceed...\n";
            cin.ignore();
            return true;
        }
        else {
            cout << "\033[31m\n     ?__Account creation failed__?\n\033[0m";
            cout << "                                                        Tap ENTER to proceed to the main menu...\n";
            cin.get();
            return false;
        }
    }

    // Voter Login
    bool voterLogin() {
        string password, cnic;

        cout << "______Login as Voter\n";
        cout << "Enter Username: ";
        getline(cin, username);
        cout << "Enter Password: ";
        getline(cin, password);
        cout << "Enter CNIC (13 digits): ";
        getline(cin, cnic);

        if (cnic.length() != 13 || !all_of(cnic.begin(), cnic.end(), ::isdigit)) {
            cout << "\033[31m\n                                                   CNIC format invalid—it should be 13 digits long.\n\033[0m";
            cout << "                                                        Tap ENTER to proceed to the main menu...\n";
            cin.get();
            return false;
        }

        string query = "SELECT id FROM Users WHERE username='" + username + "' AND password='" + password + "' AND cnic='" + cnic + "' AND role='voter'";
        auto result = MySQLHelper::Select(query);

        if (!result.empty()) {
            id = stoi(result[0]["id"]);
            cout << "___________________-_-__________________\n";
            cout << "\033[32m \n ---! Voter LoggedIn Successfully !---\n\033[0m";
            cout << "________________________________________\n";
            cout << "                                                        Tap ENTER to proceed to the menu...\n";
            cin.ignore();
            return true;
        }
        else {
            cout << "\033[31m\n     ?__Account LogIn failed__?\n\033[0m";
            cout << "                                                        Tap ENTER to proceed to the main menu...\n";
            cin.get();
            return false;
        }
    }
};

// -----------------------------------------------__Admin Class__-----------------------------------------------
class Admin : public User {
public:
    void menu() override {
        int choice;
        do {
            cout << "\n*Admin Menu:\n";
            cout << "\033[32m\n--<<<<<<<< things to be done >>>>>>>>--\033[0m";
            cout << "\n         -> 1. view  Election\n";
            cout << "         -> 2. Start  Election\n";
            cout << "         -> 3. Add Candidate\n";
            cout << "         -> 4. Add Districts\n";
            cout << "         -> 5. Add Province\n";
            cout << "         -> 6. End  Election\n";
            cout << "         -> 7. View  Results\n";
            cout << "         -> 8. Logout\n";
            cout << "\033[32m\n<<<<<<<<------------------->>>>>>>>\033[0m";
            cout << ("\033[32m\n                                                           --> Enter Your Choice: \033[0m");
            cin >> choice;
            switch (choice) {
            case 1: viewElections(); break;
            case 2: startElection(); break;
            case 3: addCandidate(); break;
            case 4: addDistrict(); break;
            case 5: addProvince(); break;
            case 6: endElection(); break;
            case 7: viewResults(); break;
            case 8: cout << "Logging out...\n"; break;
                cout << "\033[31m\n     ?__Invalid Choice__?\n\033[0m";
                cout << "                                                        Tap ENTER to proceed to the main menu...\n";
                cin.get();
            }
        } while (choice != 7);
    }
    void viewElections()
    {
        auto elections = MySQLHelper::Select("SELECT * FROM Elections");
        cout << "\033[32m\n_____________* Elections *\n\033[0m";
        for (const auto& e : elections)
        {
            cout << "\033[36m===========================================\033[0m\n";
            cout << "\033[33mElection ID:\033[0m      " << e.at("id") << "\n";
            cout << "\033[33mTitle:\033[0m            " << e.at("title") << "\n";
            cout << "\033[33mType:\033[0m             " << e.at("type") << "\n";
            cout << "\033[33mDate:\033[0m             " << e.at("date") << "\n";
            cout << "\033[33mStatus:\033[0m           " << e.at("status") << "\n";
            cout << "\033[33mStart Time:\033[0m       " << (e.at("start_time").empty() ? "N/A" : e.at("start_time")) << "\n";
            cout << "\033[33mEnd Time:\033[0m         " << (e.at("end_time").empty() ? "N/A" : e.at("end_time")) << "\n";
            cout << "\033[36m===========================================\033[0m\n\n";
            cout << "                                                        Tap ENTER to proceed to the main menu...\n";
            cin.get();
        }
    }
   
    void startElection() {
        // Check if an ongoing election already exists
        string checkQuery = "SELECT COUNT(*) FROM Elections WHERE status = 'Ongoing'";
        MYSQL_RES* res = MySQLHelper::ExecuteSelect(checkQuery);

        if (res) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row && atoi(row[0]) > 0) {
                cout << "\033[31m\n     ?__An election is already ongoing__?\n\033[0m";
                mysql_free_result(res);
            }
            else {
                mysql_free_result(res);
                // Start the election
                string query = "INSERT INTO Elections (id,title, type, date, status, start_time) "
                    "VALUES (1,'National Assembly Election', 'General', '2025-05-11', 'Ongoing', NOW())";
                if (MySQLHelper::ExecuteQuery(query)) {
                    cout << "\033[32m\n---! Election Started Successfully !---\033[0m\n";
                    cout << "                                                        Tap ENTER to proceed to the menu...\n";
                    cin.ignore();
                    cin.get();
                }
                else {
                    cout << "\033[31m\n     ?__Error Starting Election__?\n\033[0m";
                    cout << "                                                        Tap ENTER to proceed to the menu...\n";
                    cin.ignore();
                    cin.get();
                }
            }
        }
        else {
            cout << "\033[31m\n     ?__Database Error During Check__?\n\033[0m";
            cout << "                                                        Tap ENTER to proceed to the main menu...\n";
            cin.ignore();
            cin.get();
        }
       
    }

    void endElection() {
    // Check if there is an ongoing election to end
    string checkQuery = "SELECT COUNT(*) FROM Elections WHERE status = 'Ongoing'";
    MYSQL_RES* res = MySQLHelper::ExecuteSelect(checkQuery);
    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row && atoi(row[0]) == 0) {
            cout << "\033[31m\n     ?__No ongoing election to end__?\n\033[0m";

            mysql_free_result(res);
        } else {
            mysql_free_result(res);
            // End the most recent ongoing election
            string query = "UPDATE Elections "
                "SET status = 'Ended', end_time = NOW() "
                "WHERE status = 'Ongoing' AND id=1";


            if (MySQLHelper::ExecuteQuery(query)) {
                cout << "\033[32m\n---! Election Ended Successfully !---\033[0m\n";
                cout << "                                                        Tap ENTER to proceed to the menu...\n";
                cin.ignore();
                cin.get();
            } else {
                cout << "\033[31m\n     ?__Error Ending Election__?\n\033[0m";
                cout << "                                                        Tap ENTER to proceed to the menu...\n";
                cin.ignore();
                cin.get();
            }
        }
    }
    else {
        cout << "\033[31m\n     ?__Database Error During Check__?\n\033[0m";
    cout << "                                                        Tap ENTER to proceed to the menu...\n";
    cin.get();
    }
}
 
    const int CANDIDATE_LIMIT = 5;  // Maximum number of candidates that we  allowed
    void addCandidate() {
        string name, party, type;
        cin.ignore(); 
        cout << "Enter Candidate Name: "; 
        getline(cin, name);
        cout << "Enter Party: "; 
        getline(cin, party);
        cout << "----Type (MPA/MNA): "; 
        getline(cin, type);

        // Query to count current candidates in the database
        string countQuery = "SELECT COUNT(*) AS candidate_count FROM Candidates WHERE type='" + type + "'";
        auto countResult = MySQLHelper::Select(countQuery);

        int candidateCount = 0;
        if (!countResult.empty()) {
            candidateCount = stoi(countResult[0]["candidate_count"]);
        }

        // Check if the number of candidates has reached the limit
        if (candidateCount >= CANDIDATE_LIMIT) {
            cout << "\033[31m\n     ?__Your candidates have reached the limit!__?\n\033[0m";
            cout << "                                                        Tap ENTER to proceed to the menu...\n";
            cin.ignore();
            cin.get();
            return;  // Exit the function without adding a new candidate
        }

        int province_id = 0, district_id = 0;

        if (type == "MPA") {
            auto provs = MySQLHelper::Select("SELECT id,name FROM Provinces");
            cout << "\033[32m\n_____________* Explore the Provinces Below: *\n\033[0m";
            for (auto& p : provs)
                cout << p.at("id") << ". " << p.at("name") << "\n";

            cout << "Select Province ID: "; 
            cin >> province_id;
            cin.ignore(); 

            auto dists = MySQLHelper::Select("SELECT id,name FROM Districts WHERE province_id=" + to_string(province_id));
            cout << "\033[32m\n___________* Choose Your District *\n\033[0m";
            for (auto& d : dists)
                cout << d.at("id") << ". " << d.at("name") << "\n";

            cout << "Select District ID: "; cin >> district_id;
            cin.ignore(); 

            // Insert with district_id for MPA
            string q = "INSERT INTO Candidates (name,party,type,province_id,district_id) VALUES ('" +
                name + "', '" + party + "', '" + type + "', " + to_string(province_id) + "," + to_string(district_id) + ")";
            if (MySQLHelper::ExecuteQuery(q)) 
                cout << "Candidate added.\n";
            else 
                cout << "\033[31m\n     ?__Error adding candidate__?\n\033[0m";
            cout << "                                                        Tap ENTER to proceed to the menu...\n";
            cin.get();

        }
        else if (type == "MNA") {
            auto provs = MySQLHelper::Select("SELECT id,name FROM Provinces");
            cout << "\033[32m\n_____________* Explore the Provinces Below: *\n\033[0m";
            for (auto& p : provs) cout << p.at("id") << ". " << p.at("name") << "\n";

            cout << "Select Province ID: "; cin >> province_id;
            cin.ignore(); // <<< Ignore leftover newline

            // Insert without district_id for MNA (set district_id NULL)
            string q = "INSERT INTO Candidates (name,party,type,province_id,district_id) VALUES ('" +
                name + "','" + party + "','" + type + "'," + to_string(province_id) + ", NULL)";
            if (MySQLHelper::ExecuteQuery(q))
            {
                cout << "___________________-_-__________________\n";
                cout << "\033[32m \n ---! Candidate Added Successfully !---\n\033[0m";
                cout << "________________________________________\n";
                cout << "                                                        Tap ENTER to proceed to the menu...\n";
                cin.ignore();
            }
            else
                cout << "\033[31m\n     ?__Error adding candidate__?\n\033[0m";
            cout << "                                                        Tap ENTER to proceed to the menu...\n";
            cin.get();
        }
        else {
            cout << "\033[31m\n     ?__Unknown Type__?\n\033[0m";
            cout << "                                                        Tap ENTER to proceed to the menu...\n";
            cin.get();
        }
    }

    void viewResults() {
        // Check if election has ended
        string statusQuery = "SELECT status, DATE_FORMAT(start_time, '%Y-%m-%d %H:%i:%s') AS start_time, DATE_FORMAT(end_time, '%Y-%m-%d %H:%i:%s') AS end_time FROM Elections where id=1";
        auto electionStatus = MySQLHelper::Select(statusQuery);
      
        if (electionStatus.empty()) {
            cout << "\033[31m\nNo election record found!\n\033[0m";
            char choice;
            cout << "                                                        Press A to return to Admin Menu or Enter for Main Menu...\n";
            cin >> choice;
            cin.ignore();
            if (choice == 'A' || choice == 'a') {
                menu(); // Or whatever your actual admin menu function is named
            }
            else
                return;
        }

        string status = electionStatus[0]["status"];
        string startTime = electionStatus[0]["start_time"];
        string endTime = electionStatus[0]["end_time"];

        // Show election timing info
        cout << "\033[34m\nElection Start Time: " << startTime << "\033[0m";
        if (!endTime.empty())
            cout << "\033[34mn\nElection End Time: " << endTime << "\033[0m\n";
        else
            cout << "\033[33m | (Election is still ongoing...)\033[0m\n";

        if (status != "Ended") {
            cout << "\033[31m\nResults cannot be displayed until the election has ended.\n\033[0m";
            char choice;
            cout << "                                                        Press A to return to Admin Menu or Enter for Main Menu...\n";
            cin >> choice;
            cin.ignore();
            if (choice == 'A' || choice == 'a') {
                menu(); // Or whatever your actual admin menu function is named
            }
            else
                return;
        }

        // Continue with results display as you've written...

        cout << "\033[32m\n_____________* MPA Winners by District *\n\033[0m";
        string mpa_q =
            "SELECT d.name AS region, c.name AS candidate, c.party, mv.votes FROM "
            "(SELECT district_id, candidate_id, COUNT(*) AS votes FROM Votes WHERE vote_type='MPA' GROUP BY district_id,candidate_id) mv "
            "JOIN (SELECT district_id AS did, MAX(votes) AS max_votes FROM "
            "(SELECT district_id, candidate_id, COUNT(*) AS votes FROM Votes WHERE vote_type='MPA' GROUP BY district_id,candidate_id) t GROUP BY district_id) mx "
            "ON mv.district_id=mx.did AND mv.votes=mx.max_votes "
            "JOIN Candidates c ON mv.candidate_id=c.id "
            "JOIN Districts d ON mv.district_id=d.id;";
        auto mpa_res = MySQLHelper::Select(mpa_q);
        for (auto& r : mpa_res) {
            cout << "\033[33mDistrict: \033[0m" << setw(15) << left << r.at("region")
                << " | \033[35mCandidate: \033[0m" << setw(20) << left << r.at("candidate")
                << " | \033[33mParty: \033[0m" << setw(15) << left << r.at("party")
                << " | \033[35mVotes: \033[0m" << r.at("votes") << "\n";
        }

        cout << "\033[32m\n_____________* MNA Winners by District *\n\033[0m";
        string mna_q =
            "SELECT p.name AS region, c.name AS candidate, c.party, mv.votes FROM "
            "(SELECT province_id, candidate_id, COUNT(*) AS votes FROM Votes WHERE vote_type='MNA' GROUP BY province_id,candidate_id) mv "
            "JOIN (SELECT province_id AS pid, MAX(votes) AS max_votes FROM "
            "(SELECT province_id, candidate_id, COUNT(*) AS votes FROM Votes WHERE vote_type='MNA' GROUP BY province_id,candidate_id) t GROUP BY province_id) mx "
            "ON mv.province_id=mx.pid AND mv.votes=mx.max_votes "
            "JOIN Candidates c ON mv.candidate_id=c.id "
            "JOIN Provinces p ON mv.province_id=p.id;";
        auto mna_res = MySQLHelper::Select(mna_q);
        for (auto& r : mna_res)
            cout << "\033[33mProvince: \033[0m" << setw(15) << left << r.at("region")
            << " | \033[35mCandidate: \033[0m" << setw(20) << left << r.at("candidate")
            << " | \033[33mParty: \033[0m" << setw(15) << left << r.at("party")
            << " | \033[35mVotes: \033[0m" << r.at("votes") << "\n";


        //------------------------------------------*****-- Final Selection --*****-----------------------------------------
        cout << "\033[32m\n_____________* Chief Minister Selection *\n\033[0m";
        string cm_q =
            "WITH mpa_votes AS ("
            "  SELECT d.province_id, v.candidate_id, COUNT(*) AS votes "
            "  FROM Votes v JOIN Districts d ON v.district_id=d.id "
            "  WHERE vote_type='MPA' GROUP BY d.province_id, v.candidate_id"
            "), prov_max AS ("
            "  SELECT province_id, MAX(votes) AS max_votes "
            "  FROM mpa_votes GROUP BY province_id"
            ")"
            "  SELECT p.name AS province, c.name AS candidate, c.party, mv.votes"
            "  FROM mpa_votes mv"
            "  JOIN prov_max pm ON mv.province_id=pm.province_id AND mv.votes=pm.max_votes"
            "  JOIN Candidates c ON mv.candidate_id=c.id"
            "  JOIN Provinces p ON mv.province_id=p.id;";  
        auto cm_res = MySQLHelper::Select(cm_q);
        for (auto& r : cm_res)
            cout << "\033[33mProvince: \033[0m" << setw(15) << left << r.at("province")
            << " | \033[35mChief Minister: \033[0m" << setw(15) << left << r.at("candidate")
            << " | \033[33mParty: \033[0m" << setw(15) << left << r.at("party")
            << " | \033[35mVotes: \033[0m" << r.at("votes") << "\n";
        cout << "\033[32m\n_____________* Prime Minister Selection *\n\033[0m";
        string pm_q =      
            "SELECT p.name AS province, c.name AS candidate, c.party, mv.votes FROM "
            "(SELECT candidate_id, COUNT(*) AS votes FROM Votes WHERE vote_type='MNA' GROUP BY candidate_id) mv "
            "JOIN Candidates c ON mv.candidate_id=c.id "
            "JOIN Provinces p ON c.province_id=p.id "
            "ORDER BY mv.votes DESC LIMIT 1;";
        auto pm_res = MySQLHelper::Select(pm_q);
        if (!pm_res.empty())
        {
            auto& r = pm_res[0];
            cout << "\033[33mProvince: \033[0m" << setw(15) << left << r.at("province")
                << " | \033[35mPrime Minister: \033[0m" << setw(15) << left << r.at("candidate")
                << " | \033[33mParty: \033[0m" << setw(15) << left << r.at("party")
                << " | \033[35mVotes: \033[0m" << r.at("votes") << "\n";
        }
        cout << "\n                                                        Tap ENTER to proceed to the main menu...\n";
        cin.ignore();
        cin.get();
    }
  
    void addDistrict() {
        string name;
        int province_id;

        auto provs = MySQLHelper::Select("SELECT id, name FROM Provinces");
        if (provs.empty()) {
            cout << "\033[32mNo provinces available. Please add a province first.\n\033[0m";
            return;
        }

        cout << "Provinces:\n";
        for (auto& p : provs)
            cout << p["id"] << ". " << p["name"] << "\n";

        cout << "Select Province ID to add districts to: ";
        cin >> province_id;
        cin.ignore();

        while (true) {
            // Check how many districts already exist for this province
            string countQuery = "SELECT COUNT(*) AS cnt FROM Districts WHERE province_id = " + to_string(province_id);
            auto res = MySQLHelper::Select(countQuery);

            int currentCount = (!res.empty()) ? stoi(res[0]["cnt"]) : 0;

            if (currentCount >= 5) {
                cout << "\033[31m\n     ?__This province already has 5 districts. Limit reached__?\n\033[0m";
                cout << "                                                        Tap ENTER to proceed to the menu...\n";
                cin.get();
                break;
            }

            cout << "Enter District Name: ";
            getline(cin, name);

            string query = "INSERT INTO Districts (name, province_id) VALUES ('" + name + "', " + to_string(province_id) + ")";
            if (MySQLHelper::ExecuteQuery(query))
            { cout << "\033[32m \n ---! District Added Successfully !---\n\033[0m";
            cout << "                                                        Tap ENTER to proceed to the menu...\n";
            cin.get();
        }
            else
                cout << "Error adding district.\n";
            cout << "                                                        Tap ENTER to proceed to the menu...\n";
            cin.get();
            // Ask user if they want to add another (optional)
            char choice;
            cout << "Do you want to add another district to this province? (y/n): ";
            cin >> choice;
            cin.ignore();

            if (choice != 'y' && choice != 'Y') {
                break;
            }
        }
    }

    void addProvince() {
        string name;

        while (1) {
            // Check total number of provinces
            auto res = MySQLHelper::Select("SELECT COUNT(*) AS cnt FROM Provinces");
            int currentCount = (!res.empty()) ? stoi(res[0]["cnt"]) : 0;

            if (currentCount >= 5) {
                cout << "\033[31m\n     ?__Maximum of 5 provinces already added__?\n\033[0m";
                cout << "                                                        Tap ENTER to proceed to the menu...\n";
                cin.get();
                break;
            }

            cout << "Enter Province Name: ";
            cin.ignore();
            getline(cin, name);

            string query = "INSERT INTO Provinces (name) VALUES ('" + name + "')";
            if (MySQLHelper::ExecuteQuery(query))
            {
                cout << "\033[32m \n ---! Province Added Successfully !---\n\033[0m";
                cout << "                                                        Tap ENTER to proceed to the menu...\n";
                cin.get();
            }
            else
                cout << "\033[31m\n     ?__Error while Adding Province__?\n\033[0m";
            cout << "                                                        Tap ENTER to proceed to the menu...\n";
            cin.get();
            // Ask if user wants to add another province
            char choice;
            cout << "Do you want to add another province? (y/n): ";
            cin >> choice;
            if (choice != 'y' && choice != 'Y') {
                break;
            }

            cin.ignore(); // Clear newline before next input
        }
    }


};

// -----------------------------------------------__Voter Class__-----------------------------------------------
class Voter : public User
{
public:
    void menu()override
    {
        int choice;
        do
        {
            cout << "\n*Voter Menu:\n";
            cout << "\033[32m\n--<<<<<<<< things to be done >>>>>>>>--\033[0m";
            cout << "\n         -> 1. View Elections\n";
            cout << "         -> 2. View  Candidates\n";
            cout << "         -> 3. Cast Vote\n";
            cout << "         -> 4. Logout\n";
            cout << "\033[32m\n<<<<<<<<------------------->>>>>>>>\033[0m";
            cout << ("\033[32m\n                                                           --> Enter Your Choice: \033[0m");
            cin >> choice;

            switch (choice)
            {
            case 1:
                viewElections();
                cout << "                                                        Tap ENTER to proceed to the menu...\n";

                cin.get();
                break;
            case 2:
                viewCandidates();
                break;
            case 3:
                castVote();
                break;
            case 4:
                cout << "Logging out...\n";
                break;
            default:
                cout << "\033[31m\n     ?__Invalid Choice__?\n\033[0m";
            }
        } while (choice != 4);
    };

    void viewElections()
    {
        auto elections = MySQLHelper::Select("SELECT * FROM Elections");
        cout << "\033[32m\n_____________* Elections *\n\033[0m";
        for (const auto& e : elections)
        {
            cout << "\033[36m===========================================\033[0m\n";
            cout << "\033[33mElection ID:\033[0m      " << e.at("id") << "\n";
            cout << "\033[33mTitle:\033[0m            " << e.at("title") << "\n";
            cout << "\033[33mType:\033[0m             " << e.at("type") << "\n";
            cout << "\033[33mDate:\033[0m             " << e.at("date") << "\n";
            cout << "\033[33mStatus:\033[0m           " << e.at("status") << "\n";
            cout << "\033[33mStart Time:\033[0m       " << (e.at("start_time").empty() ? "N/A" : e.at("start_time")) << "\n";
            cout << "\033[33mEnd Time:\033[0m         " << (e.at("end_time").empty() ? "N/A" : e.at("end_time")) << "\n";
            cout << "\033[36m===========================================\033[0m\n\n";
        }

    };

    void viewCandidates() {
        string vote_type;
        cout << "Select Candidate Type(MPA/MNA) to View : ";
        cin >> vote_type;
        cin.ignore();

        // Fetch and display provinces
        auto provinces = MySQLHelper::Select("SELECT id, name FROM Provinces");
        cout << "\033[33m\n_____________* Provinces list *\n\033[0m";
        for (const auto& province : provinces) {
            cout << "\033[36mProvince ID: \033[0m" << province.at("id") << "  |  "
                << "\033[32mProvince Name: \033[0m" << province.at("name") << "\n";
        }
        cout << endl;



        int province_id;
        cout << "Enter Province ID: ";
        cin >> province_id;
        cin.ignore();

        int district_id = 0;
        if (vote_type == "MPA") {
            // Fetch and display districts for the selected province
            auto districts = MySQLHelper::Select("SELECT id, name FROM Districts WHERE province_id = " + to_string(province_id));
            cout << "\033[33m\n___________* Choose Your District *\n\033[0m";
            for (const auto& district : districts) {
                cout << district.at("id") << ". " << district.at("name") << "\n";
            }

            cout << "Enter District ID: ";
            cin >> district_id;
            cin.ignore();
        }

        // Fetch and display candidates based on selection
        string candidate_query = "SELECT id, name, party FROM Candidates WHERE type = '" + vote_type + "' AND province_id = " + to_string(province_id);
        if (vote_type == "MPA") {
            candidate_query += " AND district_id = " + to_string(district_id);
        }
        auto candidates = MySQLHelper::Select(candidate_query);

        if (candidates.empty()) {
            cout << "___________________-?-__________________\n";
            cout << "\033[33m\n ---!    No candidates found for the selected region    !---\n\033[0m";
            cout << "________________________________________\n";
            return;
        }

        cout << "\033[32m\n_____________* Candidates Records *\n\033[0m";
        cout << "\033[36m===========================================\033[0m\n";
        for (const auto& candidate : candidates) {
            cout << "\033[32mID: \033[32m" << candidate.at("id")
                << " \033[32m| Name: \033[36m" << candidate.at("name")
                << " \033[32m| Party: \033[35m" << candidate.at("party")<< "\033[0m\n";
        cout << "\033[36m===========================================\033[0m\n";

        }
    }

    void castVote() {
        // Check election status
        auto election_status = MySQLHelper::Select("SELECT status FROM Election ORDER BY id DESC LIMIT 1");

        if (election_status.empty() || election_status[0]["status"] != "Ongoing") {
            cout << "\033[31m\n---! Voting is not currently allowed. Election is either not started or already ended. !---\033[0m\n";
            return;
        }
        string vote_type;
        cout << "Select Vote Type (MPA/MNA): ";
        cin >> vote_type;
        cin.ignore();

        // Fetch and display provinces
        auto provinces = MySQLHelper::Select("SELECT id, name FROM Provinces");
        cout << "\033[32m\n_____________* Explore the Provinces Below: *\n\033[0m";
        for (const auto& province : provinces) {
            cout << province.at("id") << ". " << province.at("name") << "\n";
        }

        int province_id;
        cin >> province_id;
        cin.ignore();

        int district_id = 0;
        if (vote_type == "MPA") {
            // Fetch and display districts for the selected province
            auto districts = MySQLHelper::Select("SELECT id, name FROM Districts WHERE province_id = " + to_string(province_id));
            cout << "\033[32m\n___________* Choose Your District *\n\033[0m";
            for (const auto& district : districts) {
                cout << district.at("id") << ". " << district.at("name") << "\n";
            }

            cout << "Enter District ID: ";
            cin >> district_id;
            cin.ignore();
        }

        // Fetch and display candidates based on selection
        string candidate_query = "SELECT id, name, party FROM Candidates WHERE type = '" + vote_type + "' AND province_id = " + to_string(province_id);
        if (vote_type == "MPA") {
            candidate_query += " AND district_id = " + to_string(district_id);
        }
        auto candidates = MySQLHelper::Select(candidate_query);

        if (candidates.empty()) {
            cout << "___________________-?-__________________\n";
            cout << "\033[33m\n ---!    No candidates found for the selected region    !---\n\033[0m";
            cout << "________________________________________\n";
            return;
        }

        cout << "______* Available Candidates:\n";
        for (const auto& candidate : candidates) {
            cout << "\033[33mCandidate ID: \033[0m" << candidate.at("id") << " | "
                << "\033[32mName: \033[0m" << candidate.at("name") << " | "
                << "\033[36mParty: \033[0m" << candidate.at("party") << "\n";
        }
        int candidate_id;
        cout << "Enter Candidate ID to cast your vote: ";
        cin >> candidate_id;
        cin.ignore();

        // Check if the voter has already voted for this type
        string check_vote_query = "SELECT COUNT(*) AS vote_count FROM Votes WHERE voter_id = " + to_string(id) + " AND vote_type = '" + vote_type + "'";
        auto vote_check = MySQLHelper::Select(check_vote_query);
        if (!vote_check.empty() && stoi(vote_check[0]["vote_count"]) > 0) {
            cout << "___________________-_-__________________\n";
            cout << "Y\033[32m\n ---!    Already Casted Vote for \033[0m" << vote_type << "    !---\n";
            cout << "________________________________________\n";
            return;
        }

        // Insert the vote
        string insert_vote_query = "INSERT INTO Votes (voter_id, candidate_id, vote_type, province_id";
        if (vote_type == "MPA") {
            insert_vote_query += ", district_id";
        }
        insert_vote_query += ") VALUES (" + to_string(id) + ", " + to_string(candidate_id) + ", '" + vote_type + "', " + to_string(province_id);
        if (vote_type == "MPA") {
            insert_vote_query += ", " + to_string(district_id);
        }
        insert_vote_query += ")";

        if (MySQLHelper::ExecuteQuery(insert_vote_query)) {
            cout << "___________________-_-__________________\n";
            cout << "\033[32m\n ---!    Vote Casted Successfully    !---\n\033[0m";
            cout << "________________________________________\n";
            cout << "                                                        Tap ENTER to proceed to the menu...\n";
            cin.get();
        }
        else {
            cout << "___________________-?-__________________\n";
            cout << "\033[32m\n ---!    Error Occured While Casting    !---\n\033[0m";
            cout << "________________________________________\n";
            cout << "                                                        Tap ENTER to proceed to the menu...\n";
            cin.get();
        }
        
    }
};

// ======================= Main Function =======================
int main()
{
    int choice;
    do
    {
        showEVotingFrontPage();
        cout << "\033[32m\n\n--<<<<<<<< Specify Given Mode >>>>>>>>--\033[0m";
        cout << "\n         1. Admin SignUp\n         2. Admin Login\n         3. Voter SignUp\n         4. Voter Login\n         5. Exit";
        cout << "\033[32m\n  <<<<<<<<------------------->>>>>>>>\033[0m";
        cout << ("\033[32m\n                                                           --> Enter Your Choice: \033[0m");
        cin >> choice;
        cin.ignore(); // clear buffer

        if (choice == 1)
        {
            Admin admin;
            if (admin.adminSignup()) // Admin sign-up if no admin exists
                admin.menu(); 
        }
        else if (choice == 2)
        {
            Admin admin;
            if (admin.adminLogin()) 
                admin.menu(); 
        }
        else if (choice == 3)
        {
            Voter voter;
            voter.voterSignup(); // Voter sign-up
        }
        else if (choice == 4)
        {
            Voter voter;
            if (voter.voterLogin()) // Voter login
                voter.menu(); // After successful login, show the voter menu
        }
        
        else if (choice == 5)
        {
            cout << "\n                                               ====================================================================" << endl;
            cout << "\033[32m                                                 T H A N K   Y O U   F O R   U S I N G   T H E   S Y S T E M\033[0m" << endl;
            cout << "                                          ==========================================================================" << endl;
            cout << endl;
            cout << "\033[32m                                                                Your support means everything!\033[0m" << endl
                << endl;
            cout << "\033[32m                                                                           GOOD BYE!\033[0m" << endl;

        }
        else
        {
            cout << "\033[31m\n     ?__Invalid Choice__?\n\033[0m";
            cout << "                                                        Tap ENTER to proceed to the main menu...\n";
            cin.get();
            //cin.ignore();
        }
    } while (choice != 5);

    return 0;
}
