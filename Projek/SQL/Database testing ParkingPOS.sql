-- Membuat database
CREATE DATABASE IF NOT EXISTS parking_db;

-- Menggunakan database yang baru dibuat
USE parking_db;

-- Membuat tabel parking_tickets
CREATE TABLE IF NOT EXISTS parking_tickets (
    id INT AUTO_INCREMENT PRIMARY KEY,
    plate_number VARCHAR(255) NOT NULL,
    checkin_time DATETIME NOT NULL,
    checkout_time DATETIME,
    total_price INT
);
