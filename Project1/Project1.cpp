#include <iostream>
#include <iomanip>// setw()
#include <algorithm> // all_of
#include <string>
#include <vector>
#include <map>
#include <mysql.h>

using namespace std;

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
        MYSQL_RES* result = mysql_store_result(conn);
        if (!result)
        {
            mysql_close(conn);
            return results;
        }
        int num_fields = mysql_num_fields(result);
        MYSQL_FIELD* fields = mysql_fetch_fields(result);
        MYSQL_ROW row;
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
};

// ======================= Base Classes =======================

// Base User Class
class User {
protected:
    int id;
    string username;
public:
    virtual void menu() = 0;
    bool voterSignup() {
        string username, password, cnic;

        cout << "Signup as Voter\n";
        cout << "Enter CNIC (13 digits): ";
        getline(cin, cnic);

        // CNIC validation
        if (cnic.length() != 13 || !all_of(cnic.begin(), cnic.end(), ::isdigit)) {
            cout << "Invalid CNIC. It must be exactly 13 digits.\n";
            return false;
        }

        // Check if CNIC already exists
        string checkCNICQuery = "SELECT id FROM Users WHERE cnic = '" + cnic + "'";
        auto existing = MySQLHelper::Select(checkCNICQuery);
        if (!existing.empty()) {
            cout << "A user with this CNIC already exists!\n";
            return false;
        }

        cout << "Enter Username: ";
        getline(cin, username);
        cout << "Enter Password: ";
        getline(cin, password);

        string insertQuery = "INSERT INTO Users (username, password, role, cnic) VALUES ('" + username + "', '" + password + "', 'voter', '" + cnic + "')";
        if (MySQLHelper::ExecuteQuery(insertQuery)) {
            cout << "Voter signed up successfully!\n";
            return true;
        }
        else {
            cout << "Error in signup!\n";
            return false;
        }
    }

    bool login(const string& role)
    {
        // kitna admins exist kar rha hain
        string checkQuery = "SELECT COUNT(*) AS count FROM Users WHERE role = 'admin'";
        auto countResult = MySQLHelper::Select(checkQuery);

        int adminCount = 0;
        if (!countResult.empty())
        {
            adminCount = stoi(countResult[0]["count"]);
        }

        if (role == "admin" && adminCount == 0)
        {
            // No admin exists, allow to create
            cout << "No Admin Found. Create New Admin:\n";
            cout << "Username: ";
            getline(cin, username);

            string pwd;
            cout << "Password: ";
            getline(cin, pwd);

            string insertQuery = "INSERT INTO Users (username, password, role) VALUES ('" + username + "', '" + pwd + "', 'admin')";
            if (MySQLHelper::ExecuteQuery(insertQuery))
            {
                cout << "Admin account created successfully!\n";
                return true;
            }
            else
            {
                cout << "Error creating admin account.\n";
                return false;
            }
        }
        else if (role == "admin" && adminCount > 0)
        {
            // Admin already exists, login normally
            cout << "Username: ";
            getline(cin, username);

            string pwd;
            cout << "Password: ";
            getline(cin, pwd);

            string q = "SELECT id FROM Users WHERE username='" + username + "' AND password='" + pwd + "' AND role='" + role + "'";
            auto res = MySQLHelper::Select(q);

            if (!res.empty())
            {
                id = stoi(res[0]["id"]);
                cout << "Login successful as " << role << "\n";
                return true;
            }
            else
            {
                cout << "Login failed!\n";
                cout << "Admin already exists. To add a new admin, please use SQL console manually!\n";
                return false;
            }
        }
        else
        {
            // Voter login
            cout << "Username: ";
            getline(cin, username);

            string pwd, cnic;
            cout << "Password: ";
            getline(cin, pwd);
            cout << "CNIC (13 digits): ";
            getline(cin, cnic);

            // Basic CNIC validation
            if (cnic.length() != 13 || !all_of(cnic.begin(), cnic.end(), ::isdigit)) {
                cout << "Invalid CNIC. It must be exactly 13 digits.\n";
                return false;
            }

            string q = "SELECT id FROM Users WHERE username='" + username + "' AND password='" + pwd + "' AND role='" + role + "' AND cnic='" + cnic + "'";
            auto res = MySQLHelper::Select(q);

            if (!res.empty())
            {
                id = stoi(res[0]["id"]);
                cout << "Login successful as " << role << "\n";
                return true;
            }
            else
            {
                cout << "Login failed! Check username, password, or CNIC.\n";
                return false;
            }
        }

    }


}; 

// -----------------------------------------------__Admin Class__-----------------------------------------------
class Admin : public User {
public:
    void menu() override {
        int choice;
        do {
            cout << "\n=== Admin Menu ===\n"
                << "1. Add Candidate\n"
                << "2. View Results\n"
                << "3. Add District\n"
                << "4. Add Province\n"
                << "5. Logout\n"
                << "Choice: ";
            cin >> choice;
            switch (choice) {
            case 1: addCandidate(); break;
            case 2: viewResults(); break;
            case 3: addDistrict(); break;
            case 4: addProvince(); break;
            case 5: cout << "Logging out...\n"; break;
            default: cout << "Invalid choice\n";
            }
        } while (choice != 5);
    }

