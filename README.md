# CubeToEqi

Creates a modern HDR equirectangular image from 6 VTF HDR cubemap faces (up, lf, ft, rt, bk, dn).

![Before and after example](https://i.imgur.com/d1xSpmY.png)

Uses [VTFLib](https://github.com/NeilJed/VTFLib) for reading VTF files and [DevIL](https://github.com/DentonW/DevIL) to create HDR image.

Projection conversion code is a port of [CubemapToEquirectangular](https://github.com/Mapiarz/CubemapToEquirectangular) for Unity.

## Usage

Start program and give folder containing all 6 faces, or pass path as launch arg.

### Arguments:
``xoffset, yoffset`` (0 - 1): Offset the final HDR, useful if you want to rotate the hdr but cannot in some software.
