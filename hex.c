/*
** Hexadecimal conversion
** version 1.0
** "works good enough edition"
**
**
** 1-11-2019
**
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <getopt.h>

#define MAX_INPUT 18		// in nibbles
#define MAX_SIZE  16		// in nibbles
#define NIBBLE	   4

// Protatoz
int help (void);
int filterHex(char *hexVal, char *rawVal);
int hexToBin(char *rawVal);
int hexToOct(char *rawVal);
long int hexToDec(char *rawVal);

// hex to nibble lookup table
static const char nibble[][5] =
{
     ['0'] =  "0000",
     ['1'] =  "0001",
     ['2'] =  "0010",
     ['3'] =  "0011",
     ['4'] =  "0100",
     ['5'] =  "0101",
     ['6'] =  "0110",
     ['7'] =  "0111",
     ['8'] =  "1000",
     ['9'] =  "1001",
     ['A'] =  "1010",
     ['B'] =  "1011",
     ['C'] =  "1100",
     ['D'] =  "1101",
     ['E'] =  "1110",
     ['F'] =  "1111",

};

// hex to decimal lookup table
static const int decimals[] =
{
     ['0'] = 0,
     ['1'] = 1,
     ['2'] = 2,
     ['3'] = 3,
     ['4'] = 4,
     ['5'] = 5,
     ['6'] = 6,
     ['7'] = 7,
     ['8'] = 8,
     ['9'] = 9,
     ['A'] = 10,
     ['B'] = 11,
     ['C'] = 12,
     ['D'] = 13,
     ['E'] = 14,
     ['F'] = 15

};


//
 
int main (int argc, char **argv)
{
    
    
    int16_t c;					// for getopt()
    int bin = 0;
    int dec = 0;
    int oct = 0;

    char *usage =  "Usage: hex -[[o][b][d]] <0x123abCD>"\
                   "Or hex --help for instructions\n\n";
                   
    static struct option long_options[] =
    {
         { "help",   no_argument, 0, 1 },
         { "binary", no_argument, 0, 2 },
         { "octal",  no_argument, 0, 3 },  
         { "decimal",no_argument, 0, 4 },
         {  NULL,    0,        NULL, 0 },
    };
    
    while ((c = getopt_long(argc, argv, "bdoh", long_options, NULL)) != -1)
    {
        switch (c)
        {
        
            // help
            case 'h':
            case  1 :
            {
                help();
                return 0;
                break;
            }
            
            // binary
            case 'b':
            case  2 :
            {
                bin = 1;
                break;
            }
            
            // octal
            case 'o':
            case  3 :
            {
                oct = 1;
                break;
            }
            
            case 'd':
            case  4 :
            {
                dec = 1;
                break;
            }
            
            
            case '?':
            {
                fprintf (stderr, "unknown option\n");
                exit(1);
            }
            
            default:
            {
                fprintf (stderr, "Mandatory option missed\n");
                exit(1);
            }
          }
                  
    } // end while
    
    // if user didn't give any options, kill the program
    if (oct == 0 && dec == 0 && bin == 0)
    {
         fprintf (stderr, "%s\n", usage);
         return 1;
    }
         
    // Storage for hexadecimal input value
    char *hexVal = calloc(MAX_INPUT + 1, sizeof(char) );
    
    // Handle non option arguments
    if (optind == argc)
    {
          printf ("You need to enter a hex value dood.. ");
          return 2;
    }

    // There is a non option argument
    if (optind < argc)    
    {
    
          
         
         // copy into hexval 
         if (strlen(argv[optind]) > 0 && strlen(argv[optind]) <= MAX_INPUT)
         {
              strcpy(hexVal, argv[optind]);
         }
         
         else
         {
            fprintf (stderr, "Enter a 64-bit maximum hexadecimal value\n");
            return 3;
         } 
         
         // Make sure there were no more args
         if (argv[++optind] != NULL)
         {
              printf ("Unknown argument %s\n", argv[optind]);
              return 4;
         }
         
//         printf ("debug: hexVal contains %s\n\n", hexVal);
    }
    
   char *rawVal  = calloc(MAX_SIZE+1, sizeof(char) );  // hold final hex value 
   int8_t size = 0;	// will hold the final string length
   if ( (size = filterHex(hexVal, rawVal)) < 0 )
   {
        // filterHex returned an error, can the program.
        return 5;
   }
   
   if (bin == 1)
        hexToBin(rawVal);
        
   if (oct == 1)
        hexToOct(rawVal);
     
   if (dec == 1)
        printf ("%ld\n", hexToDec(rawVal));
    
   free(hexVal);
   return 0;
    
        
} 
    
// H E L P ///////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////    
int help (void)
{
    printf ("\n\n     Hexadecmial Conversion     	\n\n"\
            "--------------------------------		\n"\
            "   OPTIONS 			     	\n"\
            "-b   | --binary    Convert to binary    	\n"\
            "-d   | --decimal   Convert to decimal   	\n"\
            "-o   | --octal     Convert to octal(TODO)	\n"\
            "\nex: hex -b 0x123faCE			\n\n");
            
            return 1;
           
} 

// F I L T E R ///////////////////////////////////////////////////////////
/* Remove any trailing characters (x, 0x, 0X) from the
** users input and update the *rawVal
*/
//////////////////////////////////////////////////////////////////////////
int filterHex(char *hexVal, char *rawVal)
{
     /* Iterate through the input array. If either of the first two characters ar
    ** an x or X, restart the counter. 16 max hex digits allowed without an x,
    ** or following an x. Stop program if an x appears after the second value,
    ** program expects x, X, 0x, or 0X.
    */
    uint8_t count     = 0;                              // count of characters e
    uint8_t leadChar  = 0;                              // x, 0x
    uint8_t i	      = 0;
    uint8_t j	      = 0;
    uint8_t length    = 0;
    
    
    
    length = strlen(hexVal);
    
    for (i = 0, j = 0; i < length; ++i)
    {

        // if user entered x or 0x
        if (i < 2 && (hexVal[i] == 'x' || hexVal[i] == 'X'))
        {
            leadChar = i+1;                             // how many characters t
            count = j = 0;                              // set j back to zero to
            continue;
        }

        // verify input    
        else 
        if   (!isdigit(hexVal[i])  &&                          // if not a digi
             ( (hexVal[i] < 'a' || hexVal[i] > 'f') &&        // and not lower
                 (hexVal[i] < 'A' || hexVal[i] > 'F') ) )     // and not upper
            {
                printf ("Time for a new keeb, bro\n");
                return -1;
            }

        // if     
        else
        {
            rawVal[j] = toupper(hexVal[j+leadChar]);
            ++j;
        }
        ++count; 

     }
     
    // Final error check, make sure input value does not exceed 64-bit
    if (count > MAX_SIZE  )      // count is off by 1 here due to starting at 0. 
    {
        printf ("Nope, you're not going to sink this ship\n\n");
        return 4;
    }


    //printf ("rawVal now contains %s\n", rawVal);
    return count;
}

