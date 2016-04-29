//#pragma comment (lib, "lzo.lib") /* link with OpenGL Utility lib */

#include <windows.h>		// Header File For Windows
#include <stdio.h>

//BOOL uncompress_x(char *filename, char *filename2);
//BOOL uncompress_y(char *filename, char *filename2);

#include "compression.h"

struct zfs_header
{
	char	branding[4];
	int		unknown1;
	int		unknown2;
	int		diramount;
	int		totalfiles;
	int		unknown3;
	int		unknown4;
	int		unknown5;
};

struct zfs_fileinfo
{
	char	name[15];
	int		unknown1;
	int		unknown2;
	int		size;
	int		unknown3;

	byte	compression;
	byte	compression_reallength[2];
	byte	unknown;
};	

int main(int argc, char **argv)
{
	FILE*		fPtr;
	zfs_header	zfsheader;
	zfs_fileinfo	zfstemp;
	zfs_fileinfo*	zfsfiles;
	
	//If we're null or /? then show help
	if (argv[1] == NULL || argv[2] == NULL || !strcmp(argv[1], "/?") || !strcmp(argv[1], "?"))
	{
		printf("Performs actions on a ZFS file.\n\n");
		printf("UNZFS [command] path\n\n");
		printf("command is one of the following:\n");
		printf("	-u		Extract and uncompress all files to current directory.\n");
		printf("	-e		Extract all to current directory without uncompressing.\n");
		printf("	-l		List all files in the archive.\n");
		printf("	-i		Display information on the archive.\n\n");
		printf("UNZFS v0.1a by Dummy (c) 2003 Paradum Games\nhttp://games.paradum.com\n");
		return TRUE;
	}
		
	//Open the file
	fPtr = fopen(argv[2], "rb");
	if (!fPtr)
	{
		printf("Could not open file %s\n", argv[2]);
		return FALSE;
	}

	//Read and check the header
	fread(&zfsheader, sizeof(zfsheader), 1, fPtr);
	if (memcmp(zfsheader.branding, "ZFSF",4))
	{
		printf("This is not a valid Activision ZFS file.\n");
		fclose(fPtr);
		return FALSE;
	}

	//Show the user the information
	if (!strcmp(argv[1], "-i") || !strcmp(argv[1], "-e") || !strcmp(argv[1], "-u"))
	printf("Archive: %s\nTotal Files:   %i\n\nDirectory Amount:   %i\n\n", argv[2], zfsheader.totalfiles, zfsheader.diramount);
	
	//If we're just getting the info, exit now
	if (!strcmp(argv[1], "-i"))
	{
		fclose(fPtr);
		return TRUE;
	}

	//Allocate the memory
	zfsfiles = new zfs_fileinfo[zfsheader.totalfiles];

	//Now we read in the file list data
	while (zfsheader.totalfiles > 0)
	{

		for (int i = 0; i < zfsheader.diramount; i++)
		{
			fread(&zfstemp, sizeof(zfstemp), 1, fPtr);
			if (i < zfsheader.totalfiles)
			{
				char isCompressed[10] = "Unknown";
				zfsfiles[i] = zfstemp;

				if (zfsfiles[i].compression == 0)
					sprintf(isCompressed, "None");
				if (zfsfiles[i].compression == 2)
					sprintf(isCompressed, "LZO1X");
				if (zfsfiles[i].compression == 4)
					sprintf(isCompressed, "LZO1Y");

				printf ("File: %4i    Name: %15s    Size: %8i    Compression: %5s\n", zfstemp.unknown2+1, zfstemp.name, zfstemp.size, isCompressed);
			}
		}
		
		//Now we read in the actual files and save to a new location
		byte*	thefile;
		for (int i = 0; i < zfsheader.diramount; i++)
		{
			if (i < zfsheader.totalfiles)
			{
				FILE*	fWrite;

				//Allocate temp mem
				thefile = new byte[zfsfiles[i].size];

				//Read in the file bytes into a buffer
				fread(thefile, zfsfiles[i].size, 1, fPtr);

				//If we're extracting, extract
				if (!strcmp(argv[1], "-e") || !strcmp(argv[1], "-u"))
				{
					fWrite = fopen(zfsfiles[i].name, "wb");
					fwrite(thefile, zfsfiles[i].size, 1, fWrite);
					fclose(fWrite);
				}

				//If we're uncompressing and it's compressed we uncompress it
				if (!strcmp(argv[1], "-u"))
				{
					if (zfsfiles[i].compression == 2)
						uncompress_x(zfsfiles[i].name, zfsfiles[i].name);
					if (zfsfiles[i].compression == 4)
						uncompress_y(zfsfiles[i].name, zfsfiles[i].name);
				}

				//Unallocate temp mem
				delete thefile;
			}
		}

		int tmp;
		fread(&tmp, 4, 1, fPtr);

		zfsheader.totalfiles -= 100;

	}

	//Deallocate the memory
	delete zfsfiles;

	//Say a bye bye
	printf("Process completed successfully.\n\n");
	printf("UNZFS v0.1a by Dummy (c) 2003 Paradum Games\nhttp://games.paradum.com\n");
	return TRUE;

	//Close and return
	fclose(fPtr);
	return TRUE;
}