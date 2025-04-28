#include <iostream>
#include <iomanip>// setw()
#include <string>
#include <vector>
#include <map>
#include <mysql.h>

using namespace std;

// ================= MySQLHelper Class (Your given code remains same) =================
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
    bool login(const string& role)
    {
        // Check how many admins exist
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
            // Voter or other user login
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
                return false;
            }
        }
    }


};

// Admin Class
class Admin : public User {
public:
    void menu() override {
        int choice;
        do {
            cout << "\n=== Admin Menu ===\n"
                << "1. Add Candidate\n"
                << "2. View Results\n"
                << "3. Logout\n"
                << "Choice: "; cin >> choice;
            switch (choice) {
            case 1: addCandidate(); break;
            case 2: viewResults(); break;
            case 3: cout << "Logging out...\n"; break;
            default: cout << "Invalid choice\n";
            }
        } while (choice != 3);
    }
    const int CANDIDATE_LIMIT = 10;  // Maximum number of candidates allowed
    void addCandidate() {
        string name, party, type;
        cin.ignore(); // <<< IGNORE before first getline
        cout << "Enter Candidate Name: "; getline(cin, name);
        cout << "Enter Party: "; getline(cin, party);
        cout << "Type (MPA/MNA): "; getline(cin, type);

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
            for (auto& p : provs) cout << p.at("id") << ". " << p.at("name") << "\n";

            cout << "Select Province ID: "; cin >> province_id;
            cin.ignore(); // <<< Important after cin >>

            auto dists = MySQLHelper::Select("SELECT id,name FROM Districts WHERE province_id=" + to_string(province_id));
            cout << "Districts:\n";
            for (auto& d : dists) cout << d.at("id") << ". " << d.at("name") << "\n";

            cout << "Select District ID: "; cin >> district_id;
            cin.ignore(); // <<< Again if you will do getline next (safe to ignore)

            // Insert with district_id for MPA
            string q = "INSERT INTO Candidates (name,party,type,province_id,district_id) VALUES ('" +
                name + "','" + party + "','" + type + "'," + to_string(province_id) + "," + to_string(district_id) + ")";
            if (MySQLHelper::ExecuteQuery(q)) cout << "Candidate added.\n";
            else cout << "Error adding candidate.\n";
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
        if (!pm_res.empty()) {
            auto& r = pm_res[0];
            cout << "PM: " << r.at("candidate")
                << " (" << r.at("party") << "), Province: " << r.at("province")
                << " - Votes: " << r.at("votes") << "\n";
        }
    }
};

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

    void viewCandidates()
    {
        int election_id;
        cout << "Enter Election ID: ";
        cin >> election_id;
        auto candidates = MySQLHelper::Select("SELECT * FROM Candidates WHERE election_id = " + to_string(election_id));
        cout << "\n=== Candidates ===\n";
        for (const auto& c : candidates)
        {
            cout << "ID: " << c.at("id") << " | Name: " << c.at("name") << " | Party: " << c.at("party") << "\n";
        }
    };

    void castVote()
    {
        int election_id, candidate_id;
        cout << "Enter Election ID: ";
        cin >> election_id;
        cout << "Enter Candidate ID: ";
        cin >> candidate_id;

        string voterQuery = "SELECT id FROM Users WHERE username='" + username + "'";
        auto res = MySQLHelper::Select(voterQuery);
        if (!res.empty())
        {
            string voter_id = res[0].at("id");
            string voteQuery = "INSERT INTO Votes (voter_id, candidate_id, election_id) VALUES (" + voter_id + ", " + to_string(candidate_id) + ", " + to_string(election_id) + ")";
            if (MySQLHelper::ExecuteQuery(voteQuery))
            {
                cout << "Vote Casted Successfully!\n";
            }
            else
            {
                cout << "You may have already voted or error occurred.\n";
            }
        }
    };
};

// ======================= Main Function =======================
int main()
{
    int choice;
    do
    {
        cout << "\n1. Admin Login\n2. Voter Login\n3. Exit\nChoice: ";
        cin >> choice;
        cin.ignore(); // <- clear buffer after number input!

        if (choice == 1)
        {
            Admin admin;
            if (admin.login("admin"))
                admin.menu();
        }
        else if (choice == 2)
        {
            Voter voter;
            if (voter.login("voter"))
                voter.menu();
        }
        else if (choice == 3)
        {

            cout << "Exiting...\n";
        }
        else
        {

            cout << "Invalid choice.\n";
        }
    } while (choice != 3);

    return 0;
}
