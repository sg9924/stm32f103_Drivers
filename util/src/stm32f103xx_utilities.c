#include"stm32f103xx_utilities.h"
#include"stm32f103xx_serial.h"
#include<stdarg.h>

//char temp_buff[33];
char temp_buff[64];
char result_buff[64];

static uint32_t bi;
static char str_buffer[1024];


void wait_ms(uint16_t d)
{
	for(int i=0; i<(d*350); i++);
}

void wait(uint8_t d)
{
	wait_ms(d*1000);
}

uint32_t string_size(char* data)
{
    uint32_t c=0;
    while(*data++ != '\0')
        c++;
    return c;
}

char* string_truncate(char* pbuff, uint8_t length, uint8_t no_of_chars, uint8_t direction)
{
    if(direction == TRUNCATE_RIGHT)
        pbuff[length - 1 - no_of_chars + 1] = '\0';
    
    return pbuff;
}

char* string_concat(char* s1, char* s2)
{
    char* temp = s1;
    //traverse to end of s1
    temp = end_of_string(s1);
    temp--;
    
    while(*s2 != '\0')
    {
        *(temp++) = *(s2++);
    }
    *temp='\0';

    return s1;
}

char* string_trim(char* pbuff, uint8_t direction, char character, uint32_t length)
{
    if(direction == TRIM_RIGHT)
    {
        uint8_t i = length;
        while(pbuff[--i] == character);
        pbuff[++i] = '\0';
    }
    else if(direction == TRIM_LEFT)
    {
        uint8_t i = 0;
        while(pbuff[i++] != character);
        pbuff[i] = '\0';
    }
    return pbuff;
}








char* array_reverse(char* pbuff, uint8_t length)
{
    uint8_t i=0, j=length-1;
    char temp;

    while(i<=j)
    {
        temp = pbuff[i];
        pbuff[i] = pbuff[j];
        pbuff[j] = temp;
        i++;
        j--;
    }
    return pbuff;
}


char* string_reverse(char* pbuff, uint8_t length)
{
    return (array_reverse(pbuff, length-1)); //send string excluding '/0' character
}

char* end_of_string(char* str)
{
    char* ptr = str;

    while(*(ptr++) != '\0');
    
    return ptr;
}


char* string_copy(char* dest, char* src)
{
    char* ptr = dest;

    while(*src != '\0')
    {
        *ptr = *src;
        ptr++;
        src++;
    }
    *ptr = '\0';

    return dest;
}



int32_t string_to_int(char* str)
{
    int8_t sign = 1;
    int32_t result = 0;

    if(*str == '-')
    {
        sign = -1;
        str++;
    }

    for(;*str != '\0'; str++)
    {
        if(*str >= '0' && *str <= '9')
            result = (result*10) + (*str - '0');
        else
            return 1;
    }

    return (sign * result);
}



uint32_t power(uint8_t num, uint8_t power)
{
    uint32_t result=1;
    while(power!=0)
    {
        result = result*num;
        power--;
    }

    return result;
}



uint8_t no_of_digits(uint32_t num)
{
    uint8_t i=0;

    while(num!=0)
    {
        temp_buff[i] = '0' + (num%10);
        num=num/10;
        i++;
    }

    return i;
}