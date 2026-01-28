#include <cassert>
#include "bigint.h"

// Notes: 
// Vector is a resizable array, ArrayList. 
// uint64_t is an unsigned 64-bit integer type.
// the range is 0 → 18,446,744,073,709,551,615


// Constructor initializes to given values
BigInt::BigInt(std::initializer_list<uint64_t> vals, bool negative)
{
  this->bit_vector = vals;
  this->negative = negative;
}

// Constructor initializes to other's values
BigInt::BigInt(const BigInt &other)
{
  this->bit_vector = other.bit_vector;
  this->negative = other.negative;
}

// !! Destructor empty since vector and bool will clean up itself
BigInt::~BigInt()
{
}

BigInt &BigInt::operator=(const BigInt &rhs)
{
  // TODO: implement
}

bool BigInt::is_negative() const
{
  return this->negative;
}

const std::vector<uint64_t> &BigInt::get_bit_vector() const 
{
  return this->bit_vector; // return reference to internal vector
}

uint64_t BigInt::get_bits(unsigned index) const
{
  // If index is within bounds of vector, return the value at that index
  if (this->bit_vector.size() > index && index >= 0) {
    return this->bit_vector[index];
  } else {
    return 0;
  }
}

BigInt BigInt::operator+(const BigInt &rhs) const
{
  // TODO: implement
}

BigInt BigInt::operator-(const BigInt &rhs) const
{
  // Hint: a - b could be computed as a + -b
}

BigInt BigInt::operator-() const
{
  // If value is zero, return zero (sign doesn't matter)
  if (this->bit_vector.size() == 1 && this->bit_vector[0] == 0) {
    return BigInt(0, false);
  } else {
    // Return new BigInt with same bit_vector but flipped sign
    BigInt copy(*this);
    copy.negative = !this->negative;
    return copy;
  }
}

bool BigInt::is_bit_set(unsigned n) const
{
  // TODO: implement
}

BigInt BigInt::operator<<(unsigned n) const
{
  // TODO: implement
}

BigInt BigInt::operator*(const BigInt &rhs) const
{
  // TODO: implement
}

BigInt BigInt::operator/(const BigInt &rhs) const
{
  // TODO: implement
}

int BigInt::compare(const BigInt &rhs) const
{
  // TODO: implement
}

std::string BigInt::to_hex() const
{
  if (this->bit_vector.size() == 0 ||
      (this->bit_vector.size() == 1 && this->bit_vector[0] == 0)) {
    return "0";
  }
  // '-' sign if negative
  std::string result;
  if (this->negative) { 
    result += '-'; 
  }
  
  const char* hex = "0123456789abcdef";
  bool first = true;

  // Convert to hex, the most significant append first (last in the vector)
  for (size_t i = this->bit_vector.size(); i-- > 0;) { 
    
    std::string part;
    uint64_t val = bit_vector[i];

    // Run 16 times (64/4)
    for (int j = 0; j < 16; j++) { 
      int position = val & 0xF; // leaves only the last four bits
      part = hex[position] + part; // insert at front (last is most significant)
      val = val >> 4; // shift right by 4 bits to process next digit
    }
    if (first) {
        // strip leading zeros from most significant part
        size_t pos = part.find_first_not_of('0');
        if (pos == std::string::npos) { // not found (guard against all zeros)
            part = "0";
        } else {
            part.erase(0, pos);
        }
        first = false;
    }
    result += part; // first processed is most significant
  }
  return result;
}

std::string BigInt::to_dec() const
{
  // TODO: implement
}
