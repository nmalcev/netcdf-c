# About

Utility for reading data from netcdf files.

Here's an example of how to use the app:
```bash
source start.sh
```

## Additional packages:

- Dependencies for 32-bit systems (`sudo apt-get install gcc-multilib`)
- To be able to import a netcdf.h file (`sudo apt-get install libnetcdf-dev`)
- The ncdump command-line utility converts netCDF data to human-readable text form.

### Ncdump

```bash
sudo apt-get install netcdf-bin
ncdump -h GlobColour/L3b_20181001__GLOB_4_AV-MOD_NRRS555_DAY_00.nc 
ncdump -v GlobColour/L3b_20181001__GLOB_4_AV-MOD_NRRS555_DAY_00.nc
```

## Data samples

To run the code, you will need sample data (the globcolour dataset), which can be downloaded from this link https://hermes.acri.fr/index.php?class=archive .
