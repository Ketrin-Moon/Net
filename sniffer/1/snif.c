#include "sniffer.h"

void process(u_char *args, struct pcap_pkthdr* pkthdr, const u_char* packet)
{
	ethernet(args, pkthdr, packet);
	if(etype == ETHERTYPE_IP)
		print_ip_header(args, pkthdr, packet);
	else if(etype == ETHERTYPE_ARP)
		printf("\n(ARP)\n");

}

void print_udp_header(u_char *args, struct pcap_pkthdr *pkthdr, const u_char *packet, int len, struct my_ip *ip)
{
//        struct my_udphdr *udp;
//        int iplen = sizeof(struct ether_header) + sizeof(struct my_ip);

	struct udphdr *udp;
	struct ps_header *ps;
	int tmp_csum, new_csum = 0;
        u_short iplen;
	u_char *udp_packet;
	int udp_packet_len;
	int header_size;

	iplen = IP_HL(ip)*4;
	ps = malloc(sizeof(struct ps_header));
	udp = (struct udphdr *)(packet + iplen + sizeof(struct ether_header));
	header_size = sizeof(struct ether_header) + iplen + sizeof(udp);
	udp_packet_len = 12 + ntohs(udp->len);
	udp_packet = malloc(udp_packet_len);

        printf("\n-----------------------------------UDP Header---------------------------------- \n\n");
        printf("\t|-Destination port : %d\n", ntohs(udp->dest));
        printf("\t|-Source port : %d\n", ntohs(udp->source));
	printf("\t|-UDP length : %d\n", ntohs(udp->len));
    
	ps->source = ip->ip_src;
	ps->dest = ip->ip_dst;
	ps->res = 0;
	ps->prot = ip->ip_p;
	ps->len = htons(ntohs(ip->ip_len) - iplen);

	tmp_csum = ntohs(udp->check);
	udp->check = 0;

	memcpy(udp_packet, ps, 12);
	memcpy(udp_packet + 12, udp, ntohs(udp->len));

	new_csum = checksum((unsigned short*)udp_packet, udp_packet_len);
	udp_packet = 0; udp_packet_len = 0;
	printf("\t|-Old csum : 0x%X\n\t|-New csum : 0x%X\n", tmp_csum, ntohs(new_csum));
	new_csum = 0;

}

void print_tcp_header(u_char *args, struct pcap_pkthdr *pkthdr, const u_char *packet, int len, struct my_ip *ip)
{
        struct tcphdr *tcp;
	struct ps_header *ps;
	int tmp_csum, new_csum = 0;
        u_short iplen;
	u_char *tcp_packet;
	int tcp_packet_len;
	int header_size;

	iplen = IP_HL(ip)*4;
	ps = malloc(sizeof(struct ps_header));
	tcp = (struct tcphdr *)(packet + iplen + sizeof(struct ether_header));
	header_size = sizeof(struct ether_header) + iplen + tcp->doff*4;
	tcp_packet_len = 12 + tcp->doff*4 + len - header_size;
	tcp_packet = malloc(tcp_packet_len);

        printf("\n-----------------------------------TCP Header---------------------------------- \n\n");
        printf("\t|-Destination port : %d\n", ntohs(tcp->dest));
        printf("\t|-Source port : %d\n", ntohs(tcp->source));
	printf("\t|-Sequence number : %u\n", ntohs(tcp->seq));
	printf("\t|-Acknowlege number : %u\n", ntohs(tcp->ack_seq));
	printf("\t|-Window : %d\n", ntohs(tcp->window));
	printf("\t|-Urgent Pointer : %d\n", ntohs(tcp->urg_ptr));

	ps->source = ip->ip_src;
	ps->dest = ip->ip_dst;
	ps->res = 0;
	ps->prot = ip->ip_p;
	ps->len = htons(ntohs(ip->ip_len) - iplen);

	tmp_csum = ntohs(tcp->check);
	tcp->check = 0;

	memcpy(tcp_packet, ps, 12);
	memcpy(tcp_packet + 12, tcp, tcp->doff*4);
	memcpy(tcp_packet + 12 + tcp->doff*4, packet + header_size, len - header_size);

	new_csum = checksum((unsigned short*)tcp_packet, tcp_packet_len);
	tcp_packet = 0; tcp_packet_len = 0;
	printf("\t|-Old csum : 0x%X\n\t|-New csum : 0x%X\n", tmp_csum, ntohs(new_csum));
	new_csum = 0;
}

