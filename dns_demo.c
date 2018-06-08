#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>

// See http://beej.us/guide/bgnet/html/single/bgnet.html#syscalls
// See https://linux.die.net/man/3/getaddrinfo

int main(int argc, char *argv[])
{
    const char *hostname = argv[1];
    const char *port = "8484";

    /*
     * struct addrinfo {
     *      int ai_flags;
     *      int ai_family;
     *      int ai_socktype;
     *      int ai_protocol;
     *      socklen_t ai_addrlen;
     *      struct sockaddr *ai_addr;
     *      char *ai_canonname;
     *      struct addrinfo *ai_next;
     * };
     */
    struct addrinfo hints = {0};
    struct addrinfo *server_info = NULL;        // Resulting details about this server
    int status;

    hints.ai_family = AF_UNSPEC;                // IPv4 or IPv6 (or Unix?)
    hints.ai_socktype = SOCK_STREAM;            // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;                // fill in my IP automatically

    /*
     * int getaddrinfo(const char *node,           // hostname e.g. "www.example.com" or IP
     *             const char *service,            // e.g. "http", "ftp", "telnet"; or port number
     *             const struct addrinfo *hints,   // Criteria
     *             struct addrinfo **res);         // Result
     *
     * Returns 0 on success, < 0 on failure.
     */
    // Deprecated: call gethostbyname() and manually add info to a struct sockaddr_in.
    status = getaddrinfo(hostname, port, &hints, &server_info);
    if (status < 0)
    {
        fprintf( stderr, "Error resolving hostname: %s\n", gai_strerror(status) );
        freeaddrinfo(server_info);
        exit(1);
    }

    printf("IP addresses for %s:\n", hostname);

    // getaddrinfo() returns a linked list of results
    // Loop through the list and print the IP address for each.
    struct addrinfo *ptr = server_info;
    while (ptr != NULL)
    {
        void *addr;

        if (ptr->ai_family == AF_INET)
        {
            // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)ptr->ai_addr;
            addr = &(ipv4->sin_addr);
        }
        else
        {
            // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ptr->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        char ip_string[INET6_ADDRSTRLEN] = {0};
        // Network to presentation
        inet_ntop(ptr->ai_family, addr, ip_string, sizeof(ip_string));
        printf("\t%s\n", ip_string);

        ptr = ptr->ai_next;
    }

    printf("Tearing down\n");
    freeaddrinfo(server_info);

    printf("Done\n");
    return 0;
}
