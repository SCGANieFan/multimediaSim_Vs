#include "eshell.h"
#include "hal_trace.h"
//#include "cmsis_os2.h"
//#include "hal_timer.h"
//#include "string.h"
//#include "hal_location.h"
//#include "smf_media_api.h"
#include "hal_sysfreq.h"
//#include "cmsis_os.h"
#include "smf_api.h"

#define SMF_FS_LIST 0
#include "fs/fs.h"
//static void test_sysfreq_get(int argc, char *argv[]) {
//    TRACE(1,"sys freq calc : %ld\n", (long)hal_sys_timer_calc_cpu_freq(5, 0));
//}
//
//static void test_sysfreq_set(int argc, char *argv[]) {
//    TRACE(1,"set sys freq ");
//    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_208M);
//    TRACE(1,"sys freq calc : %ld", (long)hal_sys_timer_calc_cpu_freq(5, 0));
//}
//ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_SMF, "test_sysfreq_get", "test_sysfreq_get", test_sysfreq_get);
//ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_SMF, "test_sysfreq_set", "test_sysfreq_set", test_sysfreq_set);

#if SMF_FS_LIST
static void test_smf_fs_list() {
    DIR *dir;
    struct dirent *entry;

    dir = opendir("data/emmc0");
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }
    while ((entry = readdir(dir)) != NULL) {
        TRACE(0, "data/emmc0/%s", entry->d_name);
    }
    closedir(dir);
}
#endif
static void test_smf_cmd_entry(int argc, char *argv[]) {
    TRACE(1,"test_smf_cmd_entry ");
    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_208M);
    if( memcmp(argv[1], "fs", 2) == 0){
    #if SMF_FS_LIST
        test_smf_fs_list();
    #endif
    }else{
        smf_cmd_entry(argc, argv);
    }
}

ESHELL_DEF_COMMAND(ESHELL_CMD_GRP_SMF, "smf", "smf_cmd_entry", test_smf_cmd_entry);