void ethernet(u_char *args, struct pcap_pkthdr* pkthdr, const u_char* packet)
{
	struct ether_header *eth;
	int size = pkthdr->len;
	eth = (struct ether_header *)packet;
	int offset = sizeof(struct ether_header);


	printf("\n-----------------------------Ethernet Header----------------------------------- \n\n");
	printf("\t|-Destination address : %s\n", ether_ntoa(eth->ether_dhost));
	printf("\t|-Source address : %s\n", ether_ntoa(eth->ether_shost));
	printf("\t|-Protocol: %.4X\n", ntohs(eth->ether_type));
       	printf("\t|-Size header : %d\n", size);
	printf("\t|-Size data payload : %d\n\n", size - offset);
	printData(packet + offset, size - offset);
	etype = ntohs(eth->ether_type);
}

void print_ip_header(u_char *args, struct pcap_pkthdr* pkthdr, const u_char* packet)
{
        struct my_ip* ip;
	int tmp_csum;
	int flag = 0;

        ip = (struct my_ip *)(packet + sizeof(struct ether_header));
        printf("\n----------------------------------IP Header------------------------------------\n\n");
        printf("\t|-IP version : %d\n", IP_V(ip));
	printf("\t|-Length header : %d\n", IP_HL(ip));
	printf("\t|-Type of service : %d\n", ip->ip_tos);
//	printf("\t|-Protocol : %d\n", ip->ip_p);
        printf("\t|-Destination IP : %s\n", inet_ntoa(ip->ip_dst));
        printf("\t|-Source IP : %s\n", inet_ntoa(ip->ip_src));


        if(ip->ip_p == IPPROTO_UDP){
                printf("\t|-Protocol : UDP(%d)\n", ip->ip_p);
		flag = 1;
	}else if(ip->ip_p == IPPROTO_TCP){
		printf("\t|-Protocol : TCP(%d)\n", ip->ip_p);
		flag = 2;
	}

	tmp_csum = ntohs(ip->ip_sum);
	ip->ip_sum = 0;
	ip_checksum(ip);
	printf("\t|-Old csum: 0x%x \n\t|-New csum: 0x%x\n", tmp_csum, ntohs(ip->ip_sum));

	if(flag == 1)
		print_udp_header(args, pkthdr, packet, pkthdr->len, ip);
	else if(flag == 2)
		print_tcp_header(args, pkthdr, packet, pkthdr->len, ip);

}

void ip_checksum(struct my_ip *ip)
{
	ip->ip_sum = 0;
	ip->ip_sum = checksum((unsigned short *)ip, IP_HL(ip)<<2);
}

unsigned short checksum(unsigned short *addr, unsigned int size)
{
	register unsigned long csum = 0;

	while(size > 1){
		csum += *addr++;
		size -= 2;
	}
	if(size > 0)
		csum += ((*addr)&htons(0xFF00));
	while(csum >> 16)
		csum = (csum & 0xffff) + (csum >> 16);
	csum = ~csum;

	return ((unsigned short)csum);
}
void printData(const u_char* packet, int size)
{
	int i, j;
	int offset = 0;
	int nlines = size / 16;
	if(nlines * 16 < size)
		nlines++;
	printf("        ");
	for(i = 0; i < 16; i++)
		printf("%02X ", i);
	printf("\n\n");
	for(i = 0; i < nlines; i++){
		printf("        ");
		for(j = 0; j < 16; j++){
			if(offset + j >= size)
				printf("   ");
			else
				printf("%02X ", packet[offset + j]);
		}
		printf("    ");
		for(j = 0; j < 16; j++){
			if(offset + j >= size)
				printf("  ");
			else if(packet[offset + j] > 31 && packet[offset + j] < 127)
				printf("%c", packet[offset + j]);
			else
				printf(".");
		}
		offset += 16;
		printf("\n");
	}
}

int main()
{
	pcap_t *handle;
	char *device, errbuf[MAXSIZE];
	struct in_addr addr;
	bpf_u_int32 net, mask;
	struct bpf_program fp;

	device = pcap_lookupdev(errbuf);

	printf("Device: %s\n", device);

	if(pcap_lookupnet(device, &net, &mask, errbuf) < 0){
		perror("Pcap_lookupnet");
		exit(1);
	}
	addr.s_addr = net;
	printf("Net: %s\t", inet_ntoa(addr));
	addr.s_addr = mask;
	printf("Mask: %s\n", inet_ntoa(addr));

	handle = pcap_open_live(device, 65535, 1, 0, errbuf);

	if(pcap_compile(handle, &fp, "ip", 0, mask)==-1) {
			printf("Error pcap_compile()\n"); exit(1); }
	if(pcap_setfilter(handle, &fp)==-1) {
			printf("Error pcap_setfilter\n"); exit(1); }
	pcap_loop(handle, -1, (void*)process, NULL);

	pcap_close(handle);

	return 0;
}
