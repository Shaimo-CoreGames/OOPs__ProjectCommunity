CREATE DATABASE  EVoting;
USE EVoting;
drop database es;
-- Provinces
CREATE TABLE  Provinces (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL
);

-- Districts
CREATE TABLE  Districts (
    id INT AUTO_INCREMENT PRIMARY KEY,
    province_id INT NOT NULL,
    name VARCHAR(100) NOT NULL,
    FOREIGN KEY (province_id) REFERENCES Provinces(id)
);

-- Users (Admins and Voters)
CREATE TABLE  Users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(100) NOT NULL,
    role ENUM('admin','voter') NOT NULL,
    cnic VARCHAR(13) UNIQUE,
    province_id INT DEFAULT NULL,
    district_id INT DEFAULT NULL,
    FOREIGN KEY (province_id) REFERENCES Provinces(id),
    FOREIGN KEY (district_id) REFERENCES Districts(id)
);
/* --  for droping table 
SET FOREIGN_KEY_CHECKS = 0;
DROP TABLE IF EXISTS Users;
SET FOREIGN_KEY_CHECKS = 1;
*/


-- Elections (optional, for extensibility)
CREATE TABLE Elections (
    id INT PRIMARY KEY,
    title VARCHAR(100) NOT NULL,
    type VARCHAR(50),  -- added type column
	date DATE,
    status VARCHAR(20), -- e.g., 'Not Started', 'Ongoing', 'Ended'
    start_time DATETIME DEFAULT NULL,
    end_time DATETIME DEFAULT NULL);


INSERT INTO Elections (title, type, date) 
VALUES ('National Assembly Election', 'General', '2025-07-01');
delete from Elections where id=4;
drop table Elections;

-- Candidates (MPA and MNA)
CREATE TABLE Candidates (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    party VARCHAR(100),
    type ENUM('MPA','MNA') NOT NULL,
    province_id INT DEFAULT NULL,
    district_id INT DEFAULT NULL,
    FOREIGN KEY (province_id) REFERENCES Provinces(id),
    FOREIGN KEY (district_id) REFERENCES Districts(id)
);

-- Votes
CREATE TABLE  Votes (
    id INT AUTO_INCREMENT PRIMARY KEY,
    vote_type ENUM('MPA','MNA') NOT NULL,
    voter_id INT NOT NULL,
    province_id INT DEFAULT NULL,
    district_id INT DEFAULT NULL,
    candidate_id INT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (voter_id) REFERENCES Users(id),
    FOREIGN KEY (province_id) REFERENCES Provinces(id),
    FOREIGN KEY (district_id) REFERENCES Districts(id),
    FOREIGN KEY (candidate_id) REFERENCES Candidates(id)
);
select * from Users;

-- Insert MNA votes (1 per voter for MNA in their province)
INSERT INTO Votes (vote_type, voter_id, province_id, candidate_id)
SELECT 
    'MNA',
    u.id AS voter_id,
    u.province_id,
    c.id AS candidate_id
FROM Users u
JOIN (
    SELECT id, province_id
    FROM Candidates
    WHERE type = 'MNA'
) c ON c.province_id = u.province_id
WHERE u.role = 'voter' AND u.province_id IS NOT NULL
ORDER BY RAND()
LIMIT 500;  -- Optional: adjust based on how many votes you want

-- Insert MPA votes (1 per voter for MPA in their district)
INSERT INTO Votes (vote_type, voter_id, province_id, district_id, candidate_id)
SELECT 
    'MPA',
    u.id AS voter_id,
    u.province_id,
    u.district_id,
    (
        SELECT c.id
        FROM Candidates c
        WHERE c.type = 'MPA' 
          AND c.province_id = u.province_id 
          AND c.district_id = u.district_id
        ORDER BY RAND()
        LIMIT 1
    ) AS candidate_id
FROM Users u
WHERE u.role = 'voter' 
  AND u.province_id IS NOT NULL 
  AND u.district_id IS NOT NULL
  AND EXISTS (
        SELECT 1 
        FROM Candidates c 
        WHERE c.type = 'MPA' 
          AND c.province_id = u.province_id 
          AND c.district_id = u.district_id
  );


