#include "Header.h"


/*
 * dd if=/dev/zero of=disk bs=1024 count=4096
 * mkfs -b 1024 disk 4096
 */
//***** Function Prototypes *****
int quit(char*), menu(char*);
int init(void);
int mount_root(void);
int get_command_index(char*,int*), execute_cmd(int,int,char*);

//***** Global Variables *****
//Main Commands
int (*main_fptr[])(char*) = { quit, menu, NULL };
char* main_cmds[] = { "quit", "menu", NULL };

//Level1 Commands
int (*lvl1_fptr[])(char*) = {_cd, _ls,  _pwd, _chmod, _stat, _creat, _touch, _rm, _mkdir, _rmdir, _link, _symlink, _readlink, _unlink, NULL };
char* lvl1_cmds[] = { "cd", "ls",  "pwd", "chmod", "stat", "creat", "touch", "rm", "mkdir", "rmdir", "link", "symlink", "readlink", "unlink", NULL };

//Level2 Commands
int (*lvl2_fptr[])(char*) = {_mv, _open, _close, _read, _write, _pfd, _cp, NULL }; //_cat,
char* lvl2_cmds[] = { "mv", "open",  "close", "read", "write", "pfd", "cp", NULL }; //"cat",

//Level2 Commands
int (*lvl3_fptr[])(char*) = { NULL }; //_mount, _unmount
char* lvl3_cmds[] = { NULL }; //"mount", "unmount"

//used to hold all command function pointer arrays
int (**all_fptr[])(char*) = {main_fptr, lvl1_fptr, lvl2_fptr, lvl3_fptr, NULL};



int init(void)
{
    puts("Initializing EXT2 Data Structures");
    //Sets all MINODE's refCount to 0 in the array
    for (int i = 0; i < NUM_MINODE; i++) {
        _MINodes[i].refCount = 0;
        _MINodes[i].dev = 0;
        _MINodes[i].ino = 0;
        _MINodes[i].refCount = 0;
        _MINodes[i].mounted = 0;
        _MINodes[i].mptr = 0;
    }
    for (int i = 0; i < NUM_MTABLE; i++) {
        _MTables[i].dev = 0;
    }
    for (int i = 0; i < NUM_OFT; i++) {
        _Ofts[i].refCount = 0;
    }
    for (int i = 0; i < NUM_PROC; i++){
        _Procs[i].pid = i; //sets the id of the procs
        _Procs[i].uid = i;
        _Procs[i].cwd = 0;
        _Procs[i].status = PROC_FREE; //sets all procs to free
        for (int j = 0; j < NUM_FD; j++) {
            _Procs[i].fd[j] = 0;
        }
        _Procs[i].next = &_Procs[i+1]; //set the address of the next proc
    }
    _Procs[NUM_PROC-1].next = &_Procs[0]; //circular linked list
    _Running = &_Procs[0]; //sets the running proc to P0
}

