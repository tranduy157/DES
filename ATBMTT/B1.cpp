#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <cmath>
using namespace std;

string round_keys[16];

// --- Các hàm chuy?n ð?i ---
string convertDecimalToBinary(int decimal) {
    string binary;
    while(decimal != 0) {
        binary = (decimal % 2 == 0 ? "0" : "1") + binary; 
        decimal /= 2;
    }
    while(binary.length() < 4) binary = "0" + binary;
    return binary;
}

int convertBinaryToDecimal(string binary) {
    int decimal = 0;
    int counter = 0;
    for(int i = binary.length() - 1; i >= 0; i--)
        if(binary[i] == '1') decimal += pow(2, counter++);
        else counter++;
    return decimal;
}

// --- Các hàm h? tr? ---
string shift_left_once(string key_chunk) {
    return key_chunk.substr(1) + key_chunk[0];
}

string shift_left_twice(string key_chunk) {
    return shift_left_once(shift_left_once(key_chunk));
}

string Xor(string a, string b) {
    string result = "";
    for(int i = 0; i < b.size(); i++)
        result += (a[i] != b[i]) ? "1" : "0";
    return result;
}

// --- Hàm sinh khóa ---
void generate_keys(string key) {
    int pc1[56] = { 
        57,49,41,33,25,17,9, 1,58,50,42,34,26,18,
        10,2,59,51,43,35,27, 19,11,3,60,52,44,36,
        63,55,47,39,31,23,15, 7,62,54,46,38,30,22,
        14,6,61,53,45,37,29, 21,13,5,28,20,12,4
    };
    int pc2[48] = {
        14,17,11,24,1,5,3,28,15,6,21,10,23,19,12,4,
        26,8,16,7,27,20,13,2,41,52,31,37,47,55,30,
        40,51,45,33,48,44,49,39,56,34,53,46,42,50,
        36,29,32
    };

    string perm_key = "";
    for(int i = 0; i < 56; i++) perm_key += key[pc1[i]-1];

    string left = perm_key.substr(0,28), right = perm_key.substr(28,28);
    for(int i = 0; i < 16; i++) {
        if(i == 0 || i == 1 || i == 8 || i == 15) {
            left = shift_left_once(left);
            right = shift_left_once(right);
        } else {
            left = shift_left_twice(left);
            right = shift_left_twice(right);
        }
        string combined_key = left + right;
        string round_key = "";
        for(int i = 0; i < 48; i++) round_key += combined_key[pc2[i]-1];
        round_keys[i] = round_key;
    }
}

// --- Hàm DES chính ---
string DES(string pt, bool isEncrypt) {
    // (T?t c? các b?ng: IP, E, S-box, P, IP-1 gi?ng nhý b?n c?, không ð?i)
    // Thay v? vi?t l?i toàn b?, b?n có th? gi? nguyên ph?n DES chính ð? có
    // và s? d?ng pt là chu?i nh? phân 64 bit
    // isEncrypt: true -> dùng round_keys t? 0?15, false -> 15?0
    // (N?u b?n c?n ph?n này ð?y ð?, m?nh có th? m? r?ng)
    return pt; // ch? là placeholder
}

// --- Ð?c file và ghi k?t qu? ---
string readFileBinary(const string &filename) {
    ifstream file(filename, ios::binary);
    string content, bin = "";
    char ch;
    while(file.get(ch)) {
        bin += bitset<8>(ch).to_string();
    }
    return bin;
}

void writeFileBinary(const string &filename, const string &binary) {
    ofstream file(filename, ios::binary);
    for(size_t i = 0; i < binary.size(); i += 8) {
        string byte = binary.substr(i, 8);
        char c = static_cast<char>(bitset<8>(byte).to_ulong());
        file.put(c);
    }
}

int main() {
    string filename = "data.txt";
    string key_input;

    cout << "Nhap khoa (64 bit - gom 0 va 1): ";
    cin >> key_input;
    if(key_input.length() != 64) {
        cout << "Khoa phai co 64 bit!" << endl;
        return 1;
    }

    generate_keys(key_input);

    cout << "Ban muon (1) Ma hoa hay (2) Giai ma? Nhap 1 hoac 2: ";
    int choice;
    cin >> choice;

    string data = readFileBinary(filename);
    string result = "";

    for (size_t i = 0; i < data.size(); i += 64) {
        string block = data.substr(i, 64);
        while (block.length() < 64) block += "0"; // pad n?u chýa ð? 64
        if (choice == 1) {
            result += DES(block, true); // m? hóa
        } else {
            // ð?o chi?u round_keys trý?c
            for(int i = 0; i < 8; i++) swap(round_keys[i], round_keys[15 - i]);
            result += DES(block, false); // gi?i m?
        }
    }

    if (choice == 1) {
        writeFileBinary("data.enc", result);
        cout << "Da ma hoa xong. Ket qua luu o data.enc" << endl;
    } else {
        writeFileBinary("output.txt", result);
        cout << "Da giai ma xong. Ket qua luu o output.txt" << endl;
    }

    return 0;
}