// HEXADECIMAL TO BINARY ////////////////////////////////////////////
/* Print the value as binary using lookup table 
**
**
*/
///////////////////////////////////////////////////////////////////////
int hexToBin(char *rawVal)
{

    uint8_t i = 0;
//    printf ("hexToBin() received value %s\n", rawVal);
    
    // Print the result
    for (i = 0; i <= strlen(rawVal); ++i) 
    {

        printf ("%s ", nibble[(unsigned char)rawVal[i] ]);

    }

    putchar('\n'); 

    
    return 1;
}

/************************************************************************
* To convert to decimal, start from the least significant 
* hexadecimal value and multiply it by it's positional exponent
* iterate the exponent += 16
*************************************************************************/
long int hexToDec(char *rawVal)
{
    int pos = (strlen(rawVal) - 1);	// -1 because 0 is 1st character.
    int temp = 0;
    long int decimal = 0;
    long int power   = 1;
    
  //  printf ("debug: pos = %d\n", pos);
  //  printf ("debug: rawVal[pos] = %c\n", rawVal[pos]);   
    
    while (pos >= 0)
    {
        // printf ("debug: temp = %d\n", temp);
         temp = decimals[(unsigned char)rawVal[pos]];
        // printf ("debug: temp is now %d\n", temp);
         decimal += (temp * power);
         power *= 16;
         --pos;
    }
    
    // Print final value (PRINTING FROM MAIN INSTEAD)
    //printf ("%ld\n", decimal);
            
    return decimal;
}

int hexToOct(char *rawVal)
{
    // For ease, convert the hex value to decimal
    long int decimal  = hexToDec(rawVal);
    char * result = calloc(21, sizeof(char));
    int i = 0;
//    printf ("decimal equivalent: %ld\n", decimal);

    /* Algorthm is the same as decimal:
    ** Divide by 2, remainder becomes rightmost digit
    ** quotient becomes new value. Loop until 0 and then
    ** print result*/
    while (decimal != 0)
    {
//        printf ("debug: decimal mod 8 = %li\n", (decimal % 8));
        result[i] = ( decimal % 8 );
//        printf ("result now contains %d\n", result[i]);
        ++i;
        decimal /= 8;
    } 
    
    for (--i; i >= 0; --i)
    {
      
      printf ("%d", result[i]);
    }
      
    printf ("\n");    
      
    return 1;
} 

