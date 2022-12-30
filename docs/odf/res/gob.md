GOB File Documentation
=======================
Endianness: Little Endian

File structure overview
-----------------------
- GobArchiveHeaders
- - magic
- - directoryOffset
- List of GobFile
- Directory
- - fileCount
- - List of GobFileHeader
- - - offset
- - - length
- - - name

GobArchiveHeaders Fields
--------------------------
- magic (4 bytes): 'GOB' followed by 0x0A
- directoryOffset (uint32): Absolute pointer to the Directory

Directory Fields
----------------
- fileCount (uint32)
- List of GobFileHeader (fileCount*sizeof(GobFileHeader))

GobFileHeader Fields
--------------------
- offset (uint32): Absolute pointer to the beginning of the archived file
- length (uint32): Length of the file
- name (13 bytes): Name of the file with extension

GobFile
-------
The content of the file itself
