#include<stdio.h>
#include "decode.h"
#include "types.h"
#include<string.h>

/* Function Definiation */


Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
	if (argc > 2)
	{
		if (strcmp(strstr(argv[2],"."), ".bmp") == 0)
        {

			decInfo->stego_image_fname = argv[2];
            printf("Read and validate was successfull...");
        }
        else
        {
           printf("Failed to Read and validate the arguments");
            return e_failure;
        }
	}
	else
	{
		printf("Arguments are missing");
		return e_failure;
	}

	if (argc > 3)
	{
		strncpy(decInfo->extn_output_file, strstr(argv[3], "."), 4);
        if ((strncmp(decInfo->extn_output_file, ".txt", 4) == 0) || (strncmp(decInfo->extn_output_file, ".c", 2) == 0) || (strncmp(decInfo->extn_output_file, ".sh", 3) == 0))
        {
				decInfo->output_file_name = argv[3];
                printf("Copied successfully...");
        }
        else
        {
      		printf("Failed to Copy...");
            return e_failure;
        }
	}
	else
	{
		decInfo->output_file_name = NULL;
	}
	
	/* Passcode validation*/
		if (argc > 4)
		{
			if (strcmp(argv[4], "-p") == 0)
			{
				if (argc > 5)
				{
					decInfo->passcode = argv[5];
					decInfo->passcode_size = strlen(decInfo->passcode);
				}
				else
				{
					printf("No passcode found\n");
					return e_failure;
				}
			}
			else
			{
				fprintf(stderr,"Error: Invalid option\n");
				return e_failure;
			}
		}
		else
		{
			decInfo->passcode = NULL;
			decInfo->passcode_size = 1;
		}
	return e_success;
}
Status open_decode_files(DecodeInfo *decInfo)
{
	static int open = 0;
	if (open == 0)
	{
		open++;
		// Open Stego Image file
		decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");

		// Error handling
		if (decInfo->fptr_stego_image == NULL)
		{
			perror("fopen");
			printf("Error: Unable to open file\n");
			return e_failure;
		}
		else
		{
			printf("Successfully Opened the stego image file\n");
		}
	}
	else 
	{
		// Open Output file
		decInfo->fptr_output_file = fopen(decInfo->output_file_name, "w");
		// Error handling
		if (decInfo->fptr_output_file == NULL)
		{
			perror("fopen");
			printf("Error: Unable to open file\n");
			return e_failure;
		}
		else
		{
			printf("Successfully opened the output file...\n");
		}
	}
	return e_success;
}
Status do_decoding(DecodeInfo *decInfo)
{
	printf("INFO: ## Decoding Started ##\n");
	
	// Open the required files
	if (open_decode_files(decInfo) == e_success)
	{
		printf("Succesfully opened all the required files... \n");
		uint raster_data;
		fseek(decInfo->fptr_stego_image, 10L, SEEK_SET);
		fread(&raster_data, sizeof(int), 1, decInfo->fptr_stego_image);
		fseek(decInfo->fptr_stego_image, raster_data, SEEK_SET);
		printf("Decoding Magic String...\n");
		
		// Decoding magic string signature
		if (decode_magic_string(decInfo->passcode, decInfo) == e_success)
		{
			printf("Decoding magic string was successfully done...\n");
			
			// Decoding Output File Extenstion Size
			if (decode_output_file_extn_size(decInfo) == e_success)
			{
				printf("Decoding output file extension was successfully done... \n");

				// Check for output file provided or not
				if (decInfo->output_file_name == NULL)
				{
					
					if (strncmp(decInfo->extn_output_file, ".txt", 4) == 0)
					{
						decInfo->output_file_name = "decoded.txt";
						printf("The output file is not provided. Creating %s as default\n", "decoded.txt");
					}
					else if (strncmp(decInfo->extn_output_file, ".sh", 3) == 0)
					{
	 					decInfo->output_file_name = "decoded.sh";
						printf("The output file not mentioned. Creating %s as default\n", "decoded.sh");
					}
					else
					{
						decInfo->output_file_name = "decoded.c";
						printf("The output file not mentioned...");
					}
					if (open_decode_files(decInfo) == e_success)
					{
						printf("The decode file are opened successfully...");
					}
					else
					{
						printf("Failed toopen decode files");
						return e_failure;
					}
				}
				else
				{
					printf("The output file is mentioned\n");
					if (open_decode_files(decInfo) == e_success)
					{
						printf("The decode files are Opened \n");
					}
					else
					{
						printf("Failed to open decode files...\n");
						return e_failure;
					}
				}
				printf("Opened all required files\n");

				// Decoding File Size
				if (decode_file_size(decInfo) == e_success)
				{
					printf("Decoding File Size was done\n");

					// Decoding File Data
					if (decode_data_to_file(decInfo) == e_failure)
					{
						printf("Failed to Decoding File Data\n");
						return e_failure;
					}
                    else
                    {
                        printf("Decoding File Data done\n");
                        return e_success;
                    }
				}
				else
				{
					printf("Failed to decoding file size");
					return e_failure;
				}
			}
			else
			{
				printf("Failed to decode output file extension size");
				return e_failure;
			}
		}
		else
		{
			printf("Failed to decode magic string");
			return e_failure;
		}
	}
	else
	{
		printf("Failed to open decode files");
		return e_failure;
	}
	printf("Done.....\n");
	return e_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
	uint i;
	if (magic_string != NULL)
	{
		for (i = 0; i < decInfo->passcode_size; i++)
		{
			fread(decInfo->image_data, sizeof(char), 8, decInfo->fptr_stego_image);
			if (ferror(decInfo->fptr_stego_image))
    		{
        		printf("Failed reading the data from stego image file\n");
         		clearerr(decInfo->fptr_stego_image);
         		return e_failure;
    		}
			if (decode_lsb_to_byte(decInfo->decode_data, decInfo->image_data) == e_success)
			{
				if (decInfo->decode_data[0] == magic_string[i])
				{
					continue;
				}
				else
				{
					printf("Incorrect passcode\n");
					return e_failure;
				}
			}
			else
			{
				printf("failed to decode lsb to byte");
				return e_failure;
			}
		}
	}
	fread(decInfo->image_data, sizeof(char), 8, decInfo->fptr_stego_image);
	if (ferror(decInfo->fptr_stego_image))
    {
        printf("Failed reading the data from stego image file\n");
        clearerr(decInfo->fptr_stego_image);
        return e_failure;
    }
	if (decode_lsb_to_byte(decInfo->decode_data, decInfo->image_data) == e_success)
	{
		if (decInfo->decode_data[0] == '*')
		{
				return e_success;
		}
		else
		{
				fprintf(stderr,"Incorrect passcode\n");
				return e_failure;
		}
	}
	else
	{
		printf("failed to decode lsb to byte");
		return e_failure;
	}
}

