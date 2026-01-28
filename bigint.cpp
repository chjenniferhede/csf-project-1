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

BigInt::~BigInt()
{
  // !! Destructor empty since vector will clean up itself
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
  // Note: this is a const reference return type, so 
  // we can just return the internal vector directly
  return this->bit_vector;
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
  // '-' sign if negative
  std::string result;
  if (this->negative) { 
    result += '-'; 
  }
  
  // Convert to hex, the most significant append first (last in the array)
  for (int i = this->bit_vector.size() - 1; i >= 0; i--) { 
    result += to_hex_digit(bit_vector[i]);
  }
  return result;
}

std::string BigInt::to_dec() const
{
  // TODO: implement
}

// Helper function to convert a uint64_t to hex string
std::string BigInt::to_hex_digit(uint64_t val) const
{
  std::string result;
  const char* hex = "0123456789abcdef";
  
  // Run 16 times (64/4)
  for (int i = 0; i < 16; i++) { 
    uint64_t temp = val; 
    int position = temp & 0xF; // this leaves only the last four digit
    result = hex[position] + result; // insert at front
    val = val >> 4; // shift right by 4 bits to process next digit
  }

  return result;
}
