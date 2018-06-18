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

// Steal this idea from eXosip
typedef struct QUERY_BUFFER
{
    // See <arpa/nameser_compat.h>
    HEADER header;
    u_char msg[PACKETSZ-sizeof(HEADER)];
} QUERY_BUFFER;

static int parse_response(u_char *answer, int answer_len)
{
    HEADER *hp;             // Defined in <arpa/nameser_compat.h>
    unsigned char *msg;     // Pointer to start of answer
    unsigned char *eom;     // Pointer to end of answer
    unsigned char *cp;      // Current position within answer
    int num_queries;
    int ancount;
    int nscount;
    int arcount;
    char hostbuf[256];

    // Parse stuff from the header
    hp = (HEADER *)answer;
	num_queries = ntohs(hp->qdcount);
	ancount = ntohs(hp->ancount);       // Number of answers
	nscount = ntohs(hp->ancount);       // Number of nameservers (authoritative?)
	arcount = ntohs(hp->arcount);       // Number of resource records (A records?)

    fprintf(stderr, "queries: %d\n", num_queries);
    fprintf(stderr, "answers: %d\n", ancount);
    fprintf(stderr, "nameservers: %d\n", nscount);
    fprintf(stderr, "resource records: %d\n", arcount);

    msg = (unsigned char *)(answer);
    eom = (unsigned char *)(answer) + answer_len;
    cp = (unsigned char *)(answer) + sizeof(HEADER);    // The response data begins after the header

    // Parse domain names/hostnames from response
    while (num_queries-- > 0 && cp < eom)
    {
        int n;
        /*
         * int dn_expand(const unsigned char *msg,
         *           const unsigned char *eomorig,
         *           const unsigned char *comp_dn, char *exp_dn,
         *           int length);
         *
         * Expand a compressed domain name (comp_dn) into a full domain name.
         *
         * msg is a pointer to the start of the full response from res_query().
         * eomorig (original EOM) is a pointer to the end of the response from res_query().
         * comp_dn is a pointer within the answer - to the start of the compressed domain name.
         * Result is placed in buffer exp_dn; caller specifies the buffer length in length.
         *
         * Returns 0 on success and -1 on failure.
         */
        n = dn_expand(msg, eom, cp, hostbuf, sizeof(hostbuf));
        if (n < 0)
        {
            fprintf(stderr, "dn_expand() failed\n");
            return -1;
        }
        cp += (n + QFIXEDSZ);       // Apparently there is some "fixed data" in each response.
        fprintf(stderr, "hostbuf: %s\n", hostbuf);
    }
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

#if 0
    {
        u_char answer[PACKETSZ] = {0};
        const char *domain = "google.com";
        // const char *domain = "as2.iop1.broadworks.net";
        int class = C_IN;       // Internet
        // See <arpa/nameser_compat.h> for the complete list - there are a lot.
        int type = T_A;         // Address record
        // int type = T_NS;        // Name Server record
        // int type = T_NAPTR;     // Name Authority Pointer record
        // int type = T_SRV;       // Service record
        int rsp_len;

        // Called by gethostbyname() ?
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
        // Haven't gotten this to work yet
        rsp_len = res_search(domain, class, type, answer, sizeof(answer));
        if (rsp_len < 0)
        {
            herror("res_search");
            return;
        }
    }
#endif

    {
        u_char answer[PACKETSZ] = {0};
        const char *domain = "google.com";
        // const char *domain = "as2.iop1.broadworks.net";
        int class = C_IN;       // Internet
        // See <arpa/nameser_compat.h> for the complete list - there are a lot.
        int type = T_A;         // Address record
        // int type = T_NS;        // Name Server record
        // int type = T_NAPTR;     // Name Authority Pointer record
        // int type = T_SRV;       // Service record
        int rsp_len;

        /*
         * int res_query(const char *dname, int class, int type, unsigned char *answer, int anslen);
         *
         * dname        Domain name to be resolved
         * class        Class of desired record: C_IN (inernet)
         * type         Type of esired record. T_NS for name server record.
         * answer       Response will be placed here
         * anslen       Set to size of answer buffer
         *
         * Query the name server for the FQDN of the specified type and class.
         *
         * Returns the size of the response or -1 if there was an error.
         */
        rsp_len = res_query(domain, class, type, answer, sizeof(answer));
        if (rsp_len < 0)
        {
            herror("res_query");
            return;
        }
        printf("Response length: %d\n", rsp_len);
        hexprint(answer, rsp_len);

        parse_response(answer, rsp_len);
    }

    // Answer is binary data.
    printf("Success\n");
}

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
    deprecated_resolver();
    // test();

    return 0;
}
