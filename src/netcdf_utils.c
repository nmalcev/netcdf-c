#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include <stdio.h>
#include "utils.h"

struct varAttr{
  char* name;
  nc_type type;
  size_t length;
  void* value;
};

struct varAttr* getVarAttributes(int ncid, char var_name[], int* len) 
{
    int retval, varid;
    if ((retval = nc_inq_varid(ncid, var_name, &varid))) {
        return NULL;
    }
    
    nc_type att_type;
    int num_attributes;

    if ((retval = nc_inq_varnatts(ncid, varid, &num_attributes))) {
        return NULL;
    }
    *len = num_attributes;
    struct varAttr * items = malloc((num_attributes) * sizeof(struct varAttr));

    char name[NC_MAX_NAME + 1];
    size_t att_length;
    int i;
    for (i = 0; i < num_attributes; i++) {
        if ((retval = nc_inq_attname(ncid, varid, i, name))) {
            return NULL;
        }
        if ((retval = nc_inq_atttype (ncid, varid, name, &att_type))) {
            return NULL;
        }
        if ((retval = nc_inq_attlen (ncid, varid, name, &att_length))) {
            return NULL;
        }

        if (att_type == NC_CHAR) {
            char *char_value;
            char_value = (char *) malloc ((att_length + 1) * sizeof (char));
            if ((retval = nc_get_att_text(ncid, varid, name, char_value))) {
                return NULL;
            }
            items[i] = (struct varAttr){clone_str(name), att_type, att_length, (void*)char_value};
        } else if (att_type == NC_FLOAT) {
            float *float_values;
            float_values = (float *) malloc ((att_length + 1) * sizeof (float));
            if ((retval = nc_get_att_float(ncid, varid, name, float_values))) {
                return NULL;
            }
            items[i] = (struct varAttr){clone_str(name), att_type, att_length, (void*)float_values};
        }
    }
    return items;
}

int __getVariableSize(int nc_id, int num_dimensions, int* dimension_ids)
{
    int retval;
    int dim_i;
    int var_dim_id;
    int variable_size = 1;
    size_t dim_length;

    for (dim_i = 0; dim_i < num_dimensions; dim_i++){
        // dimension's ID
        var_dim_id = dimension_ids[dim_i];
        //  What is the size of the dimension with that ID?
        if ((retval = nc_inq_dim(nc_id, var_dim_id, 0, &dim_length))) {
            return -1;
        }

        // if (dim_i < (num_dimensions - 1)) printf ("x ");
        // Keep a running product of the variable dimensions to have the total size when done.
        variable_size *= dim_length;
    }
    return variable_size;
}

float* getVar_float(int ncid, char var_name[], int* size) 
{
  int retval, varid;
  if ((retval = nc_inq_varid(ncid, var_name, &varid))) {
    return NULL;
  }
  
  nc_type type_id;
  char name[NC_MAX_NAME + 1];
  int num_dimensions, dimension_ids[NC_MAX_VAR_DIMS], num_attributes;

  if ((retval = nc_inq_var(ncid, varid, name, &type_id, &num_dimensions, dimension_ids, &num_attributes))) {
    return NULL;
  }

  int variable_size = __getVariableSize(ncid, num_dimensions, dimension_ids);
  *size = variable_size;

  if (type_id == NC_FLOAT) {
    float *var_float_values;
    var_float_values = (float *) malloc(variable_size * sizeof(float));
    if ((retval = nc_get_var_float(ncid, varid, var_float_values))) {
        return NULL;
    }  

    return var_float_values;
  } else {
    return NULL;
  }
}

short* getVar_short(int ncid, char var_name[], int* size){
   int retval, varid;
  if ((retval = nc_inq_varid(ncid, var_name, &varid))) {
    return NULL;
  }
  
  nc_type type_id;
  char name[NC_MAX_NAME + 1];
  int num_dimensions, dimension_ids[NC_MAX_VAR_DIMS], num_attributes;

  if ((retval = nc_inq_var(ncid, varid, name, &type_id, &num_dimensions, dimension_ids, &num_attributes))) {
    return NULL;
  }

  int variable_size = __getVariableSize(ncid, num_dimensions, dimension_ids);
  *size = variable_size;

  if (type_id == NC_SHORT) {
    // If 16-bit signed short int, make an array of shorts and get the
    short *var_short_values;
    var_short_values = (short *) malloc (variable_size * sizeof (short));
    
    if ((retval = nc_get_var_short(ncid, varid, var_short_values))){
        return NULL;
    }
    return var_short_values;
  } else {
    return NULL;
  }
}

