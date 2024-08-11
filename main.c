#include <assert.h>
#include <stdio.h>

int main() {
  // iterator.
  for (int i = 0; i < 15; i++) // here we initialize i because it's only ever
                               // needed inside this loop.
  {
    int j = 0; // j is declared outside of the loop to be recycle for inside the
    for (j = 0; j < 15;
         j++) // instead of initializing the value, we can just recycling the
              // one outside of the loop, and proceed iterating through.
    {         /* code block */
    }

    assert(j == 15); // this value will always be 15 after the loops.
  }
  printf("fine\n");
}
