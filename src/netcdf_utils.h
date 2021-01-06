#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include <stdio.h>

struct varAttr{
  char* name;
  nc_type type;
  size_t length;
  void* value;
};

struct dimItem {
    char* name;
    size_t length;
};

struct varAttr* getVarAttributes(int ncid, char var_name[], int* len);
float* getVar_float(int ncid, char var_name[], int* size);
short* getVar_short(int ncid, char var_name[], int* size);
const char* getAttribute_str(int ncid, char attr_name[]);
void* getAttributeValue(int ncid, char attr_name[], nc_type* type, size_t* size);
char* value2str(void* value, nc_type type);
struct dimItem* getDimensions(int ncid, int* num_dim);
int get_var_attribute(int ncid, char* var_name, char* attr_name, struct varAttr* attr_value);
