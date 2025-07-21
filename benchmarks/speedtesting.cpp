#include "cyclichash.h"
#include "generalhash.h"
#include "rabinkarphash.h"
#include "threewisehash.h"
#include "ztimer.h"
#include <fstream>
#include <string>
#include <cstdint>

using namespace std;

template <class hashfunction>
double hashALot(int n, int L, uint_least32_t ttimes, uint_least32_t sizeoftest,
                vector<uint32_t> &recorder) {
  ZTimer t;
  for (uint_least32_t times = 0; times < ttimes; ++times) {
    hashfunction hf(n, L);
    for (uint_least32_t k = 0; k < static_cast<uint_least32_t>(n); ++k) {
      hf.eat(static_cast<unsigned char>(k));
    }
    for (uint_least32_t k = n; k < sizeoftest; ++k) {
      hf.update(static_cast<unsigned char>(k - n),
                static_cast<unsigned char>(k));
    }
    /* The goal of the recorder is to prevent
    the compiler from deciding that this whole computation
    is not required!
    */
    recorder.push_back(hf.hashvalue);
  }
  return t.split() / (1000.0 * ttimes);
}

template <class hashfunction>
double hashALot(int n, int L, uint_least32_t ttimes, vector<uint32_t> &recorder,
                vector<unsigned char> &data) {
  ZTimer t;
  for (uint_least32_t times = 0; times < ttimes; ++times) {
    hashfunction hf(n, L);
    for (uint_least32_t k = 0; k < static_cast<uint_least32_t>(n); ++k) {
      hf.eat(data[k]);
    }
    for (uint_least32_t k = n; k < data.size(); ++k) {
      hf.update(data[k - n], data[k]);
    }
    /* The goal of the recorder is to prevent
    the compiler from deciding that this whole computation
    is not required!
    */
    recorder.push_back(hf.hashvalue);
  }
  return t.split() / 1000.0;
}

void synthetic() {
  int L = 19;
  vector<uint32_t> recorder;
  uint_least32_t sizeoftest = 100000000;
  cout << "#n three-wise General BufferedGeneral Cyclic Karp-Rabin " << endl;
  for (uint_least32_t n = 1; n + L <= 32; ++n) {
    cout << n << " " << hashALot<ThreeWiseHash<>>(n, L, 1, sizeoftest, recorder)
         << " ";
    cout << hashALot<GeneralHash<NOPRECOMP>>(n, L, 1, sizeoftest, recorder)
         << " ";
    cout << hashALot<GeneralHash<FULLPRECOMP>>(n, L, 1, sizeoftest, recorder)
         << " ";
    cout << hashALot<CyclicHash<>>(n, L + n, 1, sizeoftest, recorder) << " ";
    cout << hashALot<KarpRabinHash<>>(n, L, 1, sizeoftest, recorder) << endl;
  }
  cout << "# L= " << L << " char-length= " << sizeoftest << endl;
}

void grabFileContent(vector<unsigned char> &data, string filename) {
  string line;
  ifstream file(filename.c_str());
  std::getline(file, line);
  while (file.good()) {
    std::getline(file, line);
    for (uint_least32_t k = 0; k < line.size(); ++k)
      data.push_back(line[k]); // presumably not very fast to do it char by char
  }
  file.close();
}
void realdata(string filename) {
  int L = 19;
  vector<uint32_t> recorder;
  uint_least32_t repeats = 1;
  vector<unsigned char> data;
  grabFileContent(data, filename);
  cout << "#n three-wise General BufferedGeneral Cyclic Karp-Rabin " << endl;
  for (uint_least32_t n = 1; n + L <= 32; ++n) {
    cout << n << " " << hashALot<ThreeWiseHash<>>(n, L, repeats, recorder, data)
         << " ";
    cout << hashALot<GeneralHash<NOPRECOMP>>(n, L, repeats, recorder, data)
         << " ";
    cout << hashALot<GeneralHash<FULLPRECOMP>>(n, L, repeats, recorder, data)
         << " ";
    cout << hashALot<CyclicHash<>>(n, L + n, repeats, recorder, data) << " ";
    cout << hashALot<KarpRabinHash<>>(n, L, repeats, recorder, data) << endl;
  }
  cout << "# L= " << L << " char-length= " << data.size()
       << " repeats=" << repeats << endl;
}

int main(int params, char **args) {
  if (params == 1)
    synthetic();
  else
    realdata(args[1]);

  return 0;
}
