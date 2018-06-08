#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

// See http://beej.us/guide/bgnet/html/single/bgnet.html#structs
// See http://beej.us/guide/bgnet/html/single/bgnet.html#syscalls

/*
 * struct addrinfo {
 *     int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
 *     int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
 *     int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
 *     int              ai_protocol;  // use 0 for "any"
 *     size_t           ai_addrlen;   // size of ai_addr in bytes
 *     struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
 *     char            *ai_canonname; // full canonical hostname
 *     struct addrinfo *ai_next;      // linked list, next node
 * };
 */

/*
 * struct sockaddr_in {
 *     short int          sin_family;  // Address family, AF_INET
 *     unsigned short int sin_port;    // Port number
 *     struct in_addr     sin_addr;    // Internet address
 *     unsigned char      sin_zero[8]; // Same size as struct sockaddr
 * };
 */


/*
 * Resolve the given hostname and service/port.
 *
 * Returns:
 *  On success, a pointer to a linked list of address info structs.
 *  On failure, NULL.
 * The caller must free the linked list using freeaddrinfo().
 */
struct addrinfo *resolve_hostname(const char *hostname, const char *service_or_port)
{
    struct addrinfo *server_info = NULL;
    struct addrinfo hints = {
        .ai_family = AF_INET,       // IPv4
        .ai_socktype = SOCK_STREAM, // TCP stream sockets
        .ai_flags = 0,
    };
    int status;

    status = getaddrinfo(hostname, service_or_port, &hints, &server_info);
    if (status < 0)
    {
        fprintf(stderr, "%s: Error: %s\n", __func__, gai_strerror(status));
        return NULL;
    }
    return server_info;
}

void print_ip_addresses(struct addrinfo *host_list)
{
    struct addrinfo *ptr = host_list;

    while (ptr != NULL)
    {
        char ip_string[INET_ADDRSTRLEN] = {0};
        struct sockaddr_in *ip_address = NULL;
        void *addr = NULL;

        if (ptr->ai_family != AF_INET)
        {
            fprintf(stderr, "Skipping unexpected address family: %d\n", ptr->ai_family);
            ptr = ptr->ai_next;
            continue;
        }

        ip_address = (struct sockaddr_in *)ptr->ai_addr;
        addr = &(ip_address->sin_addr);
        inet_ntop(ptr->ai_family, addr, ip_string, sizeof(ip_string));
        printf("%s\n", ip_string);

        ptr = ptr->ai_next;
    }
}

int main(int argc, char *argv[])
{
    const char *hostname = argv[1];
    const char *service = "8484";//NULL;

    struct addrinfo *server_info = NULL;

    printf("Resolving %s:%s\n", hostname, service);
    server_info = resolve_hostname(hostname, service);
    if (server_info == NULL)
    {
        return -1;
    }

    print_ip_addresses(server_info);

    freeaddrinfo(server_info);
    return 0;
}
