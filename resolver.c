#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>      // for herror()

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
    int type = T_NS;        // Name Server record

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
}

int main(void)
{

    /*
     * Old resolver functions are non-reentrant; they store global state in a local struct _res.
     * New resolver functions are reentrant: they accept the state as an argument.
     */
    deprecated_resolver();

    return 0;
}
