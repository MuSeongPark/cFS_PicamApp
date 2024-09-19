#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */
#include "picam_app_table.h"

PICAM_APP_Table_t PicamAppTable = {1, 2};

/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(PicamAppTable, PICAM_APP.PICAM_AppTable, Table Utility Test Table, picam_app_tbl.tbl )
//2) the name of the table must be same with CFE_TBL_Register()'s second parameter
