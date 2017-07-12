#include <stdbool.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include <string.h>

#define SIZE_OF_T(TYPE)                   \
    do {                                  \
        if (0 == strcmp(type, #TYPE)) {   \
            return sizeof(TYPE);          \
        }                                 \
    } while (0)

#define SIZE_OF_S(TYPE)                   \
    do {                                  \
        if (0 == strcmp(type, #TYPE)) {   \
            return sizeof(struct TYPE);   \
        }                                 \
    } while (0)

size_t
size_of(const char* type) {
    // Builtin
    SIZE_OF_T(long);

    // sys/socket
    SIZE_OF_S(sockaddr_storage);
    SIZE_OF_S(msghdr);
    SIZE_OF_S(cmsghdr);

    // sys/uio
    SIZE_OF_S(iovec);

    return 0;
}

size_t
cmsg_space(size_t size) {
  return CMSG_SPACE(size);
}

size_t
cmsg_len(size_t size) {
  return CMSG_LEN(size);
}

size_t
cmsg_data_offset(void) {
  struct cmsghdr cmsg;
  return (size_t) CMSG_DATA(&cmsg) - (size_t) &cmsg;
}

void
cmsg_init(void* cmsghdr, size_t fds, size_t cmsgs) {
  struct msghdr msghdr = {0};
  struct cmsghdr* cmsg = cmsghdr;
  msghdr.msg_control = cmsg;
  msghdr.msg_controllen = CMSG_SPACE(fds * sizeof(int)) * cmsgs;
  memset(cmsghdr, 0, msghdr.msg_controllen);
  for (size_t i = 0; i < cmsgs; ++i) {
    cmsg->cmsg_len = CMSG_LEN(sizeof(int) * fds);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    int* data = (int *) CMSG_DATA(cmsg);
    for (size_t j = 0; j < fds; ++j) {
      data[j] = (int) 0xa0 + j;
    }
    cmsg = CMSG_NXTHDR(&msghdr, cmsg);
  }
}

bool
cmsg_valid(void* cmsghdr, size_t len) {
  struct msghdr msghdr = {0};
  struct cmsghdr* cmsg = cmsghdr;
  msghdr.msg_control = cmsg;
  msghdr.msg_controllen = len;
  cmsg = CMSG_FIRSTHDR(&msghdr);
  if (!cmsg) {
    return false;
  }
  while (cmsg) {
    if (cmsg->cmsg_level != SOL_SOCKET) {
      return false;
    }
    if (cmsg->cmsg_type != SCM_RIGHTS) {
      return false;
    }
    // How do we calculate this so it handles the post-cmsghdr padding on FreeBSD?
    size_t entries = (cmsg->cmsg_len - sizeof(struct cmsghdr)) / sizeof(int);
    int* fds = (int *) CMSG_DATA(cmsg);
    for (size_t i = 0; i < entries; ++i) {
      if (fds[i] != 0) {
        return false;
      }
    }
    cmsg = CMSG_NXTHDR(&msghdr, cmsg);
  }
  return true;
}
