# NAPTR Record

Name Authority Pointer record. Used to map servers and addresses.

## Background


### Purpose

An NAPTR explicitly specifies a domain name for which an SRV record exists.
NAPTRs exist for a given string, typically but not necessarily a domain name. They list SRV records, each of which points to an A (address) record, which in turn contains actual IP address.
For example:

```
NAPTR	=> SRV: _sip._udp.example.com
			   SRV: _sip._tcp.example.com
```

Notice that the SRV record contains service (SIP, email, etc) and protocol (TCP, UDP) information.


### Construction Details

An NAPTR record pecifies a "regex-based rewrite rule".
When given a string, this rule produces a new domain label or URI, i.e. it rewrites a *string* into a *domain name*.
This resulting label or URI may be:

  * another NAPTR
  * the final output

This system allows the Domain Name System to be used for strings or URIs that are not in domain name syntax.


### Format

NAPTR Resource Records use the following format (see [RFC 2915](https://tools.ietf.org/html/rfc2915))
```
domain. TTL class type order preference flags service regexp.
```

  * domain 			Domain name of this resource record. 'key' in the rule database. (has trailing dot)
  * TTL				Time To Live
  * class			always IN (internet)
  * type			always NAPTR (35)
  * order			u16 specifying order in which NAPTR records MUST be processed. Lowest first.
  * preference		u16 that ranks records with the same order. Lowest first.
  * flags			char string. S, A, U, P
  * service			service(s) available down this rewrite path.
  * regexp			the rewrite rule - a regex
  * replacement		the next name to query. Must be a FQDN. (has trailing dot)
  * flags
  * service
  * regexp


#### Flags

| Flag	| Description |
| ----- | ----------- |
| S		| Next lookup should be for SRV records
| A		| Next lookup should be for address record
| U		| Output of Regexp field is an absolute URI
| P		| Remainder of algorithm is protocol-specific

S, A, and U are 'terminal' flags. If these flags are not present, there is another NAPTR RR at the domain.


#### Service

`[ [protocol] *("+") rs]		# e.g. SIP+D2U or SIP+D2T`

Available protocols:

  * SIP
    - Used for standard SIP
  * SIPS
    - Used for TLS
  * mailto
  * etc

`rs` stands for "resolution service"

| Resolution Service    | Transport Layer Protocol	|
| --------------------- | ------------------------- |
| D2U					          | UDP			                  |
| D2T					          | TCP			                  |
| D2S					          | SCTP		                  |

See [RFC 3263](https://www.ietf.org/rfc/rfc3263.txt).


##### SIPS

From page 6, section "4.1 Selecting a Transport Protocol" in [RFC 3263](https://www.ietf.org/rfc/rfc3263.txt).
  * "A client resolving a SIP URI SHOULD retain records with "SIPS" as the protocol, if the client supports TLS"

Specifically, the service should be "SIPS+D2T" to indicate secure SIP (TLS) over a TCP connection (UDP is not allowed in this RFC).
The replacement will then be `_sips._tcp.example.com`.



#### Regex

No need to bother right now.



## Example

Several tools can be used to make an NAPTR query:

  * nslookup
  * dig
  * resolve?
  * getaddrinfo?
  * resolv C library?

On my current setup, the actual query is made from 127.0.0.1 to 127.0.0.53 (localhost to nameserver [local?]). Set a packet capture appropriately.


### dig


NAPTR lookup yields domain names of SRV records:

```bash
$ dig redas.iop1.broadworks.net NAPTR
# ...
;; ANSWER SECTION:
redas.iop1.broadworks.net. 299	IN	NAPTR	1 50 "s" "SIP+D2U" "" _sip._udp.redas.iop1.broadworks.net.
redas.iop1.broadworks.net. 299	IN	NAPTR	2 50 "s" "SIP+D2T" "" _sip._tcp.redas.iop1.broadworks.net.
# ...
```

SRV record yields domain names of A records:

```bash
$ dig _sip._tcp.redas.iop1.broadworks.net. SRV
# ...
;; ANSWER SECTION:
_sip._tcp.redas.iop1.broadworks.net. 5 IN SRV 4 50 5060 as2.iop1.broadworks.net.
_sip._tcp.redas.iop1.broadworks.net. 5 IN SRV 3 50 5060 dummy.iop1.broadworks.net.
```

A record yields an IP address:

```bash
$ dig as2.iop1.broadworks.net. A
# ...
;; ANSWER SECTION:
as2.iop1.broadworks.net. 3549 IN  A 199.19.193.11
```


### nslookup

```bash
$ nslookup -querytype=NAPTR redas.iop1.broadworks.net
Server:		127.0.0.53
Address:	127.0.0.53#53

Non-authoritative answer:
redas.iop1.broadworks.net	naptr = 1 50 "s" "SIP+D2U" "" _sip._udp.redas.iop1.broadworks.net.
redas.iop1.broadworks.net	naptr = 2 50 "s" "SIP+D2T" "" _sip._tcp.redas.iop1.broadworks.net.

Authoritative answers can be found from:
```


### Analysis

First record:

```bash
redas.iop1.broadworks.net. 299	IN	NAPTR	1 50 "s" "SIP+D2U" "" _sip._udp.redas.iop1.broadworks.net.
```

| Item			| Value 								|
| ------------- | ------------------------------------- |
| domain 		| `redas.iop1.broadworks.net`
| TTL			| 299
| class			| IN
| type			| NAPTR
| order			| 1
| preference	| 50
| flags			| s
| service		| SIP+D2U (SIP, UDP)
| regex			| ""
| replacement	| `_sip._udp.redas.iop1.broadworks.net`



Second record:

```bash
redas.iop1.broadworks.net. 299	IN	NAPTR	2 50 "s" "SIP+D2T" "" _sip._tcp.redas.iop1.broadworks.net.
```

| Item			| Value 								|
| ------------- | ------------------------------------- |
| domain 		| `redas.iop1.broadworks.net`
| TTL			| 299
| class			| IN
| type			| NAPTR
| order			| 2
| preference	| 50
| flags			| s
| service		| SIP+D2T (SIP, TCP)
| regex			| ""
| replacement	| `_sip._udp.redas.iop1.broadworks.net`



## Links

* [RFC 2915](https://tools.ietf.org/html/rfc2915)
* [Wikipedia](https://en.wikipedia.org/wiki/NAPTR_record)
* [Blog post](https://anders.com/cms/264/)
