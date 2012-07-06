#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

//==============================================================================
// Usage:
//
// bmextract x y w h
//		where x = x offset
//					y = y offset
//					w = width
//					h = height
//		if any/all ommitted, defaults for bitmap used
//==============================================================================

void DumpArgs(int argc, char* argv[])
{
	printf("*** Passed %d arguments:\n", argc);
	for (int i = 0; i < argc; ++i)
	{
		printf("[%d] [%s]\n", i, argv[i]);
	}
	printf("*** End argument list\n");
}

//==============================================================================

bool Read32(FILE* pFile, uint32_t* pValue)
{
	uint8_t buffer[4];
	bool success = false;

	if (fread(buffer, sizeof(buffer), 1, pFile))
	{
		success = true;
		if (pValue != NULL)
		{
			*pValue = (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | (buffer[0]);
		}
	}

	return success;
}

//==============================================================================

bool Read16(FILE* pFile, uint16_t* pValue)
{
	uint8_t buffer[2];
	bool success = false;

	if (fread(buffer, sizeof(buffer), 1, pFile))
	{
		success = true;
		if (pValue != NULL)
		{
			*pValue = (buffer[1] << 8) | (buffer[0]);
		}
	}

	return success;
}

//==============================================================================

bool Read8(FILE* pFile, uint8_t* pValue)
{
	uint8_t buffer[1];
	bool success = false;

	if (fread(buffer, sizeof(buffer), 1, pFile))
	{
		success = true;
		if (pValue != NULL)
		{
			*pValue = (buffer[0]);
		}
	}

	return success;
}

//==============================================================================

int main(int argc, char* argv[])
{
//	DumpArgs(argc, argv);

	if (argc != 0)
	{
		FILE* pFile = fopen(argv[1], "rb");

		if (pFile)
		{
			uint16_t type = 0;
			Read16(pFile, &type);

			if (type == 0x4D42)
			{
				printf("[%s]:\n", argv[1]);
				uint32_t length = 0;
				Read32(pFile, &length);
				Read32(pFile, NULL); // skip reserved 1 and 2
				uint32_t pixelOffset = 0;
				Read32(pFile, &pixelOffset);
				uint32_t dibHeaderSize = 0;
				Read32(pFile, &dibHeaderSize);
				uint32_t width = 0, height = 0;
				Read32(pFile, &width);
				Read32(pFile, &height);
				printf("%dx%d pixels\n", width, height);
				uint16_t planes = 0, bitsPerPixel = 0;
				Read16(pFile, &planes);
				Read16(pFile, &bitsPerPixel);
				printf("%d bits per pixel\n", bitsPerPixel);
				uint32_t compressType, imageSize, xppm, yppm;
				Read32(pFile, &compressType);
				Read32(pFile, &imageSize);
				Read32(pFile, &xppm);
				Read32(pFile, &yppm);

				uint32_t numColours = 0, numImportantColours = 0;
				Read32(pFile, &numColours);
				Read32(pFile, &numImportantColours);
				
				uint32_t windowx = 0;
				uint32_t windowy = 0;
				if (argc >= 4)
				{
					windowx = atoi(argv[2]);
					windowy = atoi(argv[3]);
				}
				uint32_t windoww = width;
				uint32_t windowh = height;
				if (argc >= 6)
				{
					windoww = atoi(argv[4]);
					windowh = atoi(argv[5]);
				}

				uint32_t maxBytes = windoww;
				uint32_t bytesWritten = 0;
				uint32_t bitsRead = 0;
				uint8_t mask = 0xff >> (8-bitsPerPixel);
				uint8_t pixel = 0;
				uint8_t bits = 0;

				printf("GFX:\n");
				for (uint32_t y = 0; y <= height ; ++y)
				{
					fseek(pFile, pixelOffset+((height-y-1)*((width*bitsPerPixel)/8)), SEEK_SET);

					for (uint32_t x = 0; x < ((width*bitsPerPixel)/8); ++x)
					{
						Read8(pFile, &bits);
						bitsRead +=8;

						if ((y >= windowy) && (y < (windowy+windowh)))
						{
							if ((x >= windowx) && (x < (windowx+windoww)))
							{
								while (bitsRead >= bitsPerPixel)
								{
									pixel = bits&mask;
									bits >>= bitsPerPixel;
									bitsRead -= bitsPerPixel;

									if (bytesWritten == 0)
									{
										printf("	DB ");
									}

									printf("#%02X%s", pixel+1, (bytesWritten < (maxBytes-1)) ? ", " : "\n");
									bytesWritten += 1;

									if (bytesWritten == maxBytes)
									{
										bytesWritten = 0;
									}
								}
							}
						}
					}
				}

				printf("PALETTE:\n");
				fseek(pFile, dibHeaderSize+14, SEEK_SET);
				uint8_t colour[4];
				for (uint32_t index = 0; index < numColours; ++index)
				{
					fread(colour, sizeof(colour), 1, pFile);
					printf("  DW #FF%02X, #%02X%02X\n", colour[2], colour[1], colour[0]);
				}
			}
			else
			{
				printf("[%s] not a bitmap (type %04X)\n", argv[1], type);
			}

			fclose(pFile);
		}
	}
	else
	{
		printf("Some instructions or help would be nice here.\n");
	}

//	printf("All done.\n");

	return 0;
}

//==============================================================================

