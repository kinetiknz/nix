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
