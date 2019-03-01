/* Листинг 4.7 */

#include "apue.h"
#include <dirent.h>
#include <limits.h>
#include <errno.h>

/* тип функции, которая будет вызываться для каждого встреченного файла */
typedef int Myfunc(const char *, const struct stat *, int);

static Myfunc myfunc;
static int myftw(char *, Myfunc *);
static int dopath(Myfunc *);
static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int main(int argc, char *argv[])
{
    int ret;
    
    if (argc != 2)
        err_quit("Использование: ftw <начальный_каталог>");
    
    ret = myftw(argv[1], myfunc); /* выполняет всю работу */
    ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
    
    if (ntot == 0)
        ntot = 1;/* во избежание деления на 0 вывести 0 для всех счетчиков */
    
    /*printf("обычные файлы = %7ld, %5.2f %%\n", nreg,
    nreg*100.0/ntot);
    
    printf("каталоги = %7ld, %5.2f %%\n", ndir,
    ndir*100.0/ntot);
    
    printf("специальные файлы блочных устройств = %7ld, %5.2f %%\n", nblk,
    nblk*100.0/ntot);
    
    printf("специальные файлы символьных устройств = %7ld, %5.2f %%\n", nchr,
    nchr*100.0/ntot);
    
    printf("FIFO = %7ld, %5.2f %%\n", nfifo,
    nfifo*100.0/ntot);
    
    printf("символические ссылки = %7ld, %5.2f %%\n", nslink,
    nslink*100.0/ntot);
    
    printf("сокеты = %7ld, %5.2f %%\n", nsock,
    nsock*100.0/ntot);*/
    
    exit(ret);
}

/*
 * Выполняет обход дерева каталогов, начиная с каталога "pathname".
 * Для каждого встреченного файла вызывает пользовательскую функцию func().
 */
#define FTW_F 1     /* файл, не являющийся каталогом */
#define FTW_D 2     /* каталог */
#define FTW_DNR 3   /* каталог, который недоступен для чтения */
#define FTW_NS 4    /* файл, информацию о котором */
                    /* невозможно получить с помощью stat */

static char *fullpath; /* полный путь к каждому из файлов */
static size_t pathlen; 

static int /* возвращает то, что вернула функция func() */myftw(char *pathname, Myfunc *func){
    fullpath = path_alloc(&pathlen); /* выделить память для PATH_MAX+1 байт */
    /* (листинг 2.3) */
    if (pathlen <= strlen(pathname)) {
        pathlen = strlen(pathname) * 2;
        
        if ((fullpath = realloc(fullpath, pathlen)) == NULL)
            err_sys("ошибка вызова realloc");
    }
    
    strcpy(fullpath, pathname);
    
    return(dopath(func));
}

static unsigned lvl = 0;
void tabs(unsigned count, int flag){
    for(int i = 0; i < count; ++i){
        if (!flag)
            printf("=====");
        else
            printf("-----");
    }
    printf(">   ");
}

void print_tail(char *s){
    int i = strlen(s);
    for (; i > 0; i--)
        if (s[i-1] == '/')
        {
            break;
        }
    printf("%s\n",s + i);
}

/*
 * Выполняет обход дерева каталогов, начиная с "fullpath".
 * Если "fullpath" не является каталогом, для него вызывается lstat(),
 * func() и затем выполняется возврат.
 * Для каталогов производится рекурсивный вызов функции.
 */
static int /* возвращает то, что вернула функция func() */dopath(Myfunc* func){
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int ret, n;
    
    if (lstat(fullpath, &statbuf) < 0) /* ошибка вызова функции stat */
    {
        switch (errno) {
            case EACCES: printf("eacces"); break;
            case EBADF: printf("fd is bad"); break;
            case EFAULT: printf("bad address"); break;
            case ELOOP: printf("too mane sym links"); break;
            case ENAMETOOLONG: printf("path is too long"); break;
            case ENOENT: printf("enoent"); break;
            case ENOMEM: printf("enomem"); break;
            case ENOTDIR: printf("enotdir"); break;
            default: printf("underfined error");
        }
        return(func(fullpath, &statbuf, FTW_NS));
    }
    
    if (S_ISDIR(statbuf.st_mode) == 0) /* не каталог */
    {
        tabs(lvl, 1);
        print_tail(fullpath);//, ++lvl);
        return(func(fullpath, &statbuf, FTW_F));
    }
    
    /*
    * Это каталог. Сначала вызвать функцию func(),
    * а затем обработать все файлы в этом каталоге.
    */
    if ((ret = func(fullpath, &statbuf, FTW_D)) != 0)
        return(ret);
    
    if (lvl){
        tabs(lvl++, 0);
        print_tail(fullpath);
    }
    else{
        lvl++;
        printf("%s\n", fullpath);
    }
    
    n = strlen(fullpath);
    if (n + NAME_MAX + 2 > pathlen){ /* увеличить размер буфера */
        pathlen *= 2;
        if ((fullpath = realloc(fullpath, pathlen)) == NULL)
            err_sys("ошибка вызова realloc");
    }
    
    fullpath[n++] = '/';
    fullpath[n] = 0;
    
    if ((dp = opendir(fullpath)) == NULL) /* каталог недоступен */
        return(func(fullpath, &statbuf, FTW_DNR));
    
    while ((dirp = readdir(dp)) != NULL){
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0 || (dirp->d_name[0] - '.') == 0)
            continue; /* пропустить каталоги "." и ".." */ // и игнорировать папки начинающиеся с '.'

        strcpy(&fullpath[n], dirp->d_name); /* добавить имя после слеша */
        //printf("%s\n", dirp->d_name);

        if ((ret = dopath(func)) != 0) /* рекурсия */
            break; /* выход по ошибке */
    }
    
    fullpath[n-1] = 0; /* стереть часть строки от слеша и до конца */
    
    if (closedir(dp) < 0)
        err_ret("невозможно закрыть каталог %s", fullpath);

    lvl--;
    return(ret);
}

static int myfunc(const char *pathname, const struct stat *statptr, int type){
    switch (type){
        case FTW_F:
            switch (statptr->st_mode & S_IFMT) {
                case S_IFREG: nreg++; break;
                case S_IFBLK: nblk++; break;
                case S_IFCHR: nchr++; break;
                case S_IFIFO: nfifo++; break;
                case S_IFLNK: nslink++; break;
                case S_IFSOCK: nsock++; break;
                case S_IFDIR: /* каталоги должны иметь type = FTW_D*/
            
                err_dump("признак S_IFDIR для %s", pathname);
            }
            break;
        case FTW_D:
            ndir++;
            break;
        case FTW_DNR:
            err_ret("закрыт доступ к каталогу %s", pathname);
            break;
        case FTW_NS:
            err_ret("ошибка вызова функции stat для %s", pathname);
            break;
        default:
            err_dump("неизвестный тип %d для файла %s", type, pathname);
    }
    return(0);
}