    const int CANDIDATE_LIMIT = 5;  // Maximum number of candidates that we  allowed
    void addCandidate() {
        string name, party, type;
        cin.ignore(); 
        cout << "Enter Candidate Name: "; 
        getline(cin, name);
        cout << "Enter Party: "; 
        getline(cin, party);
        cout << "Type (MPA/MNA): "; 
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
            cout << "Your candidates have reached the limit!\n";
            return;  // Exit the function without adding a new candidate
        }

        int province_id = 0, district_id = 0;

        if (type == "MPA") {
            auto provs = MySQLHelper::Select("SELECT id,name FROM Provinces");
            cout << "Provinces:\n";
            for (auto& p : provs) 
                cout << p.at("id") << ". " << p.at("name") << "\n";

            cout << "Select Province ID: "; cin >> province_id;
            cin.ignore(); 

            auto dists = MySQLHelper::Select("SELECT id,name FROM Districts WHERE province_id=" + to_string(province_id));
            cout << "Districts:\n";
            for (auto& d : dists) 
                cout << d.at("id") << ". " << d.at("name") << "\n";

            cout << "Select District ID: "; cin >> district_id;
            cin.ignore(); 

            // Insert with district_id for MPA
            string q = "INSERT INTO Candidates (name,party,type,province_id,district_id) VALUES ('" +
                name + "','" + party + "','" + type + "'," + to_string(province_id) + "," + to_string(district_id) + ")";
            if (MySQLHelper::ExecuteQuery(q)) 
                cout << "Candidate added.\n";
            else 
                cout << "Error adding candidate.\n";
        }
        else if (type == "MNA") {
            auto provs = MySQLHelper::Select("SELECT id,name FROM Provinces");
            cout << "Provinces:\n";
            for (auto& p : provs) cout << p.at("id") << ". " << p.at("name") << "\n";

            cout << "Select Province ID: "; cin >> province_id;
            cin.ignore(); // <<< Ignore leftover newline

            // Insert without district_id for MNA (set district_id NULL)
            string q = "INSERT INTO Candidates (name,party,type,province_id,district_id) VALUES ('" +
                name + "','" + party + "','" + type + "'," + to_string(province_id) + ", NULL)";
            if (MySQLHelper::ExecuteQuery(q)) cout << "Candidate added.\n";
            else cout << "Error adding candidate.\n";
        }
        else {
            cout << "Unknown type\n";
        }
    }

    void viewResults() {
        cout << "\n--- MPA Winners by District ---\n";
        string mpa_q =
            "SELECT d.name AS region, c.name AS candidate, c.party, mv.votes FROM "
            "(SELECT district_id, candidate_id, COUNT(*) AS votes FROM Votes WHERE vote_type='MPA' GROUP BY district_id,candidate_id) mv "
            "JOIN (SELECT district_id AS did, MAX(votes) AS max_votes FROM "
            "(SELECT district_id, candidate_id, COUNT(*) AS votes FROM Votes WHERE vote_type='MPA' GROUP BY district_id,candidate_id) t GROUP BY district_id) mx "
            "ON mv.district_id=mx.did AND mv.votes=mx.max_votes "
            "JOIN Candidates c ON mv.candidate_id=c.id "
            "JOIN Districts d ON mv.district_id=d.id;";
        auto mpa_res = MySQLHelper::Select(mpa_q);
        for (auto& r : mpa_res)
            cout << "District: " << setw(15) << r.at("region")
            << " | " << r.at("candidate")
            << " (" << r.at("party") << ") - Votes: " << r.at("votes") << "\n";

        cout << "\n--- MNA Winners by Province ---\n";
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
            cout << "Province: " << setw(15) << r.at("region")
            << " | " << r.at("candidate")
            << " (" << r.at("party") << ") - Votes: " << r.at("votes") << "\n";

        //------------------------------------------*****-- Final Selection --*****-----------------------------------------
        cout << "\n--- Chief Minister Selection ---\n";
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
            cout << "Province: " << setw(15) << r.at("province")
            << " | CM: " << r.at("candidate")
            << " (" << r.at("party") << ") - Votes: " << r.at("votes") << "\n";

        cout << "\n--- Prime Minister Selection ---\n";
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
            cout << "PM: " << r.at("candidate")
                << " (" << r.at("party") << "), Province: " << r.at("province")
                << " - Votes: " << r.at("votes") << "\n";
        }
    }
  
    void addDistrict() {
        string name;
        int province_id;

        auto provs = MySQLHelper::Select("SELECT id, name FROM Provinces");
        if (provs.empty()) {
            cout << "No provinces available. Please add a province first.\n";
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
                cout << "This province already has 5 districts. Limit reached.\n";
                break;
            }

            cout << "Enter District Name: ";
            getline(cin, name);

            string query = "INSERT INTO Districts (name, province_id) VALUES ('" + name + "', " + to_string(province_id) + ")";
            if (MySQLHelper::ExecuteQuery(query))
                cout << "District added successfully.\n";
            else
                cout << "Error adding district.\n";

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
                cout << "Maximum of 5 provinces already added.\n";
                break;
            }

            cout << "Enter Province Name: ";
            cin.ignore();
            getline(cin, name);

            string query = "INSERT INTO Provinces (name) VALUES ('" + name + "')";
            if (MySQLHelper::ExecuteQuery(query))
                cout << "Province added successfully.\n";
            else
                cout << "Error adding province.\n";

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
            cout << "\n=== Voter Menu ===";
            cout << "\n1. View Elections";
            cout << "\n2. View Candidates";
            cout << "\n3. Cast Vote";
            cout << "\n4. Logout";
            cout << "\nEnter Choice: ";
            cin >> choice;

            switch (choice)
            {
            case 1:
                viewElections();
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
                cout << "Invalid choice!\n";
            }
        } while (choice != 4);
    };

    void viewElections()
    {
        auto elections = MySQLHelper::Select("SELECT * FROM Elections");
        cout << "\n=== Elections ===\n";
        for (const auto& e : elections)
        {
            cout << "ID: " << e.at("id") << " | Title: " << e.at("title") << " | Type: " << e.at("type") << "\n";
        }
    };

    void viewCandidates() {
        string vote_type;
        cout << "Select Candidate Type to View (MPA/MNA): ";
        cin >> vote_type;
        cin.ignore();

        // Fetch and display provinces
        auto provinces = MySQLHelper::Select("SELECT id, name FROM Provinces");
        cout << "Available Provinces:\n";
        for (const auto& province : provinces) {
            cout << province.at("id") << ". " << province.at("name") << "\n";
        }

        int province_id;
        cout << "Enter Province ID: ";
        cin >> province_id;
        cin.ignore();

        int district_id = 0;
        if (vote_type == "MPA") {
            // Fetch and display districts for the selected province
            auto districts = MySQLHelper::Select("SELECT id, name FROM Districts WHERE province_id = " + to_string(province_id));
            cout << "Available Districts:\n";
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
            cout << "No candidates found for the selected region.\n";
            return;
        }

        cout << "Candidates:\n";
        for (const auto& candidate : candidates) {
            cout << "ID: " << candidate.at("id") << " | Name: " << candidate.at("name") << " | Party: " << candidate.at("party") << "\n";
        }
    }

    void castVote() {
        string vote_type;
        cout << "Select Vote Type (MPA/MNA): ";
        cin >> vote_type;
        cin.ignore();

        // Fetch and display provinces
        auto provinces = MySQLHelper::Select("SELECT id, name FROM Provinces");
        cout << "Provinces:\n";
        for (const auto& province : provinces) {
            cout << province.at("id") << ". " << province.at("name") << "\n";
        }

        int province_id;
        cout << "Enter Province ID: ";
        cin >> province_id;
        cin.ignore();

        int district_id = 0;
        if (vote_type == "MPA") {
            // Fetch and display districts for the selected province
            auto districts = MySQLHelper::Select("SELECT id, name FROM Districts WHERE province_id = " + to_string(province_id));
            cout << " Districts:\n";
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
            cout << "No candidates found for the selected region.\n";
            return;
        }

        cout << "Available Candidates:\n";
        for (const auto& candidate : candidates) {
            cout << candidate.at("id") << ". " << candidate.at("name") << " (" << candidate.at("party") << ")\n";
        }

        int candidate_id;
        cout << "Enter Candidate ID to cast your vote: ";
        cin >> candidate_id;
        cin.ignore();

        // Check if the voter has already voted for this type
        string check_vote_query = "SELECT COUNT(*) AS vote_count FROM Votes WHERE voter_id = " + to_string(id) + " AND vote_type = '" + vote_type + "'";
        auto vote_check = MySQLHelper::Select(check_vote_query);
        if (!vote_check.empty() && stoi(vote_check[0]["vote_count"]) > 0) {
            cout << "You have already cast your vote for " << vote_type << ".\n";
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
            cout << "Your vote has been cast successfully.\n";
        }
        else {
            cout << "An error occurred while casting your vote.\n";
        }
        
    }

};

// ======================= Main Function =======================
int main()
{
    int choice;
    do
    {
        cout << "\n1. Admin Login\n2. Voter Signup\n3. Voter Login\n4. Exit\nChoice: ";
        cin >> choice;
        cin.ignore(); // clear buffer

        if (choice == 1)
        {
            Admin admin;
            if (admin.login("admin"))
                admin.menu();
        }
        else if (choice == 2)
        {
            Voter voter;
            voter.voterSignup();
        }
        else if (choice == 3)
        {
            Voter voter;
            if (voter.login("voter"))
                voter.menu();
        }
        
        else if (choice == 4)
        {
            cout << "Exiting...\n";
        }
        else
        {
            cout << "Invalid choice.\n";
        }
    } while (choice != 4);

    return 0;
}