int mount_root(void) {
    //1. ) Open rootdev
    puts("Mounting EXT2 Root");
    char rootdev[50] = "", device[64] = "../";
    printf("Enter Root-Dev (Disk) Name: ");
    fgets(rootdev, sizeof(rootdev), stdin);
    rootdev[strlen(rootdev)-1] = 0; //removes '\n' from string
    strcat(device, rootdev);
    printf("Device: '%s'\n", device);
    int dev = open(device, O_RDWR | O_RDONLY);
    if (dev < 0)
    {
        print_error("mount_root", "Unable to Open Root Device");
    }

    //2.) get super block of rootdev
    char buf[BLKSIZE];
    get_block(dev, SUPERBLOCK, buf);
    _Super = (SUPER *) buf;
    //3.) check if magic number
    if (_Super->s_magic != EXT2_SUPER_MAGIC)
    {
        print_error("mount_root", "Root Device is Not a Valid EXT2 File System");
    }
    _OpenOFT = 0;//Set the number of open oft values
    // fill mount table mtable[0] with rootdev information
    MTABLE* mp = &_MTables[0];
    // use mtable[0]
    mp->dev = dev;
    // copy super block info into mtable[0]
    mp->ninodes = _Super->s_inodes_count;
    _NumberOfBlocks = mp->nblocks = _Super->s_blocks_count;
    strcpy(mp->devName, rootdev);
    strcpy(mp->mntName, PATH_DELIMITER);
    get_block(dev, GDBLOCK, buf);
    _GroupDec = (GD *)buf;
    int bmap = mp->bmap = _GroupDec->bg_block_bitmap;
    int imap = mp->imap = _GroupDec->bg_inode_bitmap;
    int iblock = _IStartBlock = mp->iblock  = _GroupDec->bg_inode_table;

    printf("|super-magic = %x | bmap = %d | imap = %d | iblock = %d|\n",
            _Super->s_magic, bmap, imap, iblock);
    printf("|nblocks = %d | bfree = %d | ninodes = %d | ifree = %d|\n",
            _NumberOfBlocks, _Super->s_free_blocks_count, _Super->s_inodes_count, _Super->s_free_inodes_count);

    // call iget(), which inc minode’s refCount
    _Root = iget(dev, ROOT_INODE); // get root inode
    mp->mntDirPtr = _Root; // double link
    _Root->dev = dev;
    _Root->mptr = mp;
    for (int i=0; i<NUM_PROC; i++) // set proc’s CWD
    {
        _Procs[i].cwd = iget(dev, ROOT_INODE); // each inc refCount by 1
    }
    printf("root_mount: '%s' mounted on / \n", rootdev);
    return dev;
}

int init_proc(int dev){
    puts("Creating P0 as Running Process");
    _Running = &_Procs[0];
    _Running->status = PROC_BUSY;
    _Running->cwd = _Root;
    printf("Root Ref-Count = %d\n", _Root->refCount);
}

int quit(char* pathname) // write all modified minodes to disk
{
    for (int i = 0; i<NUM_MINODE; i++){
        MINODE *mip = &_MINodes[i];
        if (mip->refCount && mip->dirty){
            mip->refCount = 1;
            iput(mip);
        }
    }
    exit(0);
}

