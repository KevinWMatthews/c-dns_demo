#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>      // for herror()
#include <string.h>


void hexprint(unsigned char *buffer, int length)
{
    char hexbuf[1024] = {0};
    char tempbuf[4] = {0};
    unsigned char hexchar = 0;  // unsigned to prevent occasional leading 0xffff
    int i = 0;

    if (!buffer)
    {
        printf("%s: Passed a null buffer\n", __func__);
        return;
    }
    if ( length > sizeof(hexbuf) / 3 )  // We use 3 ascii characters per hex byte: 'xx '
        length = sizeof(hexbuf) / 3;

    for (i=0; i<length; i++)
    {
        if ((i > 0) && (i % 16 == 0))
        {
            printf("%s\n", hexbuf);
            memset(hexbuf, 0, sizeof(hexbuf));
        }
        hexchar = (unsigned char)*(buffer+i);
        sprintf(tempbuf, "%02x ", hexchar);
        strncat(hexbuf, tempbuf, 3);
    }
    printf("%s\n", hexbuf);
}

void new_resolver(void)
{
    // Declare struct to hold resolver state.
    struct __res_state state_struct = {0};
    res_state state = &state_struct;
    int status;
}

void deprecated_resolver(void)
{
    int status;

    /*
     * int res_init(void)
     *
     * Get domain name and name server address by reading /etc/resolv.conf, /etc/host.conf (?)
     *
     * Normally executed by the first call to other resolver functions?
     */
    status = res_init();
    if (status < 0)
    {
        herror("res_init");
        return;
    }

    u_char answer[PACKETSZ] = {0};
    const char *domain = "google.com";
    // const char *domain = "as2.iop1.broadworks.net";
    int class = C_IN;       // Internet
    // See <arpa/nameser_compat.h> for the complete list - there are a lot.
    int type = T_A;         // Address record
    // int type = T_NS;        // Name Server record
    // int type = T_NAPTR;     // Name Authority Pointer record
    // int type = T_SRV;       // Service record

    /*
     * int res_search(const char *dname, int class, int type, unsigned char *answer, int anslen);
     *
     * dname        Domain name to be resolved
     * class        Class of desired record: C_IN (inernet)
     * type         Type of esired record. T_NS for name server record.
     * answer       Response will be placed here
     * anslen       Set to size of answer buffer
     *
     * Returns the size of the response or -1 if there was an error.
     */
    // Called by gethostbyname()
#if 0
    // Haven't gotten this to work yet
    status = res_search(domain, class, type, answer, sizeof(answer));
    if (status < 0)
    {
        herror("res_search");
        return;
    }
#endif

    /*
     * int res_query(const char *dname, int class, int type, unsigned char *answer, int anslen);
     *
     * dname        Domain name to be resolved
     * class        Class of desired record: C_IN (inernet)
     * type         Type of esired record. T_NS for name server record.
     * answer       Response will be placed here
     * anslen       Set to size of answer buffer
     *
     * Returns the size of the response or -1 if there was an error.
     */
    status = res_query(domain, class, type, answer, sizeof(answer));
    if (status < 0)
    {
        herror("res_query");
        return;
    }
    // Answer is binary data.
    printf("Success\n");
}

typedef struct QUERY_BUFFER
{
    // See <arpa/nameser_compat.h>
    HEADER header;
    u_char msg[PACKETSZ-sizeof(HEADER)];
} QUERY_BUFFER;

void test(void)
{
    QUERY_BUFFER query_buffer = {0};
    const char *domain = "ns1-g.jnctn.net";//"sip.onsip.com";
    int class = C_IN;
    int type;
    int status;

    status = res_init();
    if (status < 0)
    {
        herror("res_init");
        return;
    }

    type = T_NAPTR;
    status = res_query(domain, class, type, (u_char *)&query_buffer, sizeof(query_buffer));
    if (status < 0)
    {
        herror("res_query");
        return;
    }
    // Answer is binary data.
    hexprint(query_buffer.msg, sizeof(query_buffer.msg));
    printf("Success\n");
}

int main(void)
{

    /*
     * Old resolver functions are non-reentrant; they store global state in a local struct _res.
     * New resolver functions are reentrant: they accept the state as an argument.
     */
    // deprecated_resolver();
    test();

    return 0;
}
