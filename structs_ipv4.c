/*
 * Simplified:
 *
 * struct addrinfo
 * {
 *      int ai_flags;
 *      int ai_family;
 *      int ai_socktype;
 *      int ai_protocol;
 *      unsigned int ai_addrlen;
 *      struct sockaddr *ai_addr;
 *      char *ai_canonname;
 *      struct addrinfo *ai_next;
 * };
 *
 * // Can be cast to sockaddr_in
 * struct sockaddr
 * {
 *      unsigned short sa_family;
 *      char sa_data[14];
 * };
 *
 * // Can be cast to struct sockaddr
 * struct sockaddr_in
 * {
 *      unsigned short sin_family;
 *      uint16_t port;
 *      struct in_addr sin_addr;
 *      unsigned char sin_zero[pad_to_size_of_sockaddr];
 * };
 *
 * struct in_addr
 * {
 *      uint32_t s_addr;
 * };
 */

/*
 * <netdb.h>
 *
 * struct addrinfo
 * {
 *   int ai_flags;			    // Input flags.
 *   int ai_family;		        // Protocol family for socket.
 *   int ai_socktype;		    // Socket type.
 *   int ai_protocol;		    // Protocol for socket.
 *   socklen_t ai_addrlen;		// Length of socket address.
 *   struct sockaddr *ai_addr;	// Socket address for socket.
 *   char *ai_canonname;		// Canonical name for service location.
 *   struct addrinfo *ai_next;	// Pointer to next in list.
 * };
 */

/*
 * <bits/socket.h>
 *
 * typedef __socklen_t socklen_t;
 */

/*
 * <bits/types.h>

 * __STD_TYPE __U32_TYPE __socklen_t;
 */

/*
 * <bits/types.h>
 *
 * #define __U32_TYPE		unsigned int
 */

/*
 * <bits/socket.h>
 *
 * struct sockaddr
 *   {
 *     __SOCKADDR_COMMON (sa_);	// Common data: address family and length.
 *     char sa_data[14];		// Address data.
 *   };
 *
 * Macro expansion:
 *  __SOCKADDR_COMMON (sa_)  =>  sa_family_t sa_family
 *
 * So
 * struct sockaddr {
 *      sa_family_t sa_family
 *      char sa_data[14];
 * This structure is generic - sa_data can contain IPv4 or IPv6 addresses (or more).
 * It can safely be cast to:
 *      struct sockaddr_in      Internet, IPv4
 *      struct sockaddr_in6     Internet, IPv6
 */

/*
 * <bits/sockaddr.h>
 *
 * typedef unsigned short int sa_family_t;
 *
 * #define	__SOCKADDR_COMMON(sa_prefix) \
 *   sa_family_t sa_prefix##family
 */

/*
 * <netinet/in.h>
 *
 * struct sockaddr_in
 *   {
 *     __SOCKADDR_COMMON (sin_);
 *     in_port_t sin_port;			// Port number.
 *     struct in_addr sin_addr;		// Internet address.
 *
 *     // Pad to size of `struct sockaddr'.
 *     unsigned char sin_zero[sizeof (struct sockaddr) -
 * 			   __SOCKADDR_COMMON_SIZE -
 * 			   sizeof (in_port_t) -
 * 			   sizeof (struct in_addr)];
 *   };
 */

/*
 * <netinet/in.h>
 *
 * typedef uint32_t in_addr_t;
 * struct in_addr
 *   {
 *     in_addr_t s_addr;
 *   };
 */

/*
 * <netinet/in.h>
 *
 * typedef uint16_t in_port_t;
 */
int main(void)
{
    return 0;
}
