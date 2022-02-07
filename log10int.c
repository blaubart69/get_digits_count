#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <string.h>


typedef uint8_t (*fn_count_digits)(const uint64_t);

//													          09876543210987654321	
// A ULONGLONG is a 64-bit unsigned integer (range: 0 through 18446744073709551615)
//
uint8_t get_decimal_digits_count(const uint64_t val)
{
    if (val < 10) return 1;
    if (val < 100) return 2;
    if (val < 1000) return 3;
    if (val < 10000) return 4;
    if (val < 100000) return 5;
    if (val < 1000000) return 6;
    if (val < 10000000) return 7;
    if (val < 100000000) return 8;
    if (val < 1000000000) return 9;
    if (val < 10000000000) return 10;
    if (val < 100000000000) return 11;
    if (val < 1000000000000) return 12;
    if (val < 10000000000000) return 13;
    if (val < 100000000000000) return 14;
    if (val < 1000000000000000) return 15;
    if (val < 10000000000000000) return 16;
    if (val < 100000000000000000) return 17;
    if (val < 1000000000000000000) return 18;
    if (val < 10000000000000000000U) return 19;
    return 20;
}



int lg2(unsigned int v)
{
    register unsigned int r; // result of log2(v) will go here
    register unsigned int shift;

        r = (v > 0xFFFF) << 4; v >>= r;
    shift = (v > 0xFF  ) << 3; v >>= shift; r |= shift;
    shift = (v > 0xF   ) << 2; v >>= shift; r |= shift;
    shift = (v > 0x3   ) << 1; v >>= shift; r |= shift;
                                            r |= (v >> 1);

    return r;                                            
}

unsigned int log10int_lg2shift(unsigned int v)
{
    // v ... non-zero 32-bit integer value to compute the log base 10 of 
    int r;          // result goes here
    int t;          // temporary

    static unsigned int const PowersOf10[] = 
        {1, 10, 100, 1000, 10000, 100000,
        1000000, 10000000, 100000000, 1000000000};

    t = (lg2(v) + 1) * 1233 >> 12; // (use a lg2 method from above)
    r = t - (v < PowersOf10[t]);

    return r;
}

int IntegerLogBase2(unsigned int val)
{
    return 32 - __builtin_ia32_lzcnt_u32(val);
}

uint8_t log10int_lzcnt(unsigned int v)
{
    // v ... non-zero 32-bit integer value to compute the log base 10 of 
    int r;          // result goes here
    int t;          // temporary

    static unsigned int const PowersOf10[] = 
        {1, 10, 100, 1000, 10000, 100000,
        1000000, 10000000, 100000000, 1000000000};


    t = (IntegerLogBase2(v) + 1) * 1233 >> 12; // (use a lg2 method from above)
    r = t - (v < PowersOf10[t]);

    return r;
}

uint8_t log10int_lzcnt_ull(const uint64_t v)
{
    static uint64_t const PowersOf10[] = 
        { 1
        , 10
        , 100
        , 1000
        , 10000
        , 100000
        , 1000000
        , 10000000
        , 100000000
        , 1000000000
        , 10000000000
        , 100000000000
        , 1000000000000
        , 10000000000000
        , 100000000000000
        , 1000000000000000
        , 10000000000000000
        , 100000000000000000
        , 1000000000000000000U };

    const uint32_t log2 = 64 - __builtin_ia32_lzcnt_u64(v);
    const uint32_t temp = (log2 + 1) * 1233 >> 12; // (use a lg2 method from above)
    return temp - (v < PowersOf10[temp]);
}


uint8_t get_decimal_digits_count_log10_lg2shift(const uint64_t val)
{
    if ( val == 0 ) return 1;
    return log10int_lg2shift(val) + 1;
}

uint8_t get_decimal_digits_count_log10_lzcnt(const uint64_t val)
{
    if ( val == 0 ) return 1;
    return log10int_lzcnt(val) + 1;
}
uint8_t get_decimal_digits_count_log10_lzcnt_ull(const uint64_t val)
{
    if ( val == 0 ) return 1;
    return log10int_lzcnt_ull(val) + 1;
}

uint8_t hari_mul(const uint64_t input)
{
    uint64_t cmp = 10; 
    for (int i=1; i < 20; i++) {
        
        if (input < cmp) {
            return i;
        }
        cmp = cmp * 10;
    }
    return 20;
}

// 18446744073709551615

typedef struct {
    fn_count_digits     func;
    char*               name;
} FUNC;

void run_from_to(fn_count_digits fn_digit_function, const uint64_t from, const uint64_t to, const char* name)
{
    printf("running from %20lu\n"
           "          to %20lu\n", from, to);

    uint64_t digit_count = 0;
    clock_t start_time = clock();
    uint64_t v=from;
    do
    {
        v += 1;
        digit_count += fn_digit_function(v);
    }
    while (v != to);

    double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("%15f seconds\t(%10lu)\t%s\n", elapsed_time, digit_count, name);   
}

void run_times(fn_count_digits fn_digit_function, const uint64_t val, const uint64_t times, const char* name)
{
    char str_val[21];
    sprintf(str_val, "%lu", val);

    printf("running %lu times with %ld digits (%lu)\n", times, strlen(str_val), val);

    uint64_t digit_count = 0;
    clock_t start_time = clock();
    for (uint64_t i=0; i<times;++i)
    {
        digit_count += fn_digit_function(val);
    }

    double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("%15f seconds\t(%10lu)\t%s\n", elapsed_time, digit_count, name);   
}

int main()
{
    FUNC funcs[] = {
          { .name = "naive",                    .func = get_decimal_digits_count }
        , { .name = "log10_lzcnt_ull",          .func = get_decimal_digits_count_log10_lzcnt_ull }
        , { .name = "hari_mul",                 .func = hari_mul }
        , { .name = NULL,                       .func = NULL }
    };

    //const uint64_t from = 0;
    //const uint64_t to   = 1000000000;
    //const uint64_t from = 1000000000;
    //const uint64_t to   = 1000000000 + 5000000000;
    const uint64_t range = 500000000;
    //const uint64_t to   = UINT64_MAX;
    //const uint64_t from = to - range;
    
    const uint64_t from = 0;
    const uint64_t to   = from + range;


    for (int i=0; ;++i)
    {
        FUNC* func_to_benchmark = &funcs[i];
        if ( func_to_benchmark->func == NULL )
        {
            break;
        }
        //run_from_to(func_to_benchmark->func, from, to, func_to_benchmark->name);
        run_times(func_to_benchmark->func, 9999, range, func_to_benchmark->name);
    }
    return 0;
}