int menu(char* pathname)
{
    puts("\n|$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ Commands $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$|");
    puts("|                                                                                                        |");
    puts("|===============================================  Level1  ===============================================|");
    puts("|                                                                                                        |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| ls <filename: string>                         - list directory contents                                |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| cd <filename: string>                         - change working directory                               |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| pwd                                           - print name of current/working directory                |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| quit                                          - terminates program                                     |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| creat <filename: string>                      - open and possibly create a file                        |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| touch <filename: string>                      - change file timestamps and possibly create a file      |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| rm <filename: string>                         - remove files or directories                            |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| link <filename: string> <linkname: string>    - call the link function to create a link to a file      |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| symlink <filename: string> <linkname: string> - make a new name for a file                             |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| readlink <linkname: string>                   - print resolved symbolic links or canonical file names  |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| unlink <linkname: string>                     - call the unlink function to remove the specified file  |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| mkdir <filename: string>                      - make directories                                       |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| rmdir <filename: string>                      - remove empty directories                               |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| stat <filename: string>                       - display file or file system status                     |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| chmod <mode: octal> <filename: string>        - change file mode bits                                  |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("|                                                                                                        |");
    puts("|===============================================  Level2  ===============================================|");
    puts("|                                                                                                        |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| mv <source: string> <destination: string>                      - move (rename) files                   |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| open <filename: string> <mode: int (0->R, 1->W, 2->RW, 3->AP)> - open and possibly create a file       |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| read <fd: int> <nbytes: int>                                   - read from a file descriptor           |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| write <fd: int> <text: string>                                 - write to a file descriptor            |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("| pfd                                                            - display opened file descriptors       |");
    puts("|--------------------------------------------------------------------------------------------------------|");
    puts("|$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$|\n");
    return 0;
}

void get_line(char* line){

    printf("|Command: ");
    fgets(line, 256, stdin);
    line[strlen(line) - 1] = 0;
}

int get_command_index(char* cmd, int *lvl)
{
    for(int i = 0; main_cmds[i] != NULL; i++){
        if(!strcmp(main_cmds[i], cmd)){
            *lvl = 0;
            return i;
        }
    }
    for(int i = 0; lvl1_cmds[i] != NULL; i++){
        if(!strcmp(lvl1_cmds[i], cmd)){
            *lvl = 1;
            return i;
        }
    }
    for(int i = 0; lvl2_cmds[i] != NULL; i++){
        if(!strcmp(lvl2_cmds[i], cmd)){
            *lvl = 2;
            return i;
        }
    }
    for(int i = 0; lvl3_cmds[i] != NULL; i++){
        if(!strcmp(lvl3_cmds[i], cmd)){
            *lvl = 3;
            return i;
        }
    }
    return -1;
}

int execute_cmd(int lvl, int command, char* pathname)
{
    return all_fptr[lvl][command](pathname);
}


int main(int argc, char *argv[ ])
{
    init();
    int dev = mount_root();
    init_proc(dev);
    while(1){
        char line[128] = "", cmd[16] = "", pathname[64] = "";
        printf("\n|================= P%d Running =================\n",  _Running->pid);
        get_line(line);
        if (line[0] == 0) { continue; }
        sscanf(line, "%s %[^\n]s", cmd, pathname);
        printf("|Cmd: '%s' | Path: '%s'\n", cmd, pathname);
        int cmd_lvl = -1;
        int cmd_index = get_command_index(cmd, &cmd_lvl);
        int result;
        if(cmd_index != -1 && cmd_lvl != -1){
            result = execute_cmd(cmd_lvl, cmd_index, pathname);
            printf("\nCommand returned: %d -> ", result);
            (result != -1) ? puts("(Successful Execution)") : puts("(Failed Execution)");
        }
        else {
            print_notice("main: Invalid Command Entered...");
        }
/*
        if (!strcmp(cmd, "menu")) {
            menu();
        }
        else if (!strcmp(cmd, "ls")) {
            _ls(pathname);
        }
        else if (!strcmp(cmd, "cd")) {
            _cd(pathname);
        }
        else if (!strcmp(cmd, "pwd")) {
            _pwd(_Running->cwd);
        }
        else if (!strcmp(cmd, "quit")) {
            quit(); break;
        }
        else if (!strcmp(cmd, "creat")) {
            _creat(pathname);
        }
        else if (!strcmp(cmd, "touch")) {
            _touch(pathname);
        }
        else if (!strcmp(cmd, "rm")) {
            _rm(pathname);
        }
        else if (!strcmp(cmd,"link")) {
            _link(pathname);
        }
        else if (!strcmp(cmd, "mkdir")) {
            _mkdir(pathname);
        }
        else if (!strcmp(cmd, "rmdir")) {
            _rmdir(pathname);
        }
        else if(!strcmp(cmd, "symlink")){
            _symlink(pathname);
        }
        else if(!strcmp(cmd, "readlink")){
            _readlink(pathname);
        }
        else if(!strcmp(cmd, "stat")){
            _stat(pathname);
        }
        else if(!strcmp(cmd, "chmod")){
            _chmod(pathname);
        }
        else if(!strcmp(cmd, "unlink")){
            _unlink(pathname);
        }
        else if(!strcmp(cmd, "mv")) {
            _mv(pathname);
        }
        else if(!strcmp(cmd, "open")){
            _open(pathname);
        }
        else if(!strcmp(cmd, "close")){
            _close(pathname);
        }
        else if(!strcmp(cmd, "read")){
            _read(pathname);
        }
        else if(!strcmp(cmd, "pfd")){
            _pfd(pathname);
        }
        */
    }
}