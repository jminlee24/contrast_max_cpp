#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

string stringToBinary(const string &str) {
  string binaryString;
  for (char c : str) {
    binaryString += bitset<8>(c).to_string();
  }
  return binaryString;
}

vector<string> decodeBinary(const string &str) {
  vector<string> vals;
  int decoded = stoi(str, nullptr, 2);

  // 4 most significant bytes define the word type
  int flag = (decoded & 15 << 12) >> 12;

  if (flag == 0b0000) {
    // EVT_ADDRE_Y : Y coordinate and system type (master/slave camera)
    int system_type = (decoded & 0b0000100000000000) >> 11;
    int y_coord = decoded & 0b0000011111111111;
    //   cout << y_coord << " ";
  } else if (flag == 0b0010) {
    // EVT_ADDR_X : Single valid event, X coord and polarity
  } else if (flag == 0b0011) {
    // VEC_BASE_X Base X coordiante and polarity for subsequent vector events
  } else if (flag == 0b0100) {
    // VEC_12 vector event with 12 consectuive els

  } else if (flag == 0b0101) {
    // VEC_8 vector event with 8 consectuive els
  }
  //  cout << (decoded << 4) << " | " << str << endl;

  return vals;
}

int main() {
  string myText;
  vector<string> vals;

  ifstream MyReadFile("../data/recording.raw");

  while (getline(MyReadFile, myText)) {
    if (myText[0] != '%') {
      string val = stringToBinary(myText);
      int length = val.length();

      for (int i = 128; i < length; i += 16) {
        decodeBinary(val.substr(i, 16));
        if (i > 100000)
          break;
      }
    } else {
      cout << myText << endl;
    }
  }

  MyReadFile.close();

  return 0;
}
