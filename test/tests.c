#include "buffer_tests.h"
#include "reader_tests.h"
#include "analyzer_tests.h"

int main(void) {
  buffer_tests();
  reader_tests();
  analyzer_tests();
  return 0;
}