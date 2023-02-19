## BMHeader structure

This structure represents the header of a custom image file with the ".BM" extension. The header is followed by the image data.

### Fields

| Field          | Type        |
|----------------|-------------|
| magic          | char[4]     |
| sizeX          | uint16_t    |
| sizeY          | uint16_t    |
| idemX          | uint16_t    |
| idemY          | uint16_t    |
| transparent    | uint8_t     |
| logSizeY       | uint8_t     |
| compressed     | uint16_t    |
| dataSize       | uint32_t    |
| pad            | uint8_t[12] |

The `magic` field is a 4-byte array that stores the characters "BM" followed by a space and the byte `0x1e`. These characters identify the file as a .BM image.

The `sizeX` and `sizeY` fields are 16-bit unsigned integers that store the width and height, respectively, of the image in pixels. If `sizeX` is `1` and `sizeY` is greater than `1`, there are multiple .BM images stored in the file.

The `idemX` and `idemY` fields are 16-bit unsigned integers that are currently not used.

The `transparent` field is a single byte that specifies the transparency of the image. If it is `0x36`, the image is normal (not transparent). If it is 0x3e, the image is transparent. If it is 0x08, the image is a weapon image.

The `logSizeY` field is a single byte that stores the logarithm (base 2) of the value of `sizeY`. If the image is a weapon image, this field should be set to 0.

The `compressed` field is a 16-bit unsigned integer that specifies the type of compression used on the image data. If it is 0, the image data is not compressed. If it is 1, the image data is RLE1 compressed. If it is 2, the image data is LRE0 compressed.

The `dataSize` field is a 32-bit unsigned integer that stores the length of the image data, in bytes, after the header. If the `dataSize` is 0 and the compression is set to `0`, the dataSize should be calculated from `sizeX` and `sizeY`

The `pad` field is a 12-byte array of `0x00` bytes that is currently not used and should be left as is.

Multiple BMs
=============
Multiple BMs are represented by having the sizeX set to 1, and sizeY set to anything buy 1.
After the header, there are 2 bytes. the 1st tells us the frequency of the change between the next images, the 2nd is always set to 2. If the frequency is 0, it means that the image is related to a switchable component (button, switch, handle etc.).
Following the frequency the file contains several 32bit offset values regarding where the next subheader can be found. These offsets will use the byte after the frequency as their reference point, whic is the 35th byte (BMHeader + frequency + `0x02`= 32 + 1 + 1 bytes = 34 bytes).