-- 1. Provinces
INSERT INTO Provinces (name) VALUES
  ('Punjab'),
  ('Sindh'),
  ('Balochistan'),
  ('KPK'),
  ('GB');

INSERT INTO Districts (province_id, name) VALUES
  (1, 'Lahore'),
  (1, 'Faisalabad'),
  (1, 'Multan'),
  (1, 'Sialkot'),
  (1, 'Sheikhupura');

INSERT INTO Districts (province_id, name) VALUES
  (2, 'Karachi'), 
  (2, 'Hyderabad'),
  (2, 'Larkana'),
  (2, 'Mirpur Khas'),
  (2, 'Thatta');
  
INSERT INTO Districts (province_id, name) VALUES
  (3,'Quetta'),
  (3,'Gwadar'),
  (3,'Sibi'),
  (3,'Chaman'),
  (3,'Naseerabad');

INSERT INTO Districts (province_id, name) VALUES
  (4, 'Peshawar'),
  (4, 'Mardan'),
  (4, 'Swat'),
  (4, 'Kohat'),
  (4, 'Charsadda');


INSERT INTO Districts (province_id, name) VALUES
  (5, 'Gilgit'),
  (5, 'Skardu'),
  (5, 'Hunza'),
  (5, 'Shigar'),
  (5, 'Roundu');

-- Candidates for MNA (4 per province)
-- MNA Candidates: 4 per province (PTI, PMLN, PPP, TLP)
INSERT INTO Candidates (name, party, type, province_id) VALUES
-- Punjab
('Imran Khan', 'PTI', 'MNA', 1),
('Shehbaz Sharif', 'PMLN', 'MNA', 1),
('Bilawal Bhutto', 'PPP', 'MNA', 1),
('Khadim Rizvi Jr', 'TLP', 'MNA', 1),
-- Sindh
('Ali Haider Zaidi', 'PTI', 'MNA', 2),
('Ahsan Iqbal', 'PMLN', 'MNA', 2),
('Murad Ali Shah', 'PPP', 'MNA', 2),
('Rizwan Haider', 'TLP', 'MNA', 2),
-- Balochistan
('Qasim Suri', 'PTI', 'MNA', 3),
('Jam Kamal Khan', 'PMLN', 'MNA', 3),
('Sanaullah Zehri', 'PPP', 'MNA', 3),
('Farooq Bangalzai', 'TLP', 'MNA', 3),
-- KPK
('Pervez Khattak', 'PTI', 'MNA', 4),
('Ameer Muqam', 'PMLN', 'MNA', 4),
('Shahram Khan Tarakai', 'PPP', 'MNA', 4),
('Abdul Hameed', 'TLP', 'MNA', 4),
-- GB
('Javed Ali Manwa', 'PTI', 'MNA', 5),
('Muhammad Ismail', 'PMLN', 'MNA', 5),
('Raja Zakaria Maqpoon', 'PPP', 'MNA', 5),
('Hussain Ali', 'TLP', 'MNA', 5);

-- Candidates for MPA (1 per district, from major parties)---------------------------------------------------
-- Punjab MPA
INSERT INTO Candidates (name, party, type, province_id, district_id) VALUES
('Rana Sanaullah', 'PMLN', 'MPA', 1, 1),
('Fawad Chaudhry', 'PTI', 'MPA', 1, 2),
('Yasir Gillani', 'PPP', 'MPA', 1, 3),
('Khawaja Asif', 'PMLN', 'MPA', 1, 4),
('Abid Sher Ali', 'PMLN', 'MPA', 1, 5);

-- Sindh MPA
INSERT INTO Candidates (name, party, type, province_id, district_id) VALUES
('Saeed Ghani', 'PPP', 'MPA', 2, 6),
('Imtiaz Shaikh', 'PPP', 'MPA', 2, 7),
('Nisar Khuhro', 'PPP', 'MPA', 2, 8),
('Ali Nawaz Shah', 'PMLN', 'MPA', 2, 9),
('Haleem Adil Sheikh', 'PTI', 'MPA', 2, 10);

