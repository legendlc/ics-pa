#include "fs.h"

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;          // 文件名
  size_t size;         // 文件大小
  size_t disk_offset;  // 文件在ramdisk中的偏移
  ReadFn read;
  WriteFn write;
  size_t open_offset;  // 文件被打开之后的读写指针
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, invalid_read, serial_write},
  {"/dev/events", 0, 0, events_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode)
{
  int fd = -1;
  assert(pathname);
  for (int i = 0; i < NR_FILES; i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      fd = i;
      break;
    }
  }

  return fd;
}

ssize_t fs_read(int fd, void *buf, size_t len) {
  assert(fd >= 0 && fd < NR_FILES);
  assert(buf);

  if (file_table[fd].read) {
    // don't care about offset
    return file_table[fd].read(buf, 0, len);
  }

  size_t remain = file_table[fd].size - file_table[fd].open_offset;
  len = (remain < len ? remain : len);

  //printk("[read] read %x bytes from offset %x to %p\n", 
  //  len, file_table[fd].open_offset, buf);
  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;

  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  assert(fd >= 0 && fd < NR_FILES);
  size_t ret = (size_t)-1;

  // due to our fs cannot change size, seek beyond file is not allowed
  if (whence == SEEK_SET) {
    assert(offset <= file_table[fd].size);
    //printk("[lseek(set)] set offset to %x\n", offset);
    file_table[fd].open_offset = offset;
    ret = offset;
  } else if (whence == SEEK_CUR) {
    assert(file_table[fd].open_offset + offset <= file_table[fd].size);
    //printk("[lseek(cur)] set offset to %x\n", file_table[fd].open_offset + offset);
    file_table[fd].open_offset += offset;
    ret = file_table[fd].open_offset;
  } else if (whence == SEEK_END) {
    assert(offset == 0);
    //printk("[lseek(end)] set offset to %x\n", file_table[fd].size + offset);
    file_table[fd].open_offset = file_table[fd].size + offset;
    ret = file_table[fd].open_offset;
  } else {
    assert(0);
  }

  return ret;
}

ssize_t fs_write(int fd, const void *buf, size_t len) {
  assert(fd >= 0 && fd < NR_FILES);
  assert(buf);
  
  if (file_table[fd].write) {
    // serial_write do not care offset 
    return file_table[fd].write(buf, 0, len);
  }

  assert(file_table[fd].open_offset + len <= file_table[fd].size);
  //printk("[write] Write %x bytes to offset %x\n", len, file_table[fd].open_offset);
  ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;

  return len;
}

int fs_close(int fd) {
  assert(fd >= 0 && fd < NR_FILES);
  file_table[fd].open_offset = 0;

  return 0;
}