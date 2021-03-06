#ifndef _FileSystem_H
#define _FileSystem_H

#include "Main.H"

//------- Public Constants  -----------------------------------------
#define Disk_Error (0xF0)
#define No_Disk_Error (0)
#define more_entries (0x8000)   
#define no_entry_found (0x80000000)  // msb set to indicate error
#define directory_bit  (0x10000000)  // lsb of first nibble (bit28)

#define MBR_RelSectors (0x01C6)
#define BPB_BytesPerSector (0x0B)
#define BPB_SectorsPerCluster (0x0D)
#define BPB_RsvdSectorCount (0x0E)
#define BPB_NumberFATs (0x10)
#define BPB_RootEntryCount (0x11)
#define BPB_TotalSectors16 (0x13)
#define BPB_FATsize16 (0x16)
#define BPB_TotalSectors32 (0x20)
#define BPB_FATsize32 (0x24)
#define BPB_RootCluster (0x2C)


// ------ Public function prototypes -------------------------------
uint8 read8(uint16 offset, uint8 * array_name);

uint16 read16(uint16 offset, uint8 * array_name);

uint32 read32(uint16 offset, uint8 * array_name);

uint8 Mount_Drive(uint8 xdata * array_name);

uint32 First_Sector (uint32 Cluster_num);

uint32 Find_Next_Clus(uint32 Cluster_num, uint8 xdata * array_name);

//uint8 Open_File(uint32 Cluster, uint8 xdata * array_in);


#endif