-- Balochistan MPA
INSERT INTO Candidates (name, party, type, province_id, district_id) VALUES
('Mir Sarfaraz Bugti', 'PTI', 'MPA', 3, 11),
('Aslam Bhootani', 'PMLN', 'MPA', 3, 12),
('Zamrak Khan', 'PPP', 'MPA', 3, 13),
('Abdul Rehman Khetran', 'PPP', 'MPA', 3, 14),
('Mir Zahoor Buledi', 'PTI', 'MPA', 3, 15);

-- KPK MPA
INSERT INTO Candidates (name, party, type, province_id, district_id) VALUES
('Taimur Jhagra', 'PTI', 'MPA', 4, 16),
('Khwaja Muhammad Khan', 'PMLN', 'MPA', 4, 17),
('Shaukat Yousafzai', 'PTI', 'MPA', 4, 18),
('Zahir Shah', 'PPP', 'MPA', 4, 19),
('Iqbal Afridi', 'PTI', 'MPA', 4, 20);

-- GB MPA
INSERT INTO Candidates (name, party, type, province_id, district_id) VALUES
('Wazir Muhammad Saleem', 'PTI', 'MPA', 5, 21),
('Mushtaq Hussain', 'PMLN', 'MPA', 5, 22),
('Sultan Ali', 'PPP', 'MPA', 5, 23),
('Haji Rehmat Khaliq', 'PTI', 'MPA', 5, 24),
('Raja Jahanzaib', 'PMLN', 'MPA', 5, 25);


  select * from Users where  role = 'admin';
DELETE FROM Users WHERE username = 'admin';


  ALTER TABLE Candidates MODIFY district_id INT DEFAULT NULL;
INSERT INTO Candidates (name, party, type, province_id, district_id)
VALUES ('Asad Umar', 'PTI', 'MNA', 1, NULL);
SELECT * FROM Users WHERE role = 'voter';
select * from Candidates;





























INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 1, 4, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 1, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 2, 1, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 2, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 3, 5, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 3, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 4, 1, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 4, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 5, 1, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 5, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 6, 1, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 6, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 7, 2, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 7, 127, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 8, 5, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 8, 129, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 9, 4, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 9, 129, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 10, 1, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 10, 127, 1, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 11, 9, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 11, 129, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 12, 6, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 12, 127, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 13, 9, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 13, 126, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 14, 10, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 14, 127, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 15, 10, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 15, 126, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 16, 10, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 16, 127, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 17, 6, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 17, 127, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 18, 6, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 18, 129, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 19, 6, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 19, 127, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 20, 9, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, province_id) VALUES ('MNA', 20, 129, 2, 1);
INSERT INTO Votes (vote_type, voter_id, candidate_id, district_id, province_id) VALUES ('MPA', 21, 12, 3, 1);


