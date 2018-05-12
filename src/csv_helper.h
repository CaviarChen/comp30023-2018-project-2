/* TLS Certificate Checker (CSV File Helper)
 * Name: Zijun Chen
 * StudentID: 813190
 * LoginID: zijunc3
 */


#ifndef CSV_HELPER_H
#define CSV_HELPER_H

void* csv_open(const char* filename, const char* mode);
void csv_close(void* csv);

void csv_write_row(void* csv, char** row, int num_fields);

int csv_get_row(void* csv, char** row, int max_fields);
void csv_free_row(char** row);



#endif
