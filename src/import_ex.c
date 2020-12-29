#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>

#include "utils.h"
#include "netcdf_utils.h"


#define ERR(e) {printf("Error: %s\n", nc_strerror(e));}

//-----------------------------------------------------
// Example of reading attributes of variable:
//-----------------------------------------------------
void show_var_attributes(int ncid, char* var_name) {
  int attr_num; // The number of attributes
  // The getVarAttributes() function collects all attributes into a single data structure:
  struct varAttr* attributes = getVarAttributes(ncid, var_name, &attr_num);
  
  int i;
  for(i=0; i < attr_num; i++) {
    printf(
      "\tATTR %s, type: %d, length: %u, VALUE: '%s'\n", 
      attributes[i].name, 
      attributes[i].type, 
      attributes[i].length, 
      value2str(attributes[i].value, attributes[i].type)
    );
  }
}


void read_netcdf(char nomfic[]) {
  int ncid, retval;

  if (
    (retval = nc_open(nomfic, NC_NOWRITE, &ncid))
  ) ERR(retval);
  

  //-----------------------------------------------------
  // Example of reading dimensions:
  //-----------------------------------------------------
  int dim_num;
  struct dimItem* dimensions = getDimensions(ncid, &dim_num);
  int dim_i = 0;
  for(; dim_i < dim_num; dim_i++) {
    printf("Dimension %s: %u\n", dimensions[dim_i].name, dimensions[dim_i].length);
  }


  //-----------------------------------------------------
  // Example of reading global attributes:
  //-----------------------------------------------------
  int attr_num = 5;
  const char* global_attributes[] = {"site_name" /* string */, "nb_equ_bins" /* int */, "pct_bins" /* float */, "first_row" /* short */, "earth_radius" /* double */};
  
  int attr_i =0;
  for(; attr_i < attr_num; attr_i++) {
    nc_type type;
    size_t size;
    void* value = getAttributeValue(ncid, (char*)global_attributes[attr_i], &type, &size);
    if (value) {
      printf("Glob. attr `%s`, type %d, size %u, value: `%s`\n", global_attributes[attr_i], type, size, value2str(value, type));
    }
  }
  
  //-----------------------------------------------------
  // Example of reading a global attribute with a string type:
  //-----------------------------------------------------
  // Retrieving the value of <parameter_code>_mean 
  // <attribute name="parameter_code" value="NRRS555" />
  char attr_name[] = "parameter_code";
  const char* attr_value = getAttribute_str(ncid, attr_name);
  printf("parameter_code: '%s'\n", attr_value);
  

  //-----------------------------------------------------
  // Example of reading data of a variable with the float type:
  //-----------------------------------------------------
  // <variable name="NRRS555_mean" shape="bin" type="float">
  char* meanVarName = join_str(attr_value, (const char*)"_mean");
  
  int meanVarLen; // The number of elements
  float* meanRes = getVar_float(ncid, meanVarName, &meanVarLen);
  // meanRes is an array of the float type
  printf("Var: '%s[%d]'\n", meanVarName, meanVarLen);
  if (meanRes) {
    printf("\t[0]=%f, [1]=%f, [2]=%f ... [%d]=%f\n", meanRes[0], meanRes[1], meanRes[2], meanVarLen-1, meanRes[meanVarLen-1]);
    show_var_attributes(ncid, meanVarName);
    // Clearing unused memory
    free(meanVarName);
    free(meanRes);
  }
  printf("\n");


  // <variable name="NRRS555_count" shape="bin" type="short">
  char* countVarName = join_str(attr_value, (const char*)"_count");
  int countVarLen;
  short* countRes = getVar_short(ncid, countVarName, &countVarLen);
  printf("Var: '%s[%d]'\n", countVarName, countVarLen);
  if (countRes) {
    printf("\t[0]=%d, [1]=%d, [2]=%d ... [%d]=%d\n", countRes[0], countRes[1], countRes[2], meanVarLen-1, countRes[countVarLen-1]);
    free(countVarName);
    free(countRes);
  }
  
  if ((retval = nc_close(ncid))) ERR(retval);

  printf("\n------------------------\n");

  return;
}

int main(int argc, char *argv[])
{
  FILE *fic_list;
  
  if ((fic_list=fopen(argv[1], "r")) == NULL) 
  { 
    printf("Can not read file: %s", argv[1]); 
    exit(-1);
  }

  char nomfic[250];
  while (fscanf(fic_list, "%s", &nomfic[0]) != EOF) 
  {
    printf("- %s\n",nomfic);
    read_netcdf(nomfic);
  }
  
  fclose(fic_list);
}
