# Incident and Escalation (IE) Management System

## 📋 Overview

The **IE Management System** is a C++ console application designed to track and manage organizational incident cases. It provides role-based access for **Admins** and **Managers**, with support for case creation, assignment, status updates, history tracking, and report generation.

This project uses structured programming techniques and static arrays, making it simple yet powerful for academic or lightweight production use.

---

## 🚀 Features

### 👤 User Roles

- **Admin**
  - Full access to all features
  - Manage managers and admins
  - Assign/export/close cases
  - Generate reports

- **Manager**
  - Access only to assigned cases
  - Add actions and update case progress
  - Export or close assigned cases

---

### 🧰 Core Functionalities

- **Case Management**
  - Add, view, edit, delete, assign managers
  - Add actions and status updates
  - Export and close cases

- **Manager Management**
  - Add, edit, toggle status, and delete managers

- **Admin Management**
  - Admin login system using `Admins.txt`
  - Add new admin credentials

- **Reporting**
  - Generate daily text reports for case summaries

- **Data Persistence**
  - Human-readable save/load from `IE.txt`
  - Case and manager data maintained across sessions

---

## 📁 Files

- `P.cpp` — Main source code
- `IE.txt` — Data storage file (auto-generated)
- `Admins.txt` — Admin credentials file (created manually or by the app) Fromat (adminusername:adminpassword)
- `case_report_YYYY-MM-DD.txt` — Auto-generated case reports

---

## 🛠️ How to Compile and Run

### 💻 Requirements
- A C++ compiler (e.g., `g++`)

### 📦 Compile
```bash
g++ -o IE_System P.cpp
``` 

### 📌 Notes
- Maximum limits:
      100 Cases
      20 Managers
      50 Actions per case
      5 Assigned Managers per case
- The system uses static arrays (no STL vectors) for compatibility and simplicity.

- The app auto-generates timestamps for actions and case creation.
