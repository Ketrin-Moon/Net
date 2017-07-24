#include "sniffer.h"

void process(u_char *args, struct pcap_pkthdr* pkthdr, const u_char* packet)
{
	ethernet(args, pkthdr, packet);
	if(etype == ETHERTYPE_IP)
		print_ip_header(args, pkthdr, packet);
	else if(etype == ETHERTYPE_ARP)
		printf("\n(ARP)\n");
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
	int size_body;
	int tmp_csum;

        ip = (struct my_ip *)(packet + sizeof(struct ether_header));
        printf("\n----------------------------------IP Header----------------------------------- \n\n");
        printf("\t|-Desrination IP : %s\n", inet_ntoa(ip->ip_dst));
        printf("\t|-Source IP : %s\n", inet_ntoa(ip->ip_src));
        if(ip->ip_p == IPPROTO_TCP)
                printf("\t|-Protocol : TCP\n");
        else if(ip->ip_p == IPPROTO_UDP)
                printf("\t|-Protocol : UDP\n");
	tmp_csum = ntohs(ip->ip_sum);
	printf("\t|-Length header : %d\n", IP_HL(ip));
	size_body = ntohs(ip->ip_len) - IP_HL(ip);
	printf("\t|-Length body : %d\n", size_body);
//	ip->ip_sum = 0;
	f_chsum(ip);
	printf("Old csum: %d New csum: %d\n", tmp_csum, ntohs(ip->ip_sum));

}

void f_chsum(struct my_ip *ip)
{
	ip->ip_sum = 0;
	ip->ip_sum = chsum((unsigned short *)ip, IP_HL(ip));
}

unsigned short chsum(unsigned short *addr, unsigned int size)
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
//	csum = ~csum;
//	return ((unsigned short)csum);
	return ~csum;
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
	pcap_loop(handle, -1, process, NULL);

	pcap_close(handle);

	return 0;
}
