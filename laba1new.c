#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif


void convertFloatToBinary(float number, bool bits[32])
{
    unsigned char bytes[4];
    memcpy(bytes,&number,sizeof(float));

    int k = 0;

    for(int i = 3; i >= 0; i--)
    {
        unsigned char value = bytes[i];
        bool temp[8];

        for(int j = 7; j >= 0; j--)
        {
            temp[j] = value % 2;
            value /= 2;
        }

        for(int j = 0; j < 8; j++)
            bits[k++] = temp[j];
    }
}

float convertBinaryToFloat(bool bits[32])
{
    unsigned char bytes[4];
    int k = 0;

    for(int i = 3; i >= 0; i--)
    {
        unsigned char value = 0;

        for(int j = 0; j < 8; j++)
            value = value * 2 + bits[k++];

        bytes[i] = value;
    }

    float result;
    memcpy(&result,bytes,sizeof(float));

    return result;
}

void convertDoubleToBinary(double number, bool bits[64])
{
    unsigned char bytes[8];
    memcpy(bytes,&number,sizeof(double));

    int k = 0;

    for(int i = 7; i >= 0; i--)
    {
        unsigned char value = bytes[i];
        bool temp[8];

        for(int j = 7; j >= 0; j--)
        {
            temp[j] = value % 2;
            value /= 2;
        }

        for(int j = 0; j < 8; j++)
            bits[k++] = temp[j];
    }
}

double convertBinaryToDouble(bool bits[64])
{
    unsigned char bytes[8];
    int k = 0;

    for(int i = 7; i >= 0; i--)
    {
        unsigned char value = 0;

        for(int j = 0; j < 8; j++)
            value = value * 2 + bits[k++];

        bytes[i] = value;
    }

    double result;
    memcpy(&result,bytes,sizeof(double));

    return result;
}


void convertLongDoubleToBinary(long double number, bool bits[128])
{
    unsigned char bytes[16];
    memcpy(bytes,&number,sizeof(long double));

    int k = 0;

    for(int i = 9; i >= 0; i--)
    {
        unsigned char value = bytes[i];
        bool temp[8];

        for(int j = 7; j >= 0; j--)
        {
            temp[j] = value % 2;
            value /= 2;
        }

        for(int j = 0; j < 8; j++)
            bits[k++] = temp[j];
    }
}

long double convertBinaryToLongDouble(bool bits[128])
{
    unsigned char bytes[16];
    memset(bytes,0,sizeof(bytes));

    int k = 0;

    for(int i = 9; i >= 0; i--)
    {
        unsigned char value = 0;

        for(int j = 0; j < 8; j++)
            value = value * 2 + bits[k++];

        bytes[i] = value;
    }

    long double result;
    memcpy(&result,bytes,sizeof(long double));

    return result;}
    

void twistMantissaBits(bool bitArray[128], int N)
{
 

    int end = 127; 
    for (int i = end - N + 1; i <= end; i++)
    {
        bitArray[i] = !bitArray[i]; 
    }
}


void printBitsToFile(FILE *file, bool bits[], int size)
{
    for (int i = 0; i < size; i++)
        fprintf(file, "%d", bits[i]);
}

double generateNumber(double a, double b, int p)
{
    double x = a + (b - a) * rand() / RAND_MAX;
    double scale = pow(10, p);
    return round(x * scale) / scale;
}


int main()
{

    srand(time(NULL));

    int n, k, bits, p;
    double a, b;

    FILE *input = fopen("laba1.txt", "r");
    if (!input)
    {
        printf("Ошибка открытия laba1.txt\n");
        return 1;
    }

    fscanf(input, "%d", &n);
    fscanf(input, "%d", &k);
    fscanf(input, "%d", &bits);
    fscanf(input, "%lf", &a);
    fscanf(input, "%lf", &b);
    fscanf(input, "%d", &p);

    fclose(input);

#ifdef _WIN32
    _mkdir("Задания");
    _mkdir("Проверка");
#else
    mkdir("Задания", 0777);
    mkdir("Проверка", 0777);
#endif

    for (int variant = 1; variant <= n; variant++)
    {

        char fileTask[100];
        char fileCheck[100];

        sprintf(fileTask, "Задания/variant_%d.md", variant);
        sprintf(fileCheck, "Проверка/variant_%d.md", variant);

        FILE *task = fopen(fileTask, "w");
        FILE *check = fopen(fileCheck, "w");

        fprintf(task, "| № п/п | Вещественное число |\n");
        fprintf(task, "|---|---|\n");

        fprintf(check, "| № п/п | Исходное число | Машинное представление | Восстановленное число | Точность |\n");
        fprintf(check, "|---|---|---|---|---|\n");

        for (int i = 1; i <= k; i++)
        {

            double num = generateNumber(a, b, p);

            fprintf(task, "| %d | %.*f |\n", i, p, num);

            if (bits == 32)
            {
                bool bitArray[32];
                float f = (float)num;
                convertFloatToBinary(f, bitArray);
                float restored = convertBinaryToFloat(bitArray);
                double accuracy = fabs((double)f - (double)restored);

                fprintf(check, "| %d | %.*f | ", i, p, f);
                printBitsToFile(check, bitArray, 32);
                fprintf(check, " | %.*f | %.20e |\n", p, restored, accuracy);
            }

            else if (bits == 64)
            {
                bool bitArray[64];
                convertDoubleToBinary(num, bitArray);
                double restored = convertBinaryToDouble(bitArray);
                double accuracy = fabs(num - restored);

                fprintf(check, "| %d | %.*f | ", i, p, num);
                printBitsToFile(check, bitArray, 64);
                fprintf(check, " | %.*f | %.20e |\n", p, restored, accuracy);
            }

            else if (bits == 128)
            {
                bool bitArray[128];
                long double ld = (long double)num;
                convertLongDoubleToBinary(ld, bitArray);
                twistMantissaBits(bitArray, 56);
                long double restored = convertBinaryToLongDouble(bitArray);
                long double accuracy = fabsl(ld - restored);
                fprintf(check, "| %d | %.*Lf | ", i, p, ld);
                printBitsToFile(check, bitArray, 128);
                fprintf(check, " | %.*Lf | %.20Le |\n", p, restored, accuracy);
                
            }
        }

        fclose(task);
        fclose(check);
    }

    printf("Файлы успешно созданы.\n");
    return 0;
}
