#ifndef CHARACTERHASH
#define CHARACTERHASH

#include "mersennetwister.h"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cstdint>

using namespace std;

class mersenneRNG {
public:
  mersenneRNG(uint32_t maxval) : mtr(), n(maxval){};
  uint32_t operator()() { return mtr.randInt(n); }
  void seed(uint32_t seedval) { mtr.seed(seedval); }
  void seed() { mtr.seed(); }
  uint32_t rand_max() { return n; }

private:
  MTRand mtr;
  int n;
};

template <typename hashvaluetype>
#if __cplusplus >= 201402L
constexpr
#endif
    hashvaluetype
    maskfnc(int bits) {
  assert(bits > 0);
  assert(size_t(bits) <= sizeof(hashvaluetype) * 8);
  hashvaluetype x = static_cast<hashvaluetype>(1) << (bits - 1);
  return x ^ (x - 1);
}

template <typename hashvaluetype = uint32_t, typename chartype = unsigned char>
class CharacterHash {
public:
  CharacterHash(hashvaluetype maxval) {
    if (sizeof(hashvaluetype) <= 4) {
      mersenneRNG randomgenerator(maxval);
      for (size_t k = 0; k < nbrofchars; ++k)
        hashvalues[k] = static_cast<hashvaluetype>(randomgenerator());
    } else if (sizeof(hashvaluetype) == 8) {
      mersenneRNG randomgenerator(maxval >> 32);
      mersenneRNG randomgeneratorbase((maxval >> 32) == 0 ? maxval
                                                          : 0xFFFFFFFFU);
      for (size_t k = 0; k < nbrofchars; ++k)
        hashvalues[k] = static_cast<hashvaluetype>(randomgeneratorbase()) |
                        (static_cast<hashvaluetype>(randomgenerator()) << 32);
    } else
      throw runtime_error("unsupported hash value type");
  }

  CharacterHash(hashvaluetype maxval, uint32_t seed1, uint32_t seed2) {
    if (sizeof(hashvaluetype) <= 4) {
      mersenneRNG randomgenerator(maxval);
      randomgenerator.seed(seed1);
      for (size_t k = 0; k < nbrofchars; ++k)
        hashvalues[k] = static_cast<hashvaluetype>(randomgenerator());
    } else if (sizeof(hashvaluetype) == 8) {
      mersenneRNG randomgenerator(maxval >> 32);
      mersenneRNG randomgeneratorbase((maxval >> 32) == 0 ? maxval
                                                          : 0xFFFFFFFFU);
      randomgenerator.seed(seed1);
      randomgeneratorbase.seed(seed2);
      for (size_t k = 0; k < nbrofchars; ++k)
        hashvalues[k] = static_cast<hashvaluetype>(randomgeneratorbase()) |
                        (static_cast<hashvaluetype>(randomgenerator()) << 32);
    } else
      throw runtime_error("unsupported hash value type");
  }

  enum { nbrofchars = 1 << (sizeof(chartype) * 8) };

  hashvaluetype hashvalues[1 << (sizeof(chartype) * 8)];
};

#endif
