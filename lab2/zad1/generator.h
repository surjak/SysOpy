#ifndef GENERATOR_LIB_H
#define GENERATOR_LIB_H

void generate(const char *file_name, const unsigned int num_records, const unsigned int byte_num);

void copy_lib(const char *file_from, const char *file_to, const unsigned int num_records, const unsigned int byte_num);

void copy_sys(const char *file_from, const char *file_to, const unsigned int num_records, const unsigned int byte_num);

void sort_sys(const char *file_name, const unsigned int num_records, const unsigned int byte_num);

void sort_lib(const char *file_name, const unsigned int num_records, const unsigned int byte_num);

#endif