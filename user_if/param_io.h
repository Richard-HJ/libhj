/*
	param_io.h	R.E. Hughes-Jones  	University of Manchester
	include file defining the parameter save/restore interface functions
*/

#ifndef  _PARAM_IO_H_
#define  _PARAM_IO_H_

#define CFG_DEC     0x01
#define CFG_IP      0x04
#define CFG_MAC     0x06
#define CFG_TXT     0x08

struct Params_tbl {
  void *address;
  int type;
  char desc[33];
};


/* functions */
void params_in(char *config_filename, struct Params_tbl *param_tbl_ptr);
void params_out(char *config_filename, struct Params_tbl *param_tbl_ptr);

#endif  /* _PARAM_IO_H_ */
