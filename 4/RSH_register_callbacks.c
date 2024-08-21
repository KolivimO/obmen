
/*HEAD REGISTER_RULE_HANDLERS_USING_CUSTOM_EXITS CCC ITK */
/*
    Instructions:
        Save this file as GTAC_register_callbacks.c
        The naming format is very important.
            GTAC_register_callbacks.c
            GTAC_register_callbacks() - function name
            GTAC - the name of the executable

        Compile
            %TC_ROOT%\sample\compile -DIPLIB=libuser_exits
                 GTAC_register_callbacks.c
        Link
            %TC_ROOT%\sample\link_custom_exits GTAC
        Move File
            copy GTAC.dll %TC_BIN%


        Set the following site environment variable in the .iman_env in TCEng V9
        or using Preference Manager in later versions.
        TC_customization_libraries=
        GTAC
*/

#include <ict/ict_userservice.h>
#include <tccore/custom.h>
#include <itk/mem.h>
#include <tc/tc.h>
#include <user_exits/user_exits.h>
#include <tc/emh.h>
#include <epm/epm.h>
#include <sa/sa.h>
#include <stdarg.h>
#include <stdarg.h>
#include <ug_va_copy.h>
#include <itk/mem.h>
#include <tc/tc_startup.h>
#include <tc/wsouif_errors.h>
#include <tc/emh.h>
#include <user_exits/user_exits.h>
#include <property/prop.h>
#include <property/prop_msg.h>
#include <property/prop_errors.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <tccore/tctype.h>
#include <tccore/item.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/method.h>
#include <tccore/workspaceobject.h>
#include <lov/lov.h>
#include <sa/sa.h>
#include <res/reservation.h>
#include <form/form.h>

static void ECHO(char *format, ...)
{
    char msg[1000];
    va_list args;
    va_start(args, format);
    vsprintf(msg, format, args);
    va_end(args);
    printf(msg);
    TC_write_syslog(msg);
}


#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
#define ERROR_CHECK(X) if (IFERR_REPORT(X)) return (X)


