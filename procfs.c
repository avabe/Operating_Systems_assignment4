#include "types.h"
#include "stat.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

int numberOfInodes;
char helper[100];
int procNumber;
//static struct dirent dirArray[5];

union block {
    struct {
        char  buf[1056];
    } s;
};

typedef union block blo;
static char bufer[1056];


//static blo block;
static blo procBlcok;
struct dirent dirArray[5];
static struct DirBLock dirBLock[5];

int numberOfBit(int readWaiting)
{
    int bits=0;
    while (readWaiting)
    {
        readWaiting= readWaiting >> 1;
        ++bits;
    }
    return ((bits / 8)+1);
}





//static BLock filestat;
//minor 200 mean Proc
//minor 201 filestat

char *
itoa (int value, char *result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

//make from number string and puting str+string(number) in target
void createStringFronIntAndString(char* target,char* str,int number)
{

    char* helper=target;
    char array[numberOfBit(number)];
    itoa(number, array, 10);
    safestrcpy(helper,str,strlen(str)+1);
    helper=helper+strlen(str);
    safestrcpy(helper,array,strlen(array)+1);
}
//clean the buffer
void cleanBuffer(char* str,int size)
{
    memset(str,'\0',size);
}



void addTwoString(char* target,char* first,char *second)
{
  //  cprintf("the first %s and the size is %d \n",first,strlen(first));

    //cprintf("the second %s and the size %d \n",second,strlen(second));

    char* helper;
    helper=target;

    safestrcpy(helper,first,strlen(first)+1);
    int sign=0;
    for(int i=0;i<1056;i++)
    {
        if(helper[i]==0)
        {
            sign=i;
            helper[i]=' ';
            break;
        }
    }
    helper=helper+sign;
    safestrcpy(helper,second,strlen(first)+1);



}




struct dirent createDir(char*name ,int inumNumber){
    struct dirent dir;
    dir.inum = inumNumber;
    memmove(dir.name, name, strlen(name)+1);
    return dir;
}
void apprendDirectoryToBuf(char *buff,struct dirent* dir, int place){
    struct dirent forHelp;
    int size=sizeof(forHelp);

    memmove(buff + place*size , dir, size);
    cprintf("the buff is %d \n",buff);

}
void createDefaultDir()
{
    struct dirent ideinfo=createDir("ideinfo",201);
    struct dirent filestat=createDir("filestat",202);
    struct dirent inodeinfo=createDir("inodeinfo",203);
    char* current=procBlcok.s.buf;
    apprendDirectoryToBuf(current,&ideinfo,0);
    apprendDirectoryToBuf(current,&filestat,1);
    apprendDirectoryToBuf(current,&inodeinfo,2);

}




void
printTheINode(struct inode* ip)
{
    cprintf("major %d \n",ip->major);
    cprintf("dev %d \n",ip->dev);
    cprintf("inum %d \n",ip->inum);
    cprintf("major %d \n",ip->major);
    cprintf("minor %d \n",ip->minor);

}

void printTheBUffer(char *dsf)
{
    cprintf("-------printTheBUffer-----\n");
    cprintf(dsf);
    cprintf("\n-------EndprintTheBUffer-----\n");



}

int findTheRightDirBlock(int inum)
{
    return inum-200;

}
void
cleanTheDirName(struct dirent* dir)
{
    for(int i=0;i<sizeof(dir->name);i++)
    {
        dir->name[i]='\0';
    }
}

void printTheAppendDir(char *buff, char * dirName, int inum, int dPlace)
{
    cprintf("the buff (%s) the dirname (%s) the inum (%d) the dPlace (%d) \n",buff,dirName,inum,dPlace);
}

void appendDirentToBufEnd(char *buff, char * dirName, int inum, int dPlace){
    struct dirent dir;
    dir.inum = inum;
    //cleanTheDirName(&dir);
    //printTheBUffer(dir.name);
    memmove(dir.name, dirName, strlen(dirName)+1);
    //printTheBUffer(dir.name);
    //printTheAppendDir(buff,dirName,inum,dPlace);
    //cprintf("size of ddir %d \n", sizeof(dir));
    //cprintf("we put it on %d \n",dPlace* sizeof(dir));
    memmove(buff + dPlace*sizeof(dir) , (void*)&dir, sizeof(dir));
    //cprintf("1 the buff is \n %s \n end buf \n ",buff);
}
int
procfsisdir(struct inode *ip) {
    if(numberOfInodes==0)//mean is the init calling
    {
        numberOfInodes=200;
        createDefaultDir();
        return 1;
    }
    if(ip->type==T_DEV || ip->major==PROCFS)
    {
        return 1;
    }



  return 0;
}

void 
procfsiread(struct inode* dp, struct inode *ip) {

    ip->type=T_DEV;
    ip->valid=1;
    ip->major=PROCFS;

}

int
procfsread(struct inode *ip, char *dst, int off, int n) {


    // cprintf("the buf after %s \n",bufer);
    //   memset(bufer,'\0',1056);


    if(numberOfInodes==0 && ip->major==PROCFS && ip->inum==18)
    {
        numberOfInodes=200;
        createDefaultDir();
    }
//cprintf("hello\n");
//cprintf("ip is %d and offset is %d andd the n is %d \n",ip->inum,off,n);
    //cprintf("procfsread the offset is %d and inum is %d \n",off,ip->inum);

    //mean im the procmak
    if(ip->minor==0 && ip->inum==18) {
        ip->size=3*sizeof(struct dirent);
        //cprintf("the dst is %d \n", dst);
        char *current = procBlcok.s.buf+off;
        if(off>ip->size)
            return 0;

        //cprintf("current is %s \n", current);
      //  cprintf("current is %d the off is %d \n", current, off);
        memmove(dst, current, n);
      //  cprintf("the dst is %d \n", dst);
        return sizeof(struct dirent);
    } else
    {
        if(ip->inum==201)
        {
            cleanBuffer(bufer,1056);
            char help[1056];
            char waitingOp[1056];
            char waitingReading[1056];
            char waitingWriting[1056];
            createStringFronIntAndString(waitingOp,"Waiting operations: ",operatWaiting);
            createStringFronIntAndString(waitingReading,"Waiting reading : ",readWaiting);
            createStringFronIntAndString(waitingWriting,"Waiting writing : ",writeWaiting);
            addTwoString(help,waitingOp,waitingReading);
            addTwoString(bufer,help,waitingWriting);
          //  cprintf("the waitingop %s \n",waitingOp);
           // cprintf("the waitingRE %s \n",waitingReading);
           // cprintf("the waitingWr %s \n",waitingWriting);
            cprintf("\nthe bufer %s \n",bufer);
            return 0;



            if(off==0) {
              //  cprintf("the number of n is %d \n",n);
             //   cprintf("im here true\n");
                char helper[1056];
                safestrcpy(helper,"waiting opearions ",strlen("waiting opearions "));
                char*pointer=helper;
                pointer+=strlen("waiting opearions");
              //  cprintf("the string is %s \n",helper);
                int reading = operatWaiting;
                char array[numberOfBit(reading)];
                itoa(reading, array, 10);
                safestrcpy(pointer,array,strlen(array)+1);
                pointer=pointer+sizeof(array);
                safestrcpy(pointer,"reading operations ",strlen("reading operations "));
                 reading = readWaiting;
              //  cprintf("the buf is %s \n",helper);
                char array1[numberOfBit(reading)];
                itoa(reading, array1, 10);
                safestrcpy(pointer,array1,strlen(array1)+1);
                pointer=pointer+sizeof(array1);
                int size=pointer+1-helper;
             //  memmove(dst, helper, size);
              //  cprintf("the size os dst is %d and strleng is %d and size is %d\n",sizeof(dst),strlen(dst),size);
                memmove(dirBLock[0].buf,helper,size);
               cprintf("the helper is %s \n",helper);
               cprintf("the buf is %s \n",dirBLock[0].buf);
                return size;
            }
            else if(off<60)
            {
                char* forCurrent=dirBLock[0].buf;
                forCurrent+=off;
                memmove(dst, forCurrent, n);
                return 16;
            } else
            {
                return 0;
            }
        }
    }

/*
        if (off == 0) {
            appendDirentToBufEnd(dst, "ideinfo", 202, 0);
        }
        else if (off == 16) {
            appendDirentToBufEnd(dst, "pros", 203, 0);
        } else
            return 0;
        return sizeof(struct dirent);
    }*/
    if(ip->inum==201)
    {
        cprintf("itts true \n");
    }
    if(ip->inum==203)//mean im in pros
    {
        cprintf("the offset %d \n",off);
        memset(dst, 0, sizeof(dst));
        if(off==0) {
            appendDirentToBufEnd(dst,"ohad", 204, 0);
            return sizeof(struct dirent);
        } else{
            return 0;
        }
    }

    return 0;
}
/*
int
procfsread(struct inode *ip, char *dst, int off, int n) {

//cprintf("hello\n");
//cprintf("ip is %d \n",ip->inum);
cprintf("procfsread the offset is %d and inum is %d \n",off,ip->inum);


    if(namei("/proc")->inum==ip->inum)
    {
        if(off==0 && ip->inum==18)
        {
            appendDirentToBufEnd(dst,"filestat",202,0);
            appendDirentToBufEnd(dst, "ideinfo", 203, 1);

            return sizeof(struct dirent);
        }
        if(off==0) {
            cprintf("offset is 0 \n");
            appendDirentToBufEnd(dst, "ideinfo", 202, 0);
        }
        else if(off==16)
        {
            cprintf("offset is 16  \n");
            appendDirentToBufEnd(dst,"filestat",203,1);
            return  sizeof(struct dirent);
        }
        else
            return 0;

      int toRet= sizeof(struct dirent);
        //cprintf("the sizeof dst is %d \n",
          //      sizeof(toRet));

      return toRet;
    }
    if(ip->inum==202)
    {
        cprintf("ip->inum==202 is true\n");
      //  char c[] = "abcd";
       // memmove(dst,c, sizeof(c));

        return strlen(dst);
    }
    if(ip->inum==203)
    {
        cprintf("ip->inum==203 is true\n");
        printTheINode(ip);
        memmove(dst,"eyal the king", sizeof("eyal the king"));
        return strlen(dst);
    }
  return 0;

}*/

int
procfswrite(struct inode *ip, char *buf, int n)
{
  return -1;//write only
}

void
procfsinit(void)
{
  devsw[PROCFS].isdir = procfsisdir;
  devsw[PROCFS].iread = procfsiread;
  devsw[PROCFS].write = procfswrite;
  devsw[PROCFS].read = procfsread;

}

void
procdump(void)
{
    cprintf("fuck");
}

