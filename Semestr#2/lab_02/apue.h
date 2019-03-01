/*
 * Наш заголовочный файл, который подключается перед любыми
 * стандартными системными заголовочными файлами
 */
 
#ifndef _APUE_H
#define _APUE_H

#define _POSIX_C_SOURCE 200809L
#if defined(SOLARIS) /* Solaris 10 */
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 700
#endif

#include <sys/types.h> /* некоторые системы требуют этого заголовка */
#include <sys/stat.h>
#include <sys/termios.h> /* структура winsize */

#if defined(MACOS) || !defined(TIOCGWINSZ)
#include <sys/ioctl.h>
#endif

#include <stdio.h> /* для удобства */
#include <stdlib.h> /* для удобства */
#include <stddef.h> /* макрос offsetof */
#include <string.h> /* для удобства */
#include <unistd.h> /* для удобства */
#include <signal.h> /* константа SIG_ERR */

#define MAXLINE 4096 /* максимальная длина строки */

/*
 * Права доступа по умолчанию к создаваемым файлам.
 */
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*
 * Права доступа по умолчанию к создаваемым каталогам.
 */
#define DIR_MODE (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

typedef void Sigfunc(int); /* обработчики сигналов */

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

/*
 * Прототипы наших собственных функций.
 */
char *path_alloc(size_t *);                 /* листинг 2.3 */
long open_max(void);                        /* листинг 2.4 */

int set_cloexec(int);                       /* листинг 13.5 */
void clr_fl(int, int);
void set_fl(int, int);                      /* листинг 3.5 */

void pr_exit(int);                          /* листинг 8.5 */

void pr_mask(const char *);                 /* листинг 10.10 */
Sigfunc *signal_intr(int, Sigfunc *);       /* листинг 10.12 */

void daemonize(const char *);               /* листинг 13.1 */

void sleep_us(unsigned int);                /* упражнение 14.5 */
ssize_t readn(int, void *, size_t);         /* листинг 14.9 */
ssize_t writen(int, const void *, size_t);  /* листинг 14.9 */

int fd_pipe(int *);                         /* листинг 17.1 */
int recv_fd(int, ssize_t (*func)(int, 
            const void *, size_t));         /* листинг 17.10 */
int send_fd(int, int);                      /* листинг 17.9 */
int send_err(int, int,
                    const char *);          /* листинг 17.8 */
int serv_listen(const char *);              /* листинг 17.5 */
int serv_accept(int, uid_t *);              /* листинг 17.6 */
int cli_conn(const char *);                 /* листинг 17.7 */
int buf_args(char *, int (*func)(int,
             char **));                     /* листинг 17.19 */
 
int tty_cbreak(int);                        /* листинг 18.10 */
int tty_raw(int);                           /* листинг 18.10 */
int tty_reset(int);                         /* листинг 18.10 */
void tty_atexit(void);                      /* листинг 18.10 */
struct termios *tty_termios(void);          /* листинг 18.10 */

int ptym_open(char *, int);                 /* листинг 19.1 */
int ptys_open(char *);                      /* листинг 19.1 */
#ifdef TIOCGWINSZ
pid_t pty_fork(int *, char *, int, const struct termios *,
            const struct winsize *);        /* листинг 19.2 */
#endif

int lock_reg(int, int, int, off_t, int, off_t); /* листинг 14.2 */
#define read_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

pid_t lock_test(int, int, off_t, int, off_t); /* листинг 14.3 */

#define is_read_lockable(fd, offset, whence, len) \
            (lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)
#define is_write_lockable(fd, offset, whence, len) \
            (lock_test((fd), F_WRLCK, (offset), (whence), (len)) == 0)

void err_msg(const char *, ...); /* приложение B */
void err_dump(const char *, ...) __attribute__((noreturn));
void err_quit(const char *, ...) __attribute__((noreturn));
void err_cont(int, const char *, ...);
void err_exit(int, const char *, ...) __attribute__((noreturn));
void err_ret(const char *, ...);
void err_sys(const char *, ...) __attribute__((noreturn));

void log_msg(const char *, ...); /* приложение B */
void log_open(const char *, int, int);
void log_quit(const char *, ...) __attribute__((noreturn));
void log_ret(const char *, ...);
void log_sys(const char *, ...) __attribute__((noreturn));
void log_exit(int, const char *, ...) __attribute__((noreturn));
void TELL_WAIT(void); /* предок/потомок из раздела 8.9 */
void TELL_PARENT(pid_t);
void TELL_CHILD(pid_t);
void WAIT_PARENT(void);
void WAIT_CHILD(void);
#endif /* _APUE_H */
