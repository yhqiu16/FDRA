#define NDATA 1024
#define NTAPS 8

int input[NDATA];
int output[NDATA];
int coefficients[NTAPS] = {
    025, 150, 375, -225, 050, 075, -300, 125};

// void kernel(int input[], int output[], int coefficient[]);

void kernel(); //(int input[], int output[], int coefficients[]);

int main()
{

  //  input_dsp (input, NTAPS, 0);

  kernel(); //(input, output, coefficients);

  //  output_dsp (input, NTAPS, 0);
  //  output_dsp (coefficients, NTAPS, 0);
  //  output_dsp (output, NTAPS, 0);
  return 0;
}

__attribute__((noinline)) void kernel() //(int input[], int output[], int coefficients[])
/*   input :           input sample array */
/*   output:           output sample array */
/*   coefficient:      coefficient array */
{
  int i, j;
  for (j = NTAPS-1; j < NDATA; ++j)
  {
    int sum = 0;
    #pragma unroll 4
    for (i = 0; i < NTAPS; ++i)
    {
 #ifdef CGRA_COMPILER
       please_map_me();
 #endif
      sum += input[j - i] * coefficients[i];
    }
    output[j] = sum;
  }
}