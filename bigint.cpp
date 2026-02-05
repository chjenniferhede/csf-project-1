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

// Copy Constructor initializes to other's values
BigInt::BigInt(const BigInt &other)
{
  this->bit_vector = other.bit_vector;
  this->negative = other.negative;
}

// !! Destructor empty since vector and bool will clean up itself
BigInt::~BigInt()
{
}

// Override the assignment operator
BigInt &BigInt::operator=(const BigInt &rhs)
{
  if (this != &rhs) { 
    this->bit_vector = rhs.bit_vector;
    this->negative = rhs.negative;
  }
  // 'this' is a pointer, *this is the BigInt obj
  return *this;
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
  // Same sign 
  if (this->negative == rhs.is_negative()) { 
    BigInt result = add_magnitudes(*this, rhs);
    result.negative = this->negative;
    return result;
  }

  // Different sign
  BigInt result = subtract_magnitudes(*this, rhs); 

  // The sign of the result is the same as the one with larger magnitude
  if (compare_magnitudes(*this, rhs) >= 0) { 
    result.negative = this->negative; 
  } else {
    result.negative = rhs.is_negative(); 
  }
  return result;
  
}

BigInt BigInt::operator-(const BigInt &rhs) const
{
  return *this + (-rhs);
}

BigInt BigInt::operator-() const
{
  // If value is zero, return zero (sign doesn't matter)
  if (is_zero) {
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
  // Each uint64_t has 64 bits
  unsigned index = n / 64; // which uint64_t in the vector
  unsigned bitPos = n % 64; // which bit in that uint64
  uint64_t chunk = get_bits(index); // get the uint64_t at that index
  // true if n is set to 1
  if (chunk == 0) return false; // out of bound
  return (chunk & (1ULL << bitPos)) != 0; // 1ULL is 64-bit 1
}

BigInt BigInt::operator<<(unsigned n) const
{
  if (is_zero() || n == 0) return *this; 

  BigInt copy(*this);

  // Handle multiplication of 64 first
  int pos = n / 64; 
  copy.bit_vector.insert(copy.bit_vector.begin(), pos, 0);
  if (n % 64 == 0) return copy;
  
  // Setup 
  int bitShift = n % 64;
  uint64_t carry = 0;

  // Shift bits in uint64_t to left by bitShift
  for (size_t i = 0; i < copy.bit_vector.size(); i++) {
    uint64_t current = copy.bit_vector[i];
    // bits that will overflow to the next uint64_t: leftmost bitShift bits of current
    uint64_t newCarry = current >> (64 - bitShift); 
    copy.bit_vector[i] = (current << bitShift) | carry; // shift and add carry from last
    carry = newCarry; 
  } 

  // if carry left, there is a new most significant uint64_t at the end
  if (carry > 0) {
    copy.bit_vector.push_back(carry); 
  }

  return copy;
}

BigInt BigInt::operator*(const BigInt &rhs) const
{
  // Setup
  BigInt result = BigInt(0, false); 
  if (this->is_zero() || rhs.is_zero()) return result;

  // Compute the number of bits to decompose
  size_t decomposeSize = this->bit_vector.size() * 64;

  // For each bit in this,
  for (int i = 0; i < decomposeSize; i++) { 
    // if it is a '1', start from least sig, left shift i bits in rhs
    if (this->is_bit_set(i)) { 
      result = result + (rhs << i);
    }
  }

  // Correct the sign of the result
  if (this->negative != rhs.negative) {
    result.negative = true;
  } 
  return result;
}

BigInt BigInt::operator/(const BigInt &rhs) const
{
  // TODO: implement
}

int BigInt::compare(const BigInt &rhs) const
{
  // Equal
  if (compare_magnitudes(*this, rhs) == 0 && this->negative == rhs.negative) return 0;

  // Different sign
  if (this->negative != rhs.negative) { 
    return (this->negative) ? -1 : 1; // if 'this' is negative, rhs < lhs
  }

  // Same sign
  // if 'this' has larger magnitude, both negative means larger mag is smaller
  if (compare_magnitudes(*this, rhs) == 1) return (this->negative) ? -1 : 1; 
  // if 'this' has smaller magnitude, both negative means smaller mag is bigger
  if (compare_magnitudes(*this, rhs) == -1) return (this->negative) ? 1 : -1; 
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

    // Run 16 times (64/4) from the first 4 bits of this uint64_t to the last
    for (int j = 0; j < 16; j++) { 
      int position = val & 0xF; // leaves only the last four bits
      part = hex[position] + part; // insert at front (last is most significant)
      val = val >> 4; // shift right by 4 bits to process next digit
    }
    if (first) {
        // strip leading zeros from most significant part
        size_t pos = part.find_first_not_of('0');
        if (pos == std::string::npos) { // npos means not found
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

// Helper functions
bool BigInt::is_zero() const
{
  return (this->bit_vector.size() == 1 && this->bit_vector[0] == 0);
}

// Add the magnitudes ignoring the signs
BigInt BigInt::add_magnitudes(const BigInt &lhs, const BigInt &rhs)
{

  int maxSize = std::max(lhs.get_bit_vector().size(), rhs.get_bit_vector().size());

  // Pad the smaller vector with zeros
  std::vector<uint64_t> lhsPadded = lhs.get_bit_vector();
  std::vector<uint64_t> rhsPadded = rhs.get_bit_vector();

  while (lhsPadded.size() < maxSize) lhsPadded.push_back(0);
  while (rhsPadded.size() < maxSize) rhsPadded.push_back(0);
  
  // Initiate result array and carry
  std::vector<uint64_t> resultVec;
  uint64_t carry = 0; 

  // Adding from the least significant
  for (size_t i = 0; i < maxSize; i++) {
    uint64_t sum = lhsPadded[i] + rhsPadded[i] + carry;
    carry = (sum < lhsPadded[i]) ? 1 : 0;
    resultVec.push_back(sum);
  }

  // if there is a carry left
  if (carry > 0) {
    resultVec.push_back(carry);
  }

  BigInt result; 
  result.bit_vector = resultVec;
  result.negative = false; // does not matter
  return result;
}

BigInt BigInt::subtract_magnitudes(const BigInt &lhs, const BigInt &rhs)
{
  // If there is no difference, return 0
  if (compare_magnitudes(lhs, rhs) == 0) return BigInt(0, false);

  // Find the larger magnitude 
  const BigInt *actualL = &lhs; // larger
  const BigInt *actualR = &rhs; // smaller
  if (compare_magnitudes(lhs, rhs) < 0) {
    actualL = &rhs;
    actualR = &lhs;
  }

  // Get the bit vectors
  const auto lhsVec = actualL->get_bit_vector();
  const auto rhsVec = actualR->get_bit_vector();  

  // Setup 
  std::vector<uint64_t> resultVec;
  uint64_t borrow = 0;

  // Substracting
  for (size_t i = 0; i < lhsVec.size(); i++) {
    uint64_t diff = lhsVec[i] - rhsVec[i] - borrow;
    borrow = (diff > lhsVec[i]) ? 1 : 0;
    resultVec.push_back(diff);
  }

  // Remove leading zeros
  while (resultVec.size() > 1 && resultVec.back() == 0) {
    resultVec.pop_back();
  }

  BigInt result; 
  result.bit_vector = resultVec;
  result.negative = false; // does not matter
  return result;
}

int BigInt::compare_magnitudes(const BigInt &lhs, const BigInt &rhs) 
{
  // Use reference to avoid copying
  const auto &lhsVec = lhs.get_bit_vector();
  const auto &rhsVec = rhs.get_bit_vector();

  // If one has more digits, it is larger
  if (lhsVec.size() > rhsVec.size()) return 1; 
  if (lhsVec.size() < rhsVec.size()) return -1; 
  
  // If same digits. compare starting from most significant digit (last in array)
  for (size_t i = lhsVec.size(); i-- > 0;) { 
    if (lhsVec[i] > rhsVec[i]) return 1; 
    if (lhsVec[i] < rhsVec[i]) return -1;
  }

  // If the loop went through without returning
  return 0;

}

// This is a right shift >> by 1
BigInt BigInt::div_by_2() const
{
  // Setup
  if (is_zero()) return *this;
  BigInt copy(*this);
  uint64_t carry = 0;

  // Loop starting from the most significant
  for (size_t i = copy.bit_vector.size(); i-- > 0;) { 
    uint64_t temp = (copy.bit_vector[i] & 1) << 63;
    copy.bit_vector[i] = (copy.bit_vector[i] >> 1) | carry; // Moved right, put carry as most sig
    carry = temp; // set next carry
  }

  // Most sig uint could became 0
  if (copy.bit_vector.back() == 0 && copy.bit_vector.size() > 1) {
    copy.bit_vector.pop_back();
  }

  return copy;  
}
