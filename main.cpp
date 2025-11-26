#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

const string USERS_FILE = "users.txt";
const string TRANSACTIONS_FILE = "transactions.txt";
const string HTML_FILE = "atm.html";

// ------------------ Data Structures ------------------
struct TransactionNode {
    string data;
    TransactionNode* next;
    TransactionNode(string d) : data(d), next(nullptr) {}
};

struct User {
    string name;
    string accNumber;
    string pin;
    double balance;
    TransactionNode* txHead;

    User(string n, string acc, string p, double b) {
        name = n;
        accNumber = acc;
        pin = p;
        balance = b;
        txHead = nullptr;
    }
};

struct BSTNode {
    User* user;
    BSTNode* left;
    BSTNode* right;
    BSTNode(User* u) : user(u), left(nullptr), right(nullptr) {}
};

// ------------------ BST CLASS ------------------
class BST {
    BSTNode* root;

    BSTNode* insert(BSTNode* node, User* u) {
        if (!node) return new BSTNode(u);
        if (u->accNumber < node->user->accNumber)
            node->left = insert(node->left, u);
        else if (u->accNumber > node->user->accNumber)
            node->right = insert(node->right, u);
        return node;
    }

    User* search(BSTNode* node, const string& accNumber, const string& pin) {
        if (!node) return nullptr;
        if (node->user->accNumber == accNumber && node->user->pin == pin)
            return node->user;
        if (accNumber < node->user->accNumber)
            return search(node->left, accNumber, pin);
        else
            return search(node->right, accNumber, pin);
    }

    void inorderSave(BSTNode* node, ofstream& fout) {
        if (!node) return;
        inorderSave(node->left, fout);
        fout << node->user->name << "," << node->user->accNumber << ","
             << node->user->pin << "," << node->user->balance << endl;
        inorderSave(node->right, fout);
    }

public:
    BST() : root(nullptr) {}
    void insert(User* u) { root = insert(root, u); }
    User* search(const string& accNumber, const string& pin) { return search(root, accNumber, pin); }
    void saveToFile() {
        ofstream fout(USERS_FILE);
        inorderSave(root, fout);
        fout.close();
    }
};

// ------------------ Transaction Functions ------------------
void addTransaction(User* user, const string& type, double amount) {
    stringstream ss;
    ss << type << ": " << amount << " BDT";
    TransactionNode* node = new TransactionNode(ss.str());
    node->next = user->txHead;
    user->txHead = node;

    ofstream fout(TRANSACTIONS_FILE, ios::app);
    fout << user->accNumber << "," << ss.str() << endl;
    fout.close();
}

