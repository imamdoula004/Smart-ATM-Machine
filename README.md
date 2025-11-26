# Smart-ATM-Machine
A C++ Smart ATM Simulator that generates an interactive web-based ATM interface with English/Bangla toggle, voice commands, transaction history, analytics charts, alerts, and recommendations. Uses BST for users, linked lists for transactions, and outputs a full ATM UI as atm.html. 

Smart ATM Machine is a lightweight C++ demo that generates a single-page ATM web app (atm.html) from local data.
Built as an educational simulation to demonstrate a simple in-memory BST user store, linked-list style transaction history, and a browser UI (with speech, simple analytics and recommendations). Not for production.

## Table of contents

1. Overview
2. Key features
3. What’s included (files)
4. Quick start (build & run)
5. How it works (high level)
6. Data formats
7. Security & limitations (important)
8. Suggested improvements
9. Contributing
10. License (MIT)

## Overview

**This repository contains a compact C++ program that:**
- Builds a demo user (BST-backed User object) and reads an optional transactions.txt.
- Generates a polished atm.html file (includes UI, Chart.js analytics, voice and speech features) and opens it in your default browser.

**Demonstrates:**
- BST structure for users
- Simple transaction linked list per user
- HTML/JS UI generation from C++
- Basic client-side voice and speech synthesis usage
- A simple transactions pie chart

This project is intended for learning and prototyping UI → native code interactions. It is not secure, and should never be used with real banking data.


## Key features

- Single binary generates a production-like ATM page: atm.html.
- Multilingual UI toggle (English / Bangla).
- Voice output (speechSynthesis) and optional voice commands (webkitSpeechRecognition when available).
- Transaction tracking (appends to transactions.txt) and last-N listing shown in UI.
- Simple analytics chart (Chart.js) for deposit/withdraw totals.
- Alerts and recommendations generated client-side.
- Cross-platform openWeb helper uses start / open / xdg-open to open the generated file.

## What’s included

`atm.cpp (or main.cpp)` — the main C++ source.
`users.txt` — (optional) file format for persistent users (CSV lines: name,accNumber,pin,balance) — used by the BST save/load functions (currently save only).
`transactions.txt` — append-only transaction log (accNumber,description) read at startup to show history.
`atm.html` — created by the program at runtime.
`click.mp3, success.mp3`— optional audio files for UI feedback (place in same directory).
README (this file) and the MIT License block below.

## Quick start (build & run)
**Requirements**
- A C++ compiler (g++/clang / MSVC).
- Internet connection (for Chart.js CDN), or host chart.js locally.
- A modern browser (Chrome recommended for Web Speech APIs).

**Compile**
`g++ -std=c++11 atm.cpp -o atm`
(On Windows you may use MinGW/MSVC; adjust flags if needed.)

**Run**
`./atm`
This produces atm.html and attempts to open it in your default browser. If audio is used, ensure click.mp3 and success.mp3 are present (or remove audio tags in generated HTML).

## How it works (high level)
- The C++ program builds an in-memory BST with User nodes (each User holds a linked list of TransactionNode).
- It reads existing lines from transactions.txt into a vector and passes the last transactions into the HTML generator.
- `generateHTML(...)` writes a full HTML document (UI, CSS, JS) to atm.html. The HTML includes:
  1. Login form (client-side demo login check)
  2. Deposit/Withdraw functions (client-side JS)
  3. Pie chart (Chart.js)
  4. Voice support (speech synthesis + recognition).

When transactions occur via the HTML UI, saveData() is a placeholder: currently changes remain browser-side only. The C++ side can append transactions when you call addTransaction() from C++ code paths.

**Data formats**
`users.txt`

*Each line is a CSV record:*
Name, AccountNumber, PIN, Balance

*Example:*
Imam, 12345678, 1234, 50000

`transactions.txt`
Append-only CSV lines the program writes in the form:
AccountNumber, Description

*Example:*
12345678,Deposit: 1000 BDT
12345678,Withdraw: 200 BDT


The HTML UI also keeps a client-side transactions array (in JS). Currently, the demo login logic is hardcoded inside the HTML to check accountNumber === '12345678' && pin === '1234'. This is purely for demo purposes.

## Security & limitations (read this)

- This is an educational demo — do not use with real accounts or sensitive info.
- Plaintext PINs and account numbers — stored and checked in plaintext — insecure.
- Client-side authentication in the generated HTML is insecure and easily bypassed. The JS login is hardcoded to demo credentials.
- No encryption, no hashing applied to PINs or sensitive data.
- No server; persistent changes made in the browser are not synchronized back into users.txt unless you modify the C++/I/O logic.
- Speech APIs: voice recognition requires browser support (Chrome has best support). Results vary by platform and settings.
- Fraud checks are naive (hardcoded thresholds). Not suitable for real fraud detection.

## Suggested improvements

If you want to take it beyond a demo, consider:
- Implement a secure backend (HTTPS) and server-side authentication.
- Hash + salt PINs using a secure algorithm (bcrypt/Argon2) and never store plaintext.
- Replace client-side login with server auth and session tokens.
- Properly parse and load users.txt into the BST on startup and persist changes (save file safely).
- Make saveData() actually POST to a local server or write to files securely.
- Add input validation / sanitize any user inputs to prevent XSS in generated HTML (if user-provided strings are inserted).
- Replace system-dependent openWeb() with a more controlled approach or just print the path and let the user open it.
- Unit tests for BST and transaction storage logic.

## Code map / function summary
1. `struct User` — stores user meta, balance, txHead linked list.
2. `struct BSTNode + class BST` — insert/search/save operations for users (inorder save to users.txt).
3. `addTransaction(User*, type, amount)` — creates a TransactionNode and appends to transactions.txt.
4. `generateHTML(User*, balance, lastTx)` — writes atm.html (CSS + JS + Chart.js logic).
5. `openWeb(filename)` — platform-dependent system call to open the generated HTML.
6. `main()` — demo user creation, read transactions.txt, generate HTML, open page.

## Contributing

- Contributions are welcome. If you open issues or PRs, please:
- Keep changes focused (e.g., “implement secure PIN storage” or “real server to persist transactions”).
- Include unit tests for core C++ structures.
- Document any security changes clearly.

## License (MIT)
MIT License

Copyright (c) 2025 Imam

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
