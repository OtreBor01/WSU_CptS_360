//
// Created by austinmm on 4/23/19.
//

#include "Level3.h"
/*
 * prints the mounted file systems and the directories they are mounted to
 */
int mountStat(void){
    for(int i = 0; i < _Total_Mounts; i++){
        printf("%s is mounted on %s", _MTables[i].devName, _MTables[i].mntName);
    }
}


/*
 * Returns 1 if mounted and 0 if not mounted
 */
int checkMounted(char* diskname){
    int i = 0;
    while((i < _Total_Mounts) & (strlen(_MTables[i].devName) != 0)){
        if(strcmp(_MTables[i].devName, diskname) == 0){
            return -1;
        }
    }
    return 0;
}

/*
 *Checks if a path is currently a processes cwd
 */
int checkCwdBusy(char* pathname){
    int i = 0;
    while(i < NUM_PROC & strlen(_Procs[i].cwd)!= 0 ){
        if(strcmp(_Procs[i].cwd,pathname)==0){
            print_notice("Directory is currently busy");
            return -1;
        }
    }
    return 0;
}