-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter1', 'pass1', 'voter', '471858464971', 1, 1);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter2', 'pass2', 'voter', '765385665431', 1, 1);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter3', 'pass3', 'voter', '905965738503', 1, 1);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter4', 'pass4', 'voter', '279887559574', 1, 1);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter5', 'pass5', 'voter', '223081887884', 1, 1);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter6', 'pass6', 'voter', '691051604627', 1, 1);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter7', 'pass7', 'voter', '509767088768', 1, 1);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter8', 'pass8', 'voter', '347051154497', 1, 1);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter9', 'pass9', 'voter', '530155680817', 1, 1);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ( 'voter10', 'pass10', 'voter', '193026560237', 1, 1);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter11', 'pass11', 'voter', '188543452245', 1, 2);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter12', 'pass12', 'voter', '130589691427', 1, 2);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter13', 'pass13', 'voter', '862542913321', 1, 2);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter14', 'pass14', 'voter', '340121279839', 1, 2);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter15', 'pass15', 'voter', '369275458274', 1, 2);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter16', 'pass16', 'voter', '899205573134', 1, 2);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter17', 'pass17', 'voter', '811031332890', 1, 2);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter18', 'pass18', 'voter', '333685195639', 1, 2);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter19', 'pass19', 'voter', '287882021392', 1, 2);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter20', 'pass20', 'voter', '361612149271', 1, 2);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter21', 'pass21', 'voter', '204794860382', 1, 3);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter22', 'pass22', 'voter', '326805859553', 1, 3);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter23', 'pass23', 'voter', '961236195613', 1, 3);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter24', 'pass24', 'voter', '851247177972', 1, 3);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter25', 'pass25', 'voter', '987621405768', 1, 3);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter26', 'pass26', 'voter', '993828500181', 1, 3);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter27', 'pass27', 'voter', '279225834476', 1, 3);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter28', 'pass28', 'voter', '704133665482', 1, 3);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter29', 'pass29', 'voter', '841367647495', 1, 3);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter30', 'pass30', 'voter', '251231894728', 1, 3);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter31', 'pass31', 'voter', '971427545734', 1, 4);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter32', 'pass32', 'voter', '898997480015', 1, 4);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter33', 'pass33', 'voter', '860766893307', 1, 4);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter34', 'pass34', 'voter', '242199352275', 1, 4);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter35', 'pass35', 'voter', '812029179994', 1, 4);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter36', 'pass36', 'voter', '314986521304', 1, 4);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter37', 'pass37', 'voter', '761118096427', 1, 4);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter38', 'pass38', 'voter', '930754255862', 1, 4);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter39', 'pass39', 'voter', '569505176049', 1, 4);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter40', 'pass40', 'voter', '997918280738', 1, 4);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter41', 'pass41', 'voter', '704033740684', 1, 5);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter42', 'pass42', 'voter', '770718269846', 1, 5);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter43', 'pass43', 'voter', '393939755909', 1, 5);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter44', 'pass44', 'voter', '644975940963', 1, 5);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter45', 'pass45', 'voter', '240027805186', 1, 5);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter46', 'pass46', 'voter', '966734765931', 1, 5);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter47', 'pass47', 'voter', '336348165862', 1, 5);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter48', 'pass48', 'voter', '240594245019', 1, 5);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter49', 'pass49', 'voter', '245891478265', 1, 5);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter50', 'pass50', 'voter', '663224135556', 1, 5);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter51', 'pass51', 'voter', '406361341503', 2, 6);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter52', 'pass52', 'voter', '463281519938', 2, 6);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter53', 'pass53', 'voter', '581137230525', 2, 6);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter54', 'pass54', 'voter', '277795351884', 2, 6);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter55', 'pass55', 'voter', '774248044634', 2, 6);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter56', 'pass56', 'voter', '457864560032', 2, 6);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter57', 'pass57', 'voter', '853393975455', 2, 6);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter58', 'pass58', 'voter', '391188309431', 2, 6);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter59', 'pass59', 'voter', '704159846261', 2, 6);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter60', 'pass60', 'voter', '690858109133', 2, 6);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter61', 'pass61', 'voter', '898749767104', 2, 7);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter62', 'pass62', 'voter', '823171676804', 2, 7);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter63', 'pass63', 'voter', '604842387860', 2, 7);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter64', 'pass64', 'voter', '799655674037', 2, 7);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter65', 'pass65', 'voter', '506418912616', 2, 7);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter66', 'pass66', 'voter', '987821406393', 2, 7);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter67', 'pass67', 'voter', '401089809116', 2, 7);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter68', 'pass68', 'voter', '733186270857', 2, 7);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter69', 'pass69', 'voter', '853127875935', 2, 7);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter70', 'pass70', 'voter', '276551329929', 2, 7);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter71', 'pass71', 'voter', '976155688348', 2, 8);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter72', 'pass72', 'voter', '651345174367', 2, 8);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter73', 'pass73', 'voter', '835753140455', 2, 8);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter74', 'pass74', 'voter', '861273038340', 2, 8);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter75', 'pass75', 'voter', '273455829390', 2, 8);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter76', 'pass76', 'voter', '782798915285', 2, 8);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter77', 'pass77', 'voter', '905021836324', 2, 8);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter78', 'pass78', 'voter', '979267681096', 2, 8);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter79', 'pass79', 'voter', '768438303905', 2, 8);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter80', 'pass80', 'voter', '736282824817', 2, 8);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter81', 'pass81', 'voter', '256908734090', 2, 9);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter82', 'pass82', 'voter', '581019202736', 2, 9);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter83', 'pass83', 'voter', '117786877765', 2, 9);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter84', 'pass84', 'voter', '446911416371', 2, 9);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter85', 'pass85', 'voter', '893731246390', 2, 9);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter86', 'pass86', 'voter', '304471218017', 2, 9);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter87', 'pass87', 'voter', '563409960336', 2, 9);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter88', 'pass88', 'voter', '458959333017', 2, 9);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter89', 'pass89', 'voter', '881498610311', 2, 9);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter90', 'pass90', 'voter', '798767042188', 2, 9);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter91', 'pass91', 'voter', '848185943593', 2, 10);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter92', 'pass92', 'voter', '362797010957', 2, 10);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter93', 'pass93', 'voter', '931629049572', 2, 10);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter94', 'pass94', 'voter', '152089656604', 2, 10);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter95', 'pass95', 'voter', '906918615874', 2, 10);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter96', 'pass96', 'voter', '539625723812', 2, 10);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter97', 'pass97', 'voter', '797944863043', 2, 10);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter98', 'pass98', 'voter', '493693703663', 2, 10);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter99', 'pass99', 'voter', '736388808387', 2, 10);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ( 'voter100', 'pass100', 'voter', '693505938144', 2, 10);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter101', 'pass101', 'voter', '634892939643', 3, 11);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter102', 'pass102', 'voter', '990911749993', 3, 11);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter103', 'pass103', 'voter', '830945804273', 3, 11);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter104', 'pass104', 'voter', '108529122944', 3, 11);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter105', 'pass105', 'voter', '494044581433', 3, 11);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter106', 'pass106', 'voter', '352075251525', 3, 11);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter107', 'pass107', 'voter', '491034324042', 3, 11);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter108', 'pass108', 'voter', '470265590667', 3, 11);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter109', 'pass109', 'voter', '612507951513', 3, 11);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter110', 'pass110', 'voter', '490884128487', 3, 11);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter111', 'pass111', 'voter', '447486200658', 3, 12);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter112', 'pass112', 'voter', '782251220173', 3, 12);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter113', 'pass113', 'voter', '682808169499', 3, 12);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter114', 'pass114', 'voter', '216244268918', 3, 12);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter115', 'pass115', 'voter', '216454734153', 3, 12);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter116', 'pass116', 'voter', '301643450142', 3, 12);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter117', 'pass117', 'voter', '405406845606', 3, 12);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter118', 'pass118', 'voter', '629998874716', 3, 12);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter119', 'pass119', 'voter', '873163098862', 3, 12);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter120', 'pass120', 'voter', '872512271474', 3, 12);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter121', 'pass121', 'voter', '993707331390', 3, 13);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter122', 'pass122', 'voter', '928336102414', 3, 13);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter123', 'pass123', 'voter', '138539421950', 3, 13);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter124', 'pass124', 'voter', '818682513712', 3, 13);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter125', 'pass125', 'voter', '333578687768', 3, 13);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter126', 'pass126', 'voter', '191041912822', 3, 13);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter127', 'pass127', 'voter', '331202519924', 3, 13);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter128', 'pass128', 'voter', '594274328507', 3, 13);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter129', 'pass129', 'voter', '520019284240', 3, 13);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter130', 'pass130', 'voter', '960763973473', 3, 13);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter131', 'pass131', 'voter', '219709109671', 3, 14);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter132', 'pass132', 'voter', '270835706818', 3, 14);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter133', 'pass133', 'voter', '631258904599', 3, 14);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter134', 'pass134', 'voter', '291372019378', 3, 14);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter135', 'pass135', 'voter', '687877034938', 3, 14);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter136', 'pass136', 'voter', '944748259140', 3, 14);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter137', 'pass137', 'voter', '664692329009', 3, 14);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter138', 'pass138', 'voter', '470332599660', 3, 14);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter139', 'pass139', 'voter', '418364267620', 3, 14);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter140', 'pass140', 'voter', '156925161343', 3, 14);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter141', 'pass141', 'voter', '661242823192', 3, 15);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter142', 'pass142', 'voter', '946807605103', 3, 15);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter143', 'pass143', 'voter', '563536690871', 3, 15);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter144', 'pass144', 'voter', '209825467383', 3, 15);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter145', 'pass145', 'voter', '420156456371', 3, 15);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter146', 'pass146', 'voter', '320434676280', 3, 15);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter147', 'pass147', 'voter', '406224944867', 3, 15);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter148', 'pass148', 'voter', '904378071541', 3, 15);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter149', 'pass149', 'voter', '986824462206', 3, 15);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter150', 'pass150', 'voter', '847329461423', 3, 15);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter151', 'pass151', 'voter', '175046156523', 4, 16);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter152', 'pass152', 'voter', '704797214574', 4, 16);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter153', 'pass153', 'voter', '874108930935', 4, 16);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter154', 'pass154', 'voter', '659182583394', 4, 16);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter155', 'pass155', 'voter', '713891139405', 4, 16);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter156', 'pass156', 'voter', '879264105058', 4, 16);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter157', 'pass157', 'voter', '436401027481', 4, 16);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter158', 'pass158', 'voter', '898656283106', 4, 16);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter159', 'pass159', 'voter', '239164516752', 4, 16);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter160', 'pass160', 'voter', '806399661447', 4, 16);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter161', 'pass161', 'voter', '516096675275', 4, 17);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter162', 'pass162', 'voter', '254913206350', 4, 17);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter163', 'pass163', 'voter', '681441672645', 4, 17);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter164', 'pass164', 'voter', '865008261348', 4, 17);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter165', 'pass165', 'voter', '133374005235', 4, 17);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter166', 'pass166', 'voter', '177256538954', 4, 17);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter167', 'pass167', 'voter', '137322876003', 4, 17);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter168', 'pass168', 'voter', '185125949427', 4, 17);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter169', 'pass169', 'voter', '423364927075', 4, 17);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter170', 'pass170', 'voter', '879767341304', 4, 17);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter171', 'pass171', 'voter', '423286274481', 4, 18);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter172', 'pass172', 'voter', '375602378446', 4, 18);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter173', 'pass173', 'voter', '977136563504', 4, 18);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter174', 'pass174', 'voter', '972097573780', 4, 18);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter175', 'pass175', 'voter', '186377555534', 4, 18);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter176', 'pass176', 'voter', '437173198767', 4, 18);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter177', 'pass177', 'voter', '445626119854', 4, 18);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter178', 'pass178', 'voter', '887679094022', 4, 18);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter179', 'pass179', 'voter', '652062963706', 4, 18);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter180', 'pass180', 'voter', '361574309400', 4, 18);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter181', 'pass181', 'voter', '136337225053', 4, 19);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter182', 'pass182', 'voter', '388769990405', 4, 19);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter183', 'pass183', 'voter', '691842465902', 4, 19);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter184', 'pass184', 'voter', '173346582355', 4, 19);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter185', 'pass185', 'voter', '537968395501', 4, 19);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter186', 'pass186', 'voter', '581283519648', 4, 19);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter187', 'pass187', 'voter', '797991340174', 4, 19);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter188', 'pass188', 'voter', '846481333073', 4, 19);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter189', 'pass189', 'voter', '338443833537', 4, 19);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter190', 'pass190', 'voter', '830815581625', 4, 19);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter191', 'pass191', 'voter', '956089881703', 4, 20);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter192', 'pass192', 'voter', '418803096705', 4, 20);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter193', 'pass193', 'voter', '378559368910', 4, 20);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter194', 'pass194', 'voter', '864545360177', 4, 20);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter195', 'pass195', 'voter', '191015121024', 4, 20);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter196', 'pass196', 'voter', '373363998707', 4, 20);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter197', 'pass197', 'voter', '155449736287', 4, 20);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter198', 'pass198', 'voter', '821043697586', 4, 20);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter199', 'pass199', 'voter', '100419751689', 4, 20);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter200', 'pass200', 'voter', '823417287463', 4, 20);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter201', 'pass201', 'voter', '612349019601', 5, 21);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter202', 'pass202', 'voter', '955375736213', 5, 21);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter203', 'pass203', 'voter', '414108914042', 5, 21);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter204', 'pass204', 'voter', '903494685055', 5, 21);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter205', 'pass205', 'voter', '555773342626', 5, 21);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter206', 'pass206', 'voter', '325988657759', 5, 21);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter207', 'pass207', 'voter', '158124783880', 5, 21);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter208', 'pass208', 'voter', '772234633014', 5, 21);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter209', 'pass209', 'voter', '340276211115', 5, 21);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter210', 'pass210', 'voter', '684106587032', 5, 21);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter211', 'pass211', 'voter', '626193736065', 5, 22);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter212', 'pass212', 'voter', '893762383601', 5, 22);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter213', 'pass213', 'voter', '882297656446', 5, 22);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter214', 'pass214', 'voter', '671144075329', 5, 22);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter215', 'pass215', 'voter', '192865250727', 5, 22);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter216', 'pass216', 'voter', '727476994800', 5, 22);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter217', 'pass217', 'voter', '264187200210', 5, 22);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter218', 'pass218', 'voter', '496754381506', 5, 22);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter219', 'pass219', 'voter', '518806851394', 5, 22);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter220', 'pass220', 'voter', '530737015728', 5, 22);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter221', 'pass221', 'voter', '835375031079', 5, 23);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter222', 'pass222', 'voter', '832639970340', 5, 23);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter223', 'pass223', 'voter', '106878199440', 5, 23);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter224', 'pass224', 'voter', '431426871019', 5, 23);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter225', 'pass225', 'voter', '885596654289', 5, 23);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter226', 'pass226', 'voter', '289134270870', 5, 23);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter227', 'pass227', 'voter', '197288230379', 5, 23);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter228', 'pass228', 'voter', '942961959720', 5, 23);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter229', 'pass229', 'voter', '162318214515', 5, 23);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter230', 'pass230', 'voter', '962511080131', 5, 23);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter231', 'pass231', 'voter', '259932903880', 5, 24);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter232', 'pass232', 'voter', '194047666791', 5, 24);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter233', 'pass233', 'voter', '772723639785', 5, 24);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter234', 'pass234', 'voter', '786293568444', 5, 24);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter235', 'pass235', 'voter', '183728694665', 5, 24);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter236', 'pass236', 'voter', '602345964000', 5, 24);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter237', 'pass237', 'voter', '300805297569', 5, 24);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter238', 'pass238', 'voter', '714012293182', 5, 24);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter239', 'pass239', 'voter', '404978891420', 5, 24);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter240', 'pass240', 'voter', '720609944558', 5, 24);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter241', 'pass241', 'voter', '357841065224', 5, 25);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter242', 'pass242', 'voter', '234701502464', 5, 25);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter243', 'pass243', 'voter', '360384111355', 5, 25);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter244', 'pass244', 'voter', '856348971818', 5, 25);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter245', 'pass245', 'voter', '347022651864', 5, 25);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter246', 'pass246', 'voter', '113933920598', 5, 25);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter247', 'pass247', 'voter', '583413974183', 5, 25);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter248', 'pass248', 'voter', '901092279049', 5, 25);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter249', 'pass249', 'voter', '112465952823', 5, 25);
-- INSERT INTO Users (username, password, role, cnic, province_id, district_id) VALUES ('voter250', 'pass250', 'voter', '407324964257', 5, 25);