static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors = 0,
            *severities = NULL,
            *statuses = NULL;
        char
            **messages;


        EMH_ask_errors(&n_errors, &severities, &statuses, &messages);
        if (n_errors > 0)
        {
            ECHO("\n%s\n", messages[n_errors-1]);
            EMH_clear_errors();
        }
        else
        {
            char *error_message_string;
            EMH_get_error_string (NULLTAG, status, &error_message_string);
            ECHO("\n%s\n", error_message_string);
        }


        ECHO("error %d at line %d in %s\n", status, line, file);
        ECHO("%s\n", call);


        if (exit_on_error)
        {
            ECHO("Exiting program!\n");
            exit (status);
        }
    }

    return status;
}
// extern DLLAPI int GTAC_action_handler(EPM_action_message_t message)
/*

*/
extern DLLAPI int RSHtoggleAttribute_handler(EPM_action_message_t message)
{
	tag_t *attachments = NULL;
	tag_t job_tag = NULLTAG;
	tag_t root_tag = NULLTAG;
	tag_t rsh_attr_id = NULLTAG;

	logical is_it_null;
    logical is_it_empty;
    logical is_checked_out = FALSE;
    logical internal_checkout = FALSE;

	char *object_string = NULL;
	char *rsh_attr_value = NULL;
	char *c9ColorName = NULL;
	char *c9Rvalue = NULL;
	char *c9Gvalue = NULL;
	char *c9Bvalue = NULL;
	char *c9MakeBuy = NULL;

	int n_attachments = 0;
	int ifail = 0;
	int n_table_rows = 0;
    tag_t *table_rows = NULLTAG;

	FILE *fp;

	fp = fopen("RSHtoggleAttribute_handler.log", "w");

    fprintf(fp, "\n Inside RSHtoggleAttribute_handler \n");

EPM_ask_job(message.task,&job_tag);
EPM_ask_root_task(job_tag,&root_tag);
EPM_ask_attachments(root_tag, EPM_target_attachment, &n_attachments, &attachments);

IFERR_REPORT(RES_is_checked_out(attachments[0], &is_checked_out));

if(is_checked_out == FALSE)
 {
   internal_checkout = TRUE;
   IFERR_REPORT(RES_checkout(attachments[0], "auto checkout", "001", "C:\\temp", RES_EXCLUSIVE_RESERVE));
   fprintf(fp, "\n Checked out Item \n"); fflush(fp);
}

/*
IFERR_REPORT(AOM_ask_table_rows(attachments[0], "c9testTable", &n_table_rows, &table_rows));
fprintf(fp, "\n number of c9testTable rows -> [%i]  \n", n_table_rows); fflush(fp);

for(int row=0; row < n_table_rows; row++)
 {
IFERR_REPORT(AOM_ask_value_string(table_rows[row], "c9ColorName", &c9ColorName));
fprintf(fp, "\n c9testTable.c9testTableObj.c9ColorName -> [%s]  \n", c9ColorName); fflush(fp);
IFERR_REPORT(AOM_ask_value_string(table_rows[row], "c9Rvalue", &c9Rvalue));
IFERR_REPORT(AOM_refresh(table_rows[row], TRUE));
if(strcmp(c9Rvalue, "20")==0)
  {
	fprintf(fp, "\n Passed [strcmp-c9Rvalue, 20]=0  \n"); fflush(fp);
    IFERR_REPORT(AOM_set_value_string(table_rows[row], "c9Rvalue", "25"));
  }
else
   {
	 if(strcmp(c9Rvalue, "25")==0)
	  {
		 fprintf(fp, "\n Passed [strcmp-c9Rvalue, 25]=0  \n"); fflush(fp);
	     IFERR_REPORT(AOM_set_value_string(table_rows[row], "c9Rvalue", "20"));
      }
   }

fprintf(fp, "\n c9testTable.c9testTableObj.c9Rvalue -> [%s]  \n", c9Rvalue); fflush(fp);
IFERR_REPORT(AOM_ask_value_string(table_rows[row], "c9Gvalue", &c9Gvalue));
fprintf(fp, "\n c9testTable.c9testTableObj.c9Gvalue -> [%s]  \n", c9Gvalue); fflush(fp);
IFERR_REPORT(AOM_ask_value_string(table_rows[row], "c9Bvalue", &c9Bvalue));
fprintf(fp, "\n c9testTable.c9testTableObj.c9Bvalue -> [%s]  \n", c9Bvalue); fflush(fp);
IFERR_REPORT(AOM_ask_value_string(table_rows[row], "c9MakeBuy", &c9MakeBuy));
fprintf(fp, "\n c9testTable.c9testTableObj.c9MakeBuy -> [%s]  \n", c9MakeBuy); fflush(fp);


IFERR_REPORT(AOM_save(table_rows[row]));
IFERR_REPORT(AOM_refresh(table_rows[row], FALSE));
 }

 */


WSOM_ask_object_id_string(attachments[0], &object_string);

POM_attr_id_of_attr("object_desc", "Item", &rsh_attr_id);
POM_ask_attr_string(attachments[0], rsh_attr_id, &rsh_attr_value, &is_it_null, &is_it_empty);

	IFERR_REPORT(AOM_load(attachments[0]));
	IFERR_REPORT(AOM_refresh(attachments[0], TRUE));

if(tc_strcmp(rsh_attr_value, "RED")==0 || tc_strcmp(rsh_attr_value, "EMPTY")==0)
    IFERR_REPORT(AOM_set_value_string(attachments[0], "object_desc", "BLUE"));
else
  {
     if(tc_strcmp(rsh_attr_value, "BLUE")==0)
       IFERR_REPORT(AOM_set_value_string(attachments[0], "object_desc", "RED"));
     else
       IFERR_REPORT(AOM_set_value_string(attachments[0], "object_desc", "EMPTY"));
  }

	IFERR_REPORT(AOM_save(attachments[0]));
	IFERR_REPORT(AOM_unload(attachments[0]));

	if(internal_checkout == TRUE)
	 {
	   IFERR_REPORT(RES_checkin(attachments[0]));
	   internal_checkout = FALSE;
	   fprintf(fp, "\n Checked in Item \n"); fflush(fp);
	 }

    fclose(fp);
	return ITK_ok;
}




extern DLLAPI int Register_RSHtoggleAttribute_handler(int *decision, va_list args)
{
     int ec = ITK_ok;
    *decision = ALL_CUSTOMIZATIONS;

    ec = EPM_register_action_handler("RSH-action-handler", "", RSHtoggleAttribute_handler);

    return ec;
}


extern DLLAPI int RSH_register_callbacks()
{

    CUSTOM_register_exit("RSH", "USER_gs_shell_init_module",
                                               (CUSTOM_EXIT_ftn_t) Register_RSHtoggleAttribute_handler );

    return ITK_ok;
}
