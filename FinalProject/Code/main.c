#include "Header.h"

int init(void)
{
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

int mount_root(char *rootdev) {
    char device[64] = "../";
    strcat(device, rootdev);
    printf("Device: '%s'\n", device);
    int dev = open(device, O_RDWR | O_RDONLY);
    if (dev < 0)
    {
        print_error("mount_root", "Unable to Open Root Device");
    }
    /* get super block of rootdev */
    char buf[BLKSIZE];
    get_block(dev, SUPERBLOCK, buf);
    _Super = (SUPER *) buf;
    /* check magic number */
    if (_Super->s_magic != EXT2_SUPER_MAGIC)
    {
        print_error("mount_root", "Root Device is Not a Valid EXT2 File System");
    }
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
    printf("bmap=%d imap=%d iblock=%d\n", bmap, imap, iblock);
    // call iget(), which inc minode’s refCount
    _Root = iget(dev, ROOT_INODE); // get root inode
    mp->mntDirPtr = _Root; // double link
    _Root->dev = dev;
    //_Root->mptr = mp;
    for (int i=0; i<NUM_PROC; i++) // set proc’s CWD
    {
        _Procs[i].cwd = iget(dev, ROOT_INODE); // each inc refCount by 1
    }
    printf("root_mount: '%s' mounted on / \n", rootdev);
    return dev;
}

int init_proc(int dev){
    printf("Root Ref-Count = %d\n", _Root->refCount);
    puts("Creating P0 as Running Process");
    _Running = &_Procs[0];
    _Running->status = PROC_BUSY;
    _Running->cwd = _Root;
    printf("Root Ref-Count = %d\n", _Root->refCount);
}

int quit(void) // write all modified minodes to disk
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

void get_line(char* line){
    puts("|***** [ls|cd|pwd|creat|mkdir|link|quit] *****");
    printf("|Command: ");
    fgets(line, 256, stdin);
    line[strlen(line) - 1] = 0;

}

int main(int argc, char *argv[ ])
{
    char* rootdev = "disk";
    if (argc > 1) {
        rootdev = argv[1];
    }
    init();
    int dev = mount_root(rootdev);
    init_proc(dev);
    while(1){
        char line[128] = "", cmd[16] = "", pathname[64] = "";
        printf("\n|================= P%d Running =================\n",  _Running->pid);
        get_line(line);
        if (line[0] == 0) { continue; }
        sscanf(line, "%s %[^\n]s", cmd, pathname);
        printf("|Cmd: '%s' | Path: '%s'\n", cmd, pathname);
        if (!strcmp(cmd, "ls")) {
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
        else if (!strcmp(cmd,"link")) {
            _link(pathname);
        }
        else if (!strcmp(cmd, "mkdir")) {
            _mkdir(pathname);
        }
        else {
            printf("Invalid Command Entered...\n");
        }
    }
}