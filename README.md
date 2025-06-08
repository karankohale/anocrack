# 🔐 Anocrack – ZIP Password Cracker (C Language)

![GitHub repo size](https://img.shields.io/github/repo-size/karankohale/anocrack)
![GitHub last commit](https://img.shields.io/github/last-commit/karankohale/anocrack)
![GitHub release](https://img.shields.io/github/v/release/karankohale/anocrack)
![GitHub license](https://img.shields.io/github/license/karankohale/anocrack)
![Language](https://img.shields.io/badge/language-C-blue)

Anocrack is a lightweight and fast C-based command-line tool designed for **dictionary attacks on password-protected ZIP files**. Built for use on Kali Linux and other Unix-based systems, it's ideal for ethical hackers, red teamers, and cybersecurity students.

---

## 📦 Features

- ✅ Written entirely in C (no dependencies)
- ✅ Supports Hash and ZIP password cracking using a wordlist
- ✅ Can generate custom wordlists
- ✅ Easy to compile and use
- ✅ Perfect for CTFs and training labs
- ✅ Built and tested on Kali Linux

---

## 📥 How to Download

You can get the source code in two ways:

### 🔹 Option 1: Clone the Repository (Recommended)

```bash
git clone https://github.com/karankohale/anocrack.git
cd anocrack
```

### 🔹 Option 2: Download as ZIP
Click the green "Code" button at the top of this page.

Select "Download ZIP".

Extract the downloaded archive.

Open the folder named anocrack-main (or similar).

⚙️ How to Compile
Open your terminal in the directory where anocrack.c is located, then run:

```bash
gcc anocrack.c -o anocrack
```
This will generate an executable file named anocrack.

🔧 Requirement: GCC must be installed (already available in Kali Linux by default).

🚀 How to Use
Run the tool using the following syntax:
```bash
./anocrack
```

You can use any wordlist, but rockyou.txt is a popular default, located at:
```
/usr/share/wordlists/rockyou.txt
```
If not found, install it using:

```bash
sudo gzip -d /usr/share/wordlists/rockyou.txt.gz
```
📌 Output
If a password match is found, it will display:
```
Password found: <matched-password>
```
Otherwise, it will print:
```
Password not found in the provided wordlist.
```
---
⚠️ Disclaimer
This tool is intended for educational and authorized penetration testing only.

🚫 Do NOT use this tool on files, devices, or systems you don’t own or don’t have explicit permission to test.

❗ The creator is not responsible for any misuse or illegal activity.
---

---
👨‍💻 Author
Made by Karan Kohale
🎯 Cybersecurity Engineer | Ethical Hacker | Red Teamer
---

---
🪪 License
This project is licensed under the MIT License
---

---
⭐ Support This Project
If you found this tool useful:

Give the repo a ⭐ star

Share it with other cybersecurity learners
---
