// NOLINT(legal/copyright)
// SYMBOL "copy"
template<typename T1>
void casadi_copy(const T1* src, casadi_int n, T1* dest) {
  casadi_int i;
  if (dest) {
    if (src) {
      for (i=0; i<n; ++i) *dest++ = *src++;
    } else {
      for (i=0; i<n; ++i) *dest++ = 0.;
    }
  }
}