Status decode_output_file_extn_size(DecodeInfo *decInfo)
{
	char file_size[DECODE_FILE_EXTN_SIZE];
	fread(file_size, sizeof(char), DECODE_FILE_EXTN_SIZE, decInfo->fptr_stego_image);
	if (ferror(decInfo->fptr_stego_image))
    {
         printf("Failed to reading the data from stego image file\n");
         clearerr(decInfo->fptr_stego_image);
         return e_failure;
    }

	decInfo->image_data_size = 0;
	for (uint i = 0; i < DECODE_FILE_EXTN_SIZE; i++)
	{
		decInfo->image_data_size <<= 1;
		decInfo->image_data_size |= (uint) (file_size[i] & 0x01);
	}
	if (decode_output_file_extn(decInfo->image_data_size, decInfo) == e_failure)
	{
		printf("failed to decode output file extn ");
		return e_failure;
	}
	return e_success;
}

Status decode_output_file_extn(uint extn_size, DecodeInfo *decInfo)
{
	for (uint i = 0; i < extn_size; i++)
	{
		fread(decInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, decInfo->fptr_stego_image);
		if (ferror(decInfo->fptr_stego_image))
    	{
        	printf("Error: reading the data from stego image file\n");
         	clearerr(decInfo->fptr_stego_image);
         	return e_failure;
    	}
        else
        {
            printf("decode output file extension was done successfully...\n");
        }
        
		if (decode_lsb_to_byte(decInfo->decode_data, decInfo->image_data) == e_success)
		{
			decInfo->extn_output_file[i] = decInfo->decode_data[0];
		}
	}
	return e_success;
}

Status decode_file_size(DecodeInfo *decInfo)
{
	char file_size[DECODE_FILE_SIZE];
	fread(file_size, sizeof(char), DECODE_FILE_SIZE, decInfo->fptr_stego_image);
	if (ferror(decInfo->fptr_stego_image))
    {
         fprintf(stderr,"Error:reading the data from stego image file\n");
         clearerr(decInfo->fptr_stego_image);
         return e_failure;
    }
	decInfo->image_data_size = 0;
	for (uint i = 0; i < DECODE_FILE_SIZE; i++)
	{
		decInfo->image_data_size <<= 1;
		decInfo->image_data_size |= (uint) (file_size[i] & 0x01);
	}
	return e_success;
}
Status decode_data_to_file(DecodeInfo *decInfo)
{
	for (uint i = 0; i < decInfo->image_data_size; i++)
	{
		fread(decInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, decInfo->fptr_stego_image);
		if (ferror(decInfo->fptr_stego_image))
    	{
        	 fprintf(stderr,"Error: While reading the data from stego image file\n");
         	clearerr(decInfo->fptr_stego_image);
         	return e_failure;
    	}
		if(decode_lsb_to_byte(decInfo->decode_data, decInfo->image_data) == e_success)
		{
			fwrite(decInfo->decode_data, sizeof(char), 1, decInfo->fptr_output_file);
			if (ferror(decInfo->fptr_stego_image))
    		{
        		printf("Error:reading the data from stego image file\n");
         		clearerr(decInfo->fptr_stego_image);
         		return e_failure;
    		}

		}
		else
		{
			printf("Failed to decode lsb to byte\n");
			return e_failure;
		}
	}
	return e_success;
}
Status decode_lsb_to_byte(char *decode_data, char *image_data)
{
	decode_data[0] = 0;
	for (uint i = 0; i < MAX_IMAGE_BUF_SIZE; i++)
	{
		decode_data[0] <<= 1;
		decode_data[0] |=  (image_data[i] & 0x01);
	}
	return e_success;
}