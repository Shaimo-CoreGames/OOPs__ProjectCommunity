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
    province_id INT DEFAULT NULL,
    district_id INT DEFAULT NULL,
    FOREIGN KEY (province_id) REFERENCES Provinces(id),
    FOREIGN KEY (district_id) REFERENCES Districts(id)
);

-- Elections (optional, for extensibility)
CREATE TABLE  Elections (
    id INT AUTO_INCREMENT PRIMARY KEY,
    title VARCHAR(100) NOT NULL,
    type ENUM('local','national') NOT NULL
);

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

INSERT INTO Users (username, password, role)
  VALUES ('admin','admin124','admin');
  select * from Users where  role = 'admin';
DELETE FROM Users WHERE role = 'admin' AND username = 'admin';

-- 4. (Optional) A couple of test voters
INSERT INTO Users (username, password, role, province_id, district_id) VALUES
  ('voter1','voter124','voter', 1, 1),
  ('voter2','voter124','voter', 2, 12);
  ALTER TABLE Candidates MODIFY district_id INT DEFAULT NULL;

select * from Candidates;