const char* getAttribute_str(int ncid, char attr_name[]) 
{
    int retval;
    nc_type xtypep;

    if ((retval = nc_inq_atttype(ncid, NC_GLOBAL, attr_name, &xtypep))) {
        return NULL;
    }

    if (xtypep != NC_CHAR) {
        return NULL;
    }
    size_t lenp;
    if ((retval = nc_inq_attlen(ncid, NC_GLOBAL, attr_name, &lenp))){
        return NULL;
    }

    char *buf = malloc(lenp + 1);
    if (!buf) {
        return NULL;
    }
    if ((retval = nc_get_att_text(ncid, NC_GLOBAL, attr_name, buf))){
        return NULL;
    }
    buf[lenp] = '\0';
    return buf;
}

void* getAttributeValue(int ncid, char attr_name[], nc_type* type, size_t* size) 
{
    int retval;
    nc_type xtype;

    if ((retval = nc_inq_atttype(ncid, NC_GLOBAL, attr_name, &xtype))) {
        return NULL;
    }
    *type = xtype;
    size_t xlen;
    if ((retval = nc_inq_attlen(ncid, NC_GLOBAL, attr_name, &xlen))){
        return NULL;
    }
    *size = xlen;

    if (xtype == NC_CHAR) {
        char *buf = malloc((xlen) * sizeof(char));
        if (!buf) {
            return NULL;
        }
        if ((retval = nc_get_att_text(ncid, NC_GLOBAL, attr_name, buf))){
            return NULL;
        }
        return (void*)buf;
    } else if (xtype == NC_FLOAT) {
        float *var_float;
        var_float = (float *) malloc(xlen * sizeof(float));
        if ((retval = nc_get_att_float(ncid, NC_GLOBAL, attr_name, var_float))){
            return NULL;
        }
        return (void*)var_float;
    } else if (xtype == NC_SHORT) {
        short *var_short;
        var_short = (short *) malloc(xlen * sizeof(short));
        if ((retval = nc_get_att_short(ncid, NC_GLOBAL, attr_name, var_short))){
            return NULL;
        }
        return (void*)var_short;
    } else if (xtype == NC_INT) {
        int *var_int;
        var_int = (int *) malloc(xlen * sizeof(int));
        if ((retval = nc_get_att_int(ncid, NC_GLOBAL, attr_name, var_int))){
            return NULL;
        }
        return (void*)var_int;
    } else if (xtype == NC_DOUBLE) {
        double *var_double;
        var_double = (double *) malloc(xlen * sizeof(double));
        if ((retval = nc_get_att_double(ncid, NC_GLOBAL, attr_name, var_double))){
            return NULL;
        }
        return (void*)var_double;
    } else {
        return NULL;
    }
}

#define MAX 100 
char* value2str(void* value, nc_type type) 
{
    if (type == NC_CHAR) {
        return (char*)value;
    } else {
        char buf[MAX];
        if (type==NC_FLOAT) {
            sprintf(buf, "%f", ((float*)value)[0]);
        } else if (type == NC_INT) {
            sprintf(buf, "%d", ((int*)value)[0]);
        } else if (type == NC_SHORT) {
            sprintf(buf, "%d", ((short*)value)[0]);
        } else if (type == NC_DOUBLE) {
            sprintf(buf, "%f", ((double*)value)[0]);
        }
        return clone_str(buf);
    }
}


struct dimItem {
    char* name;
    size_t length;
};

struct dimItem* getDimensions(int ncid, int* num_dim) 
{
    int retval;
    int num_dimensions;

    if ((retval = nc_inq_ndims(ncid, &num_dimensions))) {
        return NULL;
    }
    struct dimItem* items = malloc((num_dimensions) * sizeof(struct dimItem));
    *num_dim = num_dimensions;

    char name[NC_MAX_NAME + 1];
    size_t length;

    int i = 0;
    for(; i < num_dimensions; i++) {
        if ((retval = nc_inq_dim(ncid, i, name, &length))) {
            return NULL;
        }
        items[i] = (struct dimItem){clone_str(name), length};
    }
    return items;
}

int get_var_attribute(int ncid, char* var_name, char* attr_name, struct varAttr* attr_value) {
  int attr_num; // number of attributes
  // The getVarAttributes() function collects all attributes into a single data structure:
  struct varAttr* attributes = getVarAttributes(ncid, var_name, &attr_num);
  
  int i;
  for(i=0; i < attr_num; i++) {
    if (strcmp(attributes[i].name, attr_name) == 0 ) {
      *attr_value = attributes[i];
      return 0;
    }
  }
  return -1;
}
