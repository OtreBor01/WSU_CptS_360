#include "Header.h"

int init()
{
    for (int i=0; i<NMINODE; i++) {
        _MINode[i].refCount = 0;
    }
    for (int i=0; i<NMTABLE; i++) {
        _MTable[i].dev = 0;
    }
    for (int i=0; i<NOFT; i++) {
        _Oft[i].refCount = 0;
    }
    for (int i=0; i<NPROC; i++){
        _Procs[i].status = READY; //
        _Procs[i].pid = i;
        _Procs[i].uid = i;
        for (int j=0; j<NFD; j++) {
            _Procs[i].fd[j] = 0;
        }
        _Procs[i].next = &_Procs[i+1];
    }
    _Procs[NPROC-1].next = &_Procs[0];
    _Running = &_Procs[0];
}

int mount_root(char *rootdev) {
    char buf[BLKSIZE];
    int dev = open(rootdev, O_RDWR);
    if (dev < 0) {
        printf("panic : can’t open root device\n");
        exit(1);
    }
    /* get super block of rootdev */
    get_block(dev, 1, buf);
    _Super = (SUPER *) buf;
    /* check magic number */
    if (_Super->s_magic != SUPER_MAGIC) {
        printf("super magic = %x : %s is not an EXT2 filesys\n", _Super->s_magic, rootdev);
        exit(0);
    }
    // fill mount table mtable[0] with rootdev information
    MTABLE *mp = &_MTable[0];
    // use mtable[0]
    mp->dev = dev;
    // copy super block info into mtable[0]
    _IStartBlock = mp->ninodes = _Super->s_inodes_count;
    _NumberOfBlocks = mp->nblocks = _Super->s_blocks_count;
    strcpy(mp->devName, rootdev);
    strcpy(mp->mntName, PATH_DELIMITER);
    get_block(dev, 2, buf);
    _GroupDec = (GD *)buf;
    int bmap = mp->bmap = _GroupDec->bg_block_bitmap;
    int imap = mp->imap = _GroupDec->bg_inode_bitmap;
    int iblock = mp->iblock = _GroupDec->bg_inode_table;
    printf("bmap=%d imap=%d iblock=%d\n", bmap, imap, iblock);
    // call iget(), which inc minode’s refCount
    _Root = iget(dev, 2); // get root inode
    mp->mntDirPtr = _Root; // double link
    //_Root->mptr = mp;
    for (int i=0; i<NPROC; i++) // set proc’s CWD
    {
        _Procs[i].cwd = iget(dev, 2); // each inc refCount by 1
    }
    printf("mount : %s mounted on / \n", rootdev);
    return 0;
}
int quit() // write all modified minodes to disk
{
    for (int i = 0; i<NMINODE; i++){
        MINODE *mip = &_MINode[i];
        if (mip->refCount && mip->dirty){
            mip->refCount = 1;
            iput(mip);
        }
    }
    exit(0);
}

int main(int argc, char *argv[ ])
{
    char line[128], cmd[16], pathname[64];
    char* rootdev = "disk";
    if (argc > 1) {
        rootdev = argv[1];
    }
    init();
    mount_root(rootdev);
    while(1){
        printf("P%d running: ", _Running->pid);
        printf("Command: ");
        fgets(line, 128, stdin);
        line[strlen(line)-1] = 0;
        if (line[0] == 0) { continue; }
        sscanf(line, "%s %s", cmd, pathname);
        if (!strcmp(cmd, "ls")){
            //ls(pathname);
        }
        else if (!strcmp(cmd, "cd")) {
            //cd(pathname);
        }
        else if (!strcmp(cmd, "pwd")) {
            //pwd(_Running->cwd);
        }
        else if (!strcmp(cmd, "quit")) {
            quit();
        }
    }
}