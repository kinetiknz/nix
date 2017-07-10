pub use libc::{socket, listen, bind, accept, connect, setsockopt, sendto, recvfrom, getsockname, getpeername, recv, send, getsockopt, socketpair, sendmsg, recvmsg, msghdr, cmsghdr};

#[cfg(not(target_os = "macos"))]
use libc::size_t;

#[cfg(target_os = "macos")]
use libc::c_uint;

// OSX always aligns struct cmsghdr as if it were a 32-bit OS
#[cfg(target_os = "macos")]
pub type align_of_cmsg_data = c_uint;

#[cfg(not(target_os = "macos"))]
pub type align_of_cmsg_data = size_t;