// ------------------ HTML GENERATION ------------------
void generateHTML(User* user, double balance, vector<string> lastTx) {
    ofstream html(HTML_FILE);

    html << R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ATM MACHINE</title>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<style>
body {
  background: linear-gradient(-45deg, #0a192f, #001f3f, #004d4d, #002b2b);
  background-size: 400% 400%;
  animation: gradientBG 15s ease infinite;
  color: white;
  font-family: 'Poppins', sans-serif;
  display: flex;
  justify-content: center;
  align-items: flex-start;
  padding-top: 20px;
  min-height: 100vh;
  margin: 0;
  overflow-y: auto;
}
@keyframes gradientBG {
  0%{background-position:0%50%}50%{background-position:100%50%}100%{background-position:0%50%}
}
.atm-container {
  background: rgba(0,0,0,0.6);
  padding: 30px 40px;
  border-radius: 20px;
  width: 380px;
  max-width: 95%;
  box-shadow: 0 0 40px rgba(0,0,0,0.5);
  text-align: center;
  transition: all 0.5s ease;
}
h1 { color: #00e6e6; margin-bottom: 20px; }
input { width: 85%; padding: 10px; border: none; border-radius: 12px; margin: 8px 0; font-size: 16px;
background: rgba(0,0,0,0.4); color: #00ffff; text-align: center; box-shadow: 0 0 10px #00ffff inset; transition: all 0.3s ease; }
button { background: #00b3b3; color: white; border: none; padding: 12px 20px; border-radius: 12px; font-size: 16px;
cursor: pointer; margin: 8px; width: 120px; transition: all 0.3s ease; }
button:hover { box-shadow: 0 0 15px #00ffff, 0 0 30px #00ffff; transform: scale(1.05); }
.screen { background: rgba(0,0,0,0.4); border-radius: 15px; padding: 15px; margin: 15px 0; font-size: 18px; min-height: 60px; max-height: none; overflow-y:auto; transition: all 0.5s ease; }
.hidden { display: none; opacity: 0; transition: opacity 0.5s ease; }
.visible { display: block; opacity: 1; transition: opacity 0.5s ease; }
.keypad button { width: 50px; height: 50px; margin: 3px; border-radius: 12px; }
ul { list-style: none; padding-left: 0; text-align: left; max-height:120px; overflow-y:auto;}
li { padding: 3px; }
canvas { margin-top:15px; background:rgba(0,0,0,0.3); border-radius:12px; }
</style>
</head>
<body>
<div class="atm-container">
<h1>ATM MACHINE</h1>

<!-- Language toggle -->
<button onclick="toggleLanguage(); playClick()" id="lang-toggle">বাংলা</button>

<div id="login-screen" class="visible">
  <input type="text" id="accountName" placeholder="Enter Account Name"><br>
  <input type="text" id="accountNumber" placeholder="Enter Account Number"><br>
  <input type="password" id="accountPIN" placeholder="Enter 4-digit PIN"><br>
  <button onclick="login(); playClick()" id="login-btn">Login</button>
</div>

<div id="main-screen" class="hidden">
<div class="screen" id="summary">
  <p><strong id="label-name">Account Name:</strong> <span id="name-display"></span></p>
  <p><strong id="label-number">Account Number:</strong> <span id="number-display"></span></p>
  <p><strong id="label-pin">PIN:</strong> <span id="pin-display">****</span></p>
  <p><strong id="label-balance">Balance:</strong> <span id="balance-display">)HTML" << balance << R"HTML(</span> BDT</p>
</div>

<input type="number" id="amount" placeholder="Enter amount"><br>

<div class="keypad">
  <button onclick="addNumber('1'); playClick()">1</button>
  <button onclick="addNumber('2'); playClick()">2</button>
  <button onclick="addNumber('3'); playClick()">3</button><br>
  <button onclick="addNumber('4'); playClick()">4</button>
  <button onclick="addNumber('5'); playClick()">5</button>
  <button onclick="addNumber('6'); playClick()">6</button><br>
  <button onclick="addNumber('7'); playClick()">7</button>
  <button onclick="addNumber('8'); playClick()">8</button>
  <button onclick="addNumber('9'); playClick()">9</button><br>
  <button onclick="clearAmount(); playClick()">C</button>
  <button onclick="addNumber('0'); playClick()">0</button>
</div>

<button onclick="deposit(); playSuccess()" id="deposit-btn">Deposit</button>
<button onclick="withdraw(); playSuccess()" id="withdraw-btn">Withdraw</button>
<div class="screen" id="message"></div>

<h3 id="tx-title">Last 5 Transactions</h3>
<ul id="transactions">)HTML";

for(size_t i=0; i<lastTx.size(); ++i)
    html << "<li>" << lastTx[i] << "</li>";

html << R"HTML(
</ul>
<canvas id="txChart" width="300" height="150"></canvas>
<h3 id="rec-title">Recommendations</h3>
<div class="screen" id="recommendations"></div>

<button onclick="logout(); playClick()" id="logout-btn">Logout</button>
</div>
</div>

<audio id="clickSound"><source src="click.mp3" type="audio/mpeg"></audio>
<audio id="successSound"><source src="success.mp3" type="audio/mpeg"></audio>

<script>
let balance = )HTML" << balance << R"HTML(;
let transactions = [];
let totalDeposit = 0, totalWithdraw = 0;
let currentLang = 'en'; // 'en' or 'bn'

function playClick() { let s=document.getElementById('clickSound'); if(s) s.play(); }
function playSuccess() { let s=document.getElementById('successSound'); if(s) s.play(); }

function toggleLanguage(){
  currentLang = (currentLang==='en')?'bn':'en';
  document.getElementById('lang-toggle').innerText = (currentLang==='en')?'বাংলা':'English';
  updateUIText();
  updateRecommendations();
}

function updateUIText(){
  if(currentLang==='en'){
    document.getElementById('label-name').innerText = 'Account Name:';
    document.getElementById('label-number').innerText = 'Account Number:';
    document.getElementById('label-pin').innerText = 'PIN:';
    document.getElementById('label-balance').innerText = 'Balance:';
    document.getElementById('login-btn').innerText = 'Login';
    document.getElementById('deposit-btn').innerText = 'Deposit';
    document.getElementById('withdraw-btn').innerText = 'Withdraw';
    document.getElementById('logout-btn').innerText = 'Logout';
    document.getElementById('tx-title').innerText = 'Last 5 Transactions';
    document.getElementById('rec-title').innerText = 'Recommendations';
    document.getElementById('accountName').placeholder = 'Enter Account Name';
    document.getElementById('accountNumber').placeholder = 'Enter Account Number';
    document.getElementById('accountPIN').placeholder = 'Enter 4-digit PIN';
    document.getElementById('amount').placeholder = 'Enter amount';
  } else {
    document.getElementById('label-name').innerText = 'হিসাবের নাম:';
    document.getElementById('label-number').innerText = 'হিসাব নম্বর:';
    document.getElementById('label-pin').innerText = 'পিন:';
    document.getElementById('label-balance').innerText = 'ব্যালেন্স:';
    document.getElementById('login-btn').innerText = 'লগইন';
    document.getElementById('deposit-btn').innerText = 'জমা দিন';
    document.getElementById('withdraw-btn').innerText = 'উত্তোলন';
    document.getElementById('logout-btn').innerText = 'প্রস্থান';
    document.getElementById('tx-title').innerText = 'শেষ ৫টি লেনদেন';
    document.getElementById('rec-title').innerText = 'সুপারিশসমূহ';
    document.getElementById('accountName').placeholder = 'হিসাবের নাম লিখুন';
    document.getElementById('accountNumber').placeholder = 'হিসাব নম্বর লিখুন';
    document.getElementById('accountPIN').placeholder = '৪-অঙ্কের পিন লিখুন';
    document.getElementById('amount').placeholder = 'টাকার পরিমাণ লিখুন';
  }
}

function speak(text){
  if('speechSynthesis' in window){
    let utter = new SpeechSynthesisUtterance(text);
    utter.lang = (currentLang==='en')?'en-US':'bn-BD';
    utter.rate=1; utter.pitch=1;
    speechSynthesis.speak(utter);
  }
}

// ------------------ MAIN FUNCTIONS ------------------
function login() {
  const name = document.getElementById('accountName').value.trim();
  const number = document.getElementById('accountNumber').value.trim();
  const pin = document.getElementById('accountPIN').value.trim();
  if(name===''||number===''||pin===''){
    alert((currentLang==='en')?'Fill all fields!':'সব ফিল্ড পূরণ করুন!');
    return;
  }
  if(number!=='12345678'||pin!=='1234'){
    alert((currentLang==='en')?'Incorrect credentials':'ভুল তথ্য');
    return;
  }
  document.getElementById('login-screen').classList.add('hidden');
  document.getElementById('main-screen').classList.remove('hidden');
  document.getElementById('name-display').innerText = name;
  document.getElementById('number-display').innerText = number;
  document.getElementById('pin-display').innerText = '****';
  updateTransactions(); updateChart(); updateRecommendations();
}

function deposit() {
  let amt = parseFloat(document.getElementById('amount').value);
  if(isNaN(amt)||amt<=0){showMessage((currentLang==='en')?'Enter valid deposit':'সঠিক জমা লিখুন','red'); return;}
  balance += amt;
  transactions.unshift(new Date().toLocaleString() + ' - ' + ((currentLang==='en')?'Deposit':'জমা') + ': ' + amt.toFixed(2) + ' BDT');
  totalDeposit += amt;
  if(transactions.length>5) transactions.pop();
  updateBalance(); updateTransactions(); showMessage((currentLang==='en')?'Deposit successful':'জমা সফল','#00e6e6'); updateChart(); updateRecommendations(); saveData();
}

function withdraw() {
  let amt = parseFloat(document.getElementById('amount').value);
  if(isNaN(amt)||amt<=0){showMessage((currentLang==='en')?'Enter valid withdrawal':'সঠিক উত্তোলন লিখুন','red'); return;}
  if(amt>balance){showMessage((currentLang==='en')?'Insufficient balance':'ব্যালেন্স অপর্যাপ্ত'); speak((currentLang==='en')?'Insufficient balance':'ব্যালেন্স অপর্যাপ্ত'); return;}
  balance -= amt;
  transactions.unshift(new Date().toLocaleString() + ' - ' + ((currentLang==='en')?'Withdraw':'উত্তোলন') + ': ' + amt.toFixed(2) + ' BDT');
  totalWithdraw += amt;
  if(transactions.length>5) transactions.pop();
  updateBalance(); updateTransactions(); showMessage((currentLang==='en')?'Withdrawal successful':'উত্তোলন সফল','#00e6e6'); checkAlerts(amt); updateChart(); updateRecommendations(); saveData();
}

function addNumber(num){document.getElementById('amount').value+=num;}
function clearAmount(){document.getElementById('amount').value='';}
function showMessage(msg,color){const e=document.getElementById('message');e.style.color=color;e.innerText=msg;}
function updateBalance(){document.getElementById('balance-display').innerText=balance.toFixed(2);}
function updateTransactions(){
  const ul=document.getElementById('transactions'); ul.innerHTML='';
  transactions.forEach(tr=>{let li=document.createElement('li'); li.innerText=tr; ul.appendChild(li);});
}
function logout(){
  document.getElementById('amount').value='';
  document.getElementById('login-screen').classList.remove('hidden');
  document.getElementById('main-screen').classList.add('hidden');
  showMessage('','');
  balance = )HTML" << balance << R"HTML(;
  transactions=[]; totalDeposit=0; totalWithdraw=0; updateTransactions(); updateChart(); updateRecommendations();
}

// ------------------ Transaction Analytics ------------------
function updateChart(){
  const ctx = document.getElementById('txChart').getContext('2d');
  if(window.txChartInstance) window.txChartInstance.destroy();
  window.txChartInstance = new Chart(ctx, {
    type: 'pie',
    data: {
      labels: [(currentLang==='en')?'Deposits':'জমা',(currentLang==='en')?'Withdrawals':'উত্তোলন'],
      datasets: [{
        data: [totalDeposit, totalWithdraw],
        backgroundColor: ['#00e6e6','#ff4d4d']
      }]
    }
  });
}

// ------------------ Recommendations ------------------
function updateRecommendations(){
  let rec = '';
  if(balance<1000) rec += (currentLang==='en')?'⚠️ Low balance: consider depositing more funds.<br>':'⚠️ কম ব্যালেন্স: আরও জমা দিন।<br>';
  if(totalWithdraw>5000) rec += (currentLang==='en')?'⚠️ High withdrawals detected: monitor spending.<br>':'⚠️ অনেক টাকা উত্তোলন হয়েছে: খরচ মনিটর করুন।<br>';
  if(transactions.length===0) rec += (currentLang==='en')?'Make your first transaction to get started.<br>':'প্রথম লেনদেন করুন।<br>';
  document.getElementById('recommendations').innerHTML = rec;
  speak(rec);
}

// ------------------ Alerts ------------------
function checkAlerts(amount){
  if(balance<1000) {showMessage((currentLang==='en')?'Low balance warning!':'কম ব্যালেন্স সতর্কতা','#ffcc00'); speak((currentLang==='en')?'Warning: low balance':'সতর্কতা: কম ব্যালেন্স');}
  if(amount>5000) {showMessage((currentLang==='en')?'Fraud alert: unusually high withdrawal':'প্রতারণা সতর্কতা: বড় উত্তোলন','#ff3333'); speak((currentLang==='en')?'Fraud alert: unusually high withdrawal':'প্রতারণা সতর্কতা: বড় উত্তোলন');}
}

// ------------------ Voice Support ------------------
function listen(){
  if('webkitSpeechRecognition' in window){
    let recognition = new webkitSpeechRecognition();
    recognition.lang = (currentLang==='en')?'en-US':'bn-BD';
    recognition.interimResults = false;
    recognition.onresult = function(event){
      let cmd = event.results[0][0].transcript.toLowerCase();
      processVoiceCommand(cmd);
    };
    recognition.start();
  }else{alert((currentLang==='en')?'Speech recognition not supported':'ভয়েস চিনতে পারছে না');}
}

function processVoiceCommand(cmd){
  if(cmd.includes((currentLang==='en')?'deposit':'জমা')){
    let amt = parseFloat(cmd.replace(/\D/g,''));
    if(!isNaN(amt)){document.getElementById('amount').value=amt; deposit();}
  } else if(cmd.includes((currentLang==='en')?'withdraw':'উত্তোলন')){
    let amt = parseFloat(cmd.replace(/\D/g,''));
    if(!isNaN(amt)){document.getElementById('amount').value=amt; withdraw();}
  } else if(cmd.includes((currentLang==='en')?'balance':'ব্যালেন্স')) { speak((currentLang==='en')?'Your balance is '+balance.toFixed(2)+' BDT':'আপনার ব্যালেন্স '+balance.toFixed(2)+' টাকা');}
  else if(cmd.includes((currentLang==='en')?'transactions':'লেনদেন')) { speak((currentLang==='en')?'Showing last transactions':'শেষ লেনদেন দেখাচ্ছে');}
  else { speak((currentLang==='en')?'Command not recognized':'কমান্ড চিহ্নিত হয়নি'); }
}

// ------------------ Data Save (Offline) ------------------
function saveData(){ /* offline demo: no server */ }
</script>
<button onclick="listen(); playClick()">🎤 Voice Command</button>
</body></html>)HTML";

    html.close();
}

// ------------------ OPEN WEB ------------------
void openWeb(const string& filename) {
#ifdef _WIN32
    system(("start " + filename).c_str());
#elif __APPLE__
    system(("open " + filename).c_str());
#else
    system(("xdg-open " + filename).c_str());
#endif
}

// ------------------ MAIN ------------------
int main() {
    BST bst;

    User* demo = new User("Imam", "12345678", "1234", 50000);
    bst.insert(demo);

    vector<string> lastTx;
    ifstream fin(TRANSACTIONS_FILE);
    string line;
    while(getline(fin,line)) lastTx.push_back(line);
    fin.close();

    generateHTML(demo, demo->balance, lastTx);
    openWeb(HTML_FILE);

    cout << "ATM simulation webpage generated and opened successfully!" << endl;
    cout << "Bangla/English toggle, voice support, analytics, alerts, and recommendations are active." << endl;
    return 0;
}
