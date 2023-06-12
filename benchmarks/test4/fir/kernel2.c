#define NDATA 1024
#define NTAPS 8

int input[NDATA];
int output[NDATA];


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
  int coefficients[NTAPS] = {
    025, 150, 375, -225, 050, 075, -300, 125};
  for (j = NTAPS-1; j < NDATA; ++j)
  {
#ifdef CGRA_COMPILER
       please_map_me();
 #endif
    int sum = 0;
    #pragma unroll
    for (i = 0; i < NTAPS; ++i)
    {
      sum += input[j - i] * coefficients[i];
    }
    output[j] = sum;
  }
}