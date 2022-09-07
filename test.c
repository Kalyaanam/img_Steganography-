#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    //check the operation type
    if(check_operation_type(argv)==e_encode)
    {
        EncodeInfo encInfo;
        printf("Selected encoding\n");
        if (read_and_validate_encode_args(argv,&encInfo) ==e_success)
        {
            printf("Read and Validate argument success\n");
            if (do_encoding(&encInfo)==e_success)
            {
                printf("Encoding is done successfuly\n");
            }
            else
            {
                printf("Failed Encoding the data");
            }
            
        }
        else
        {
            printf("Read and Validate argument unsuccess");
        }
        
    }
    else if(check_operation_type(argv)== e_decode)
    {
        printf("Selected decoding\n");
        DecodeInfo decInfo;
        if ( read_and_validate_decode_args(argc, argv, &decInfo) ==e_success)
        {
            printf("Read and Validate argument success\n");
            if (do_decoding(&decInfo)==e_success)
            {
                printf("Decoding is done successfuly\n");
            }
            else
            {
                printf("Failed Decoding the data\n");
            }
            
        }
        else
        {
            printf("Read and Validate argument unsuccess\n");
        }
        
    }
    
    else
    {
        printf("Invalid Option Selected\n");
        printf("Usage:\nEncoding for -e\nDecoding -d\n");
    }
    return 0;
}
    
OperationType check_operation_type(char *argv[])
{
if(strcmp(argv[1],"-e")==0)
{
    return e_encode;
}
else if(strcmp(argv[1],"-d")==0)
{
    return e_decode;
}
else
{
    return e_unsupported;
}



}