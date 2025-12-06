// Extension functions for HElib Ctxt to support HE-Bridge encoding switching
#include <helib/Context.h>
#include <helib/Ctxt.h>
#include <helib/assertions.h>

namespace helib {

// Decrease the plaintext modulus only (not the ciphertext)
// Used in encoding switching: reduce from p^r to p without affecting the ciphertext
void Ctxt::divideModByP()
{
  // Special case: if *this is empty then do nothing
  if (this->isEmpty())
    return;

  long p = getContext().getP();
  assertEq(ptxtSpace % p, 0l, "p must divide ptxtSpace");
  assertTrue(ptxtSpace > p, "ptxtSpace must be strictly greater than p");

  noiseBound /= p;        // noise is reduced by a p factor
  ptxtSpace /= p;         // and so is the plaintext space
  intFactor %= ptxtSpace; // adjust intFactor
}

} // namespace